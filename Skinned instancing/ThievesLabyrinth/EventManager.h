#pragma once
#include "System.h"
#include "EntityManager.h"
#include "ComponentManager.h"
//#include "Entity.h"
//#include "AudioManager.h"
//#include "RenderManager.h"


#include <string>
#include <queue>
#include <vector>

class CPlayerEntity;

struct TDebugMessage
{
public:
	TDebugMessage(std::string szMessage);

	std::string GetDebugMessage();

private:

	std::string m_szMessageString;
};

struct TDebugLineMessage
{
public:
	TDebugLineMessage(CMath::TVECTOR3 _tPosition, CMath::TVECTOR4 _tColor);
	CMath::TVECTOR3 GetPosition();
	CMath::TVECTOR4 GetColor();

private:
	CMath::TVECTOR3 m_tPosition;
	CMath::TVECTOR4 m_tColor;
};

struct TAudioMessage
{
public:
	TAudioMessage(bool _bTrue, int _nType, int _nSound = 0);

	static bool GetTruth();
	static int GetType();
	static int GetSound();

	
private:
	static bool bTrue;
	static int nType, nSound;
};

struct TEntityMessage
{
public:
	TEntityMessage(int _nEntityID);

	int GetID();

private:
	int nEntityID;
	IEntity* Entity;
};

struct TWindowResizeMessage
{
public:
	TWindowResizeMessage(unsigned int _nWidth, unsigned int _nHeight);
	int GetWidth();
	int GetHeight();

private:
	int nWidth;
	int nHeight;
};

struct TCollisionMessage
{
public:
	TCollisionMessage(int _nCollider, int _nCollidingWith, int _nCollisionType);
	int GetCollider();
	int GetCollidingWith();
	int GetCollisionType();

private:
	int nCollider;
	int nCollidingWith;
	int nCollisionType;
};

struct THudMessage
{
public:
	THudMessage(float _fCurrHealth, float _fMaxHealth, float _fCurrMana, float _fMaxMana);
	float GetCurrentHealth();
	float GetMaxHealth();
	float GetCurrentMana();
	float GetMaxMana();

private:
	float fCurrHealth;
	float fMaxHealth;
	float fCurrMana;
	float fMaxMana;
};

class CEventManager : ISystem
{
public:

	CEventManager();
	~CEventManager();

	void AddPlayerReceiver(CPlayerEntity cReceiver);
	
	static void SendDebugMessage(TDebugMessage cMessage);

	static void SendDebugLineMessage(TDebugLineMessage cMessage);

	static void SendAudioMessage(TAudioMessage cMessage);

	static void SendResizeMessage(TWindowResizeMessage cMessage);

	static void SendCollisionMessage(TCollisionMessage cMessage);

	static IEntity* SendEntityMessage(TEntityMessage cMessage);

	static void SendHudMessage(THudMessage cMessage);

	void Notify();



private:

	void ParseDebugMessage();
	void ParseDebugLineMessage();
	void ParseAudioMessage();
	void ParseCollissionMessage();
	void ParseResizeMessage();
	void ParseHUDMessage();

	static std::vector<CPlayerEntity>			m_cEntityReceivers;
	static std::queue<TEntityMessage>			m_cEntityMessages;
	
	static std::queue<TDebugMessage>*			m_cDebugMessages;
	static std::queue<TDebugLineMessage>*		m_cDebugLineMessages;
	static std::queue<TAudioMessage>*			m_cAudioMessages;
	static std::queue<TWindowResizeMessage>*	m_cResizeMessages;
	static std::queue<TCollisionMessage>*		m_cCollisionMessages;
	static std::queue<THudMessage>*				m_cHudMessages;


#ifdef MULTI_THREADING

	std::mutex									m_cMainMutex;
	std::mutex									m_cMutexLock;

	std::condition_variable						m_cGunCondition;
	std::condition_variable						m_cMainWait;

	std::vector<std::thread>					m_Threads;
	bool										m_bShutDownFlag;

	int											m_nThreadCount;
	int											m_nWorkingThreads;

#endif // MULTI_THREADING

	
};

