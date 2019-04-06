#include "EventManager.h"
#include "DebugManager.h"
#include "GUIManager.h"
#include "AudioManager.h"
#include "RenderManager.h"

#include "Entity.h"

#include <iostream>

std::vector<CPlayerEntity>				CEventManager::m_cEntityReceivers;
std::queue<TEntityMessage>				CEventManager::m_cEntityMessages;

std::queue<TDebugMessage>*				CEventManager::m_cDebugMessages;
std::queue<TDebugLineMessage>*			CEventManager::m_cDebugLineMessages;
std::queue<TAudioMessage>*				CEventManager::m_cAudioMessages;
std::queue<TWindowResizeMessage>*		CEventManager::m_cResizeMessages;
std::queue<TCollisionMessage>*			CEventManager::m_cCollisionMessages;
std::queue<THudMessage>*				CEventManager::m_cHudMessages;

bool TAudioMessage::bTrue;
int TAudioMessage::nType, TAudioMessage::nSound;



#pragma region DebugMessage

//Debug Message
TDebugMessage::TDebugMessage(std::string szMessage)
{
	m_szMessageString = szMessage;
}

std::string TDebugMessage::GetDebugMessage()
{
	return m_szMessageString;
}

#pragma endregion

#pragma region DebugLine
//DebugLineMessage
TDebugLineMessage::TDebugLineMessage(CMath::TVECTOR3 _tPosition, CMath::TVECTOR4 _tColor)
{
	m_tPosition = _tPosition;
	m_tColor = _tColor;
}

CMath::TVECTOR3 TDebugLineMessage::GetPosition()
{
	return m_tPosition;
}

CMath::TVECTOR4 TDebugLineMessage::GetColor()
{
	return m_tColor;
}

#pragma endregion

#pragma region Audio	

//Audio Message
TAudioMessage::TAudioMessage(bool _bTrue, int _nType, int _nSound)
{
	bTrue = _bTrue;
	nType = _nType;
	nSound = _nSound;
}

bool TAudioMessage::GetTruth()
{
	return bTrue;
}

int TAudioMessage::GetType()
{
	return nType;
}

int TAudioMessage::GetSound()
{
	return nSound;
}

#pragma endregion

#pragma region EntityMessage

//Entity Message
TEntityMessage::TEntityMessage(int _nEntityID)
{
	nEntityID = _nEntityID;
}

int TEntityMessage::GetID()
{
	return nEntityID;
}

#pragma endregion

#pragma region Collision

TCollisionMessage::TCollisionMessage(int _nCollider, int _nCollidingWith, int _nCollisionType)
{
	nCollider = _nCollider;
	nCollidingWith = _nCollidingWith;
	nCollisionType = _nCollisionType;
}

int TCollisionMessage::GetCollider()
{
	return nCollider;
}

int TCollisionMessage::GetCollidingWith()
{
	return nCollidingWith;
}

int TCollisionMessage::GetCollisionType()
{
	return nCollisionType;
}

#pragma endregion

#pragma region EventManager
CEventManager::CEventManager()
{
	m_cDebugMessages = new std::queue<TDebugMessage>;
	m_cDebugLineMessages = new std::queue<TDebugLineMessage>;
	m_cAudioMessages = new std::queue<TAudioMessage>;
	m_cResizeMessages = new std::queue<TWindowResizeMessage>;
	m_cCollisionMessages = new std::queue<TCollisionMessage>;
	m_cHudMessages = new std::queue<THudMessage>;

#ifdef MULTI_THREADING

	m_nThreadCount = 0;
	m_nWorkingThreads = 0;
	m_bShutDownFlag = false;
	for (size_t i = 0; i < 1; i++)
	{

		m_Threads.push_back(std::thread(&CEventManager::ParseDebugMessage, this));
		m_Threads.push_back(std::thread(&CEventManager::ParseDebugLineMessage, this));
		m_Threads.push_back(std::thread(&CEventManager::ParseAudioMessage, this));
		m_Threads.push_back(std::thread(&CEventManager::ParseCollissionMessage, this));
		m_Threads.push_back(std::thread(&CEventManager::ParseResizeMessage, this));
		m_Threads.push_back(std::thread(&CEventManager::ParseHUDMessage, this));
		m_nThreadCount += 6;
	}

#endif // MULTI_THREADING



}
CEventManager::~CEventManager()
{
	delete m_cDebugMessages;
	delete m_cDebugLineMessages;
	delete m_cAudioMessages;
	delete m_cResizeMessages;
	delete m_cCollisionMessages;
	delete m_cHudMessages;


#ifdef MULTI_THREADING

	m_cMainMutex.lock();
	m_bShutDownFlag = true;
	m_cGunCondition.notify_all();
	m_cMainMutex.unlock();

	for (auto& thread : m_Threads)
	{
		thread.join();
	}

#endif // MULTI_THREADING


}

//EventManager
void CEventManager::AddPlayerReceiver(CPlayerEntity tReceiver)
{
	m_cEntityReceivers.push_back(tReceiver);
}

void CEventManager::SendDebugMessage(TDebugMessage tMessage)
{
	m_cDebugMessages->push(tMessage);
}

void CEventManager::SendDebugLineMessage(TDebugLineMessage tMessage)
{
	m_cDebugLineMessages->push(tMessage);
}

void CEventManager::SendAudioMessage(TAudioMessage tMessage)
{
	m_cAudioMessages->push(tMessage);
}

void CEventManager::SendResizeMessage(TWindowResizeMessage tMessage)
{
	if (m_cResizeMessages)
	{
		m_cResizeMessages->push(tMessage);
	}
}

void CEventManager::SendCollisionMessage(TCollisionMessage tMessage)
{
	//std::cout << "Entity " << tMessage.GetCollider() << " and " << tMessage.GetCollidingWith() << " are in collision type " << tMessage.GetCollisionType() << std::endl;
	m_cCollisionMessages->push(tMessage);
}

IEntity* CEventManager::SendEntityMessage(TEntityMessage tMessage)
{
	//SendDebugMessage(TDebugMessage(std::string(std::to_string(CEntityManager::GetEntity(tMessage.GetID())->m_nEntityType) + "Returned")));
	return CEntityManager::GetEntity(tMessage.GetID());
}

#ifdef MULTI_THREADING

void CEventManager::ParseDebugMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);

	m_cMainMutex.lock();
	std::cout << "Thread: " << std::this_thread::get_id() << " started in ParseDebugMessage\n";
	m_cMainMutex.unlock();

	while (!m_bShutDownFlag)
	{
		while (!m_cDebugMessages->empty())
		{
			CDebugManager::DebugLog(m_cDebugMessages->front());

			m_cDebugMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseDebugLineMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);

	m_cMainMutex.lock();
	std::cout << "Thread: " << std::this_thread::get_id() << " started in ParseDebugLineMessage\n";
	m_cMainMutex.unlock();

	while (!m_bShutDownFlag)
	{
		while (!m_cDebugLineMessages->empty())
		{
			CDebugManager::AddDebugVertex(m_cDebugLineMessages->front());

			m_cDebugLineMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseAudioMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);

	m_cMainMutex.lock();
	std::cout << "Thread: " << std::this_thread::get_id() << " started in ParseAudioMessage\n";
	m_cMainMutex.unlock();

	while (!m_bShutDownFlag)
	{
		while (!m_cAudioMessages->empty())
		{

			CAudioManager::ReceiveSoundRequest(m_cAudioMessages->front().GetTruth(), m_cAudioMessages->front().GetType(), m_cAudioMessages->front().GetSound());


			m_cAudioMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseCollissionMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);

	m_cMainMutex.lock();
	std::cout << "Thread: " << std::this_thread::get_id() << " started in ParseCollisionMessage\n";
	m_cMainMutex.unlock();

	while (!m_bShutDownFlag)
	{
		while (!m_cCollisionMessages->empty())
		{
			TCollisionMessage m_tMessage = m_cCollisionMessages->front();

			CEntityManager::NotifyCollisionMessage(m_tMessage.GetCollider(), m_tMessage.GetCollidingWith(), m_tMessage.GetCollisionType());

			m_cCollisionMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseResizeMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);

	m_cMainMutex.lock();
	std::cout << "Thread: " << std::this_thread::get_id() << " started in ParseResizeMessage\n";
	m_cMainMutex.unlock();

	while (!m_bShutDownFlag)
	{
		while (!m_cResizeMessages->empty())
		{
			CRenderManager::ResizeEvent((float)m_cResizeMessages->front().GetWidth(), (float)m_cResizeMessages->front().GetHeight());

			m_cResizeMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

void CEventManager::ParseHUDMessage()
{
	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);
	m_cGunCondition.wait(cGunLock);

	m_cMainMutex.lock();
	std::cout << "Thread: " << std::this_thread::get_id() << " started in ParseHUDMessage\n";
	m_cMainMutex.unlock();

	while (!m_bShutDownFlag)
	{
		while (!m_cHudMessages->empty())
		{
			THudMessage m_tMessage = m_cHudMessages->front();

			CGUIManager::SetPlayerValues(m_tMessage.GetCurrentHealth(), m_tMessage.GetMaxHealth(), m_tMessage.GetCurrentMana(), m_tMessage.GetMaxMana());

			m_cHudMessages->pop();
		}
		m_cMainMutex.lock();
		m_nWorkingThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
}

#endif // MULTI_THREADING
#pragma endregion

#pragma region WindowResize

TWindowResizeMessage::TWindowResizeMessage(unsigned int _nWidth, unsigned int _nHeight)
{
	nWidth = _nWidth;
	nHeight = _nHeight;
}

int TWindowResizeMessage::GetWidth()
{
	return nWidth;
}

int TWindowResizeMessage::GetHeight()
{
	return nHeight;
}

#pragma endregion

#pragma region HUD

THudMessage::THudMessage(float _fCurrHealth, float _fMaxHealth, float _fCurrMana, float _fMaxMana)
{
	fCurrHealth = _fCurrHealth;	fMaxHealth = _fMaxHealth;
	fCurrMana = _fCurrMana;		fMaxMana = _fMaxMana;
}

float THudMessage::GetCurrentHealth()
{
	return fCurrHealth;
}

float THudMessage::GetMaxHealth()
{
	return fMaxHealth;
}

float THudMessage::GetCurrentMana()
{
	return fCurrMana;
}

float THudMessage::GetMaxMana()
{
	return fMaxMana;
}

void CEventManager::SendHudMessage(THudMessage cMessage)
{
	m_cHudMessages->push(cMessage);
}

#pragma endregion

void CEventManager::Notify()
{
#ifdef  MULTI_THREADING

	std::unique_lock<std::mutex> cULock(m_cMainMutex);


	m_cGunCondition.notify_all();
	m_cMainWait.wait(cULock, [&]()
	{
		return m_nThreadCount == m_nWorkingThreads;
	});
	cULock.unlock();

	m_nWorkingThreads = 0;


#endif	
#ifndef MULTI_THREADING

	while (!m_cDebugMessages->empty())
	{
		CDebugManager::DebugLog(m_cDebugMessages->front());

		m_cDebugMessages->pop();
	}

	while (!m_cDebugLineMessages->empty())
	{
		CDebugManager::AddDebugVertex(m_cDebugLineMessages->front());

		m_cDebugLineMessages->pop();
	}

	while (!m_cAudioMessages->empty())
	{

		CAudioManager::ReceiveSoundRequest(m_cAudioMessages->front().GetTruth(), m_cAudioMessages->front().GetType(), m_cAudioMessages->front().GetSound());


		m_cAudioMessages->pop();
	}

	while (!m_cResizeMessages->empty())
	{
		CRenderManager::ResizeEvent((float)m_cResizeMessages->front().GetWidth(), (float)m_cResizeMessages->front().GetHeight());

		m_cResizeMessages->pop();
	}

	while (!m_cCollisionMessages->empty())
	{
		TCollisionMessage m_tMessage = m_cCollisionMessages->front();

		CEntityManager::NotifyCollisionMessage(m_tMessage.GetCollider(), m_tMessage.GetCollidingWith(), m_tMessage.GetCollisionType());

		m_cCollisionMessages->pop();
	}

	while (!m_cHudMessages->empty())
	{
		THudMessage m_tMessage = m_cHudMessages->front();

		CGUIManager::SetPlayerValues(m_tMessage.GetCurrentHealth(), m_tMessage.GetMaxHealth(), m_tMessage.GetCurrentMana(), m_tMessage.GetMaxMana());

		m_cHudMessages->pop();
	}
#endif // !MULTI_THREADING
}