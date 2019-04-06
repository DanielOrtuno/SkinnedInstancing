#include "AudioManager.h"
#include "EventManager.h"
#include "EnumTypes.h"

#include <fstream>

GW::AUDIO::GSound** CAudioManager::m_cSoundList;
GW::AUDIO::GMusic** CAudioManager::m_cMusicList;
GW::AUDIO::GAudio* CAudioManager::m_pAudio;

bool CAudioManager::m_bMuted;
float CAudioManager::m_nMasterVol, CAudioManager::m_nMusicVol, CAudioManager::m_nSFXVol;


CAudioManager::CAudioManager()
{
	m_cSoundList = new GW::AUDIO::GSound*[eSFX::COUNT];
	m_cMusicList = new GW::AUDIO::GMusic*[eMusic::COUNT];

	std::ifstream file("Settings.txt");
	if (file.is_open())
	{
		file >> m_nMasterVol;
		file >> m_nMusicVol;
		file >> m_nSFXVol;
	}
	else
	{
		m_nMasterVol = m_nMusicVol = m_nSFXVol = 1.0F;
	}
	file.close();
	std::string msg = "Master vol: " + std::to_string(m_nMasterVol) + "\nMusic vol: " + std::to_string(m_nMusicVol) + "\nSFX vol: " + std::to_string(m_nSFXVol);
	CEventManager::SendDebugMessage(TDebugMessage(msg));
	m_bMuted = false;
	if (GW::GReturn::FAILURE == GW::AUDIO::CreateGAudio(&m_pAudio))
	{
		exit(-1);
	}
	m_pAudio->SetMasterVolume(m_nMasterVol);

	AddMusic("..//Assets//Sounds//Doom.wav", eMusic::MAIN);
	AddSFX("..//Assets//Sounds//Button.wav", eSFX::BUTTON);
	AddSFX("..//Assets//Sounds//footsteps.wav", eSFX::PLAYERMOVE);
	AddSFX("..//Assets//Sounds//Hit-1.wav", eSFX::HIT);
	AddSFX("..//Assets//Sounds//Hit-2.wav", eSFX::HIT2);
	AddSFX("..//Assets//Sounds//Hit-3.wav", eSFX::HIT3);
	AddSFX("..//Assets//Sounds//Hit-4.wav", eSFX::HIT4);
	AddSFX("..//Assets//Sounds//FireBall-1.wav", eSFX::FIREBALL1);
	AddSFX("..//Assets//Sounds//FireBall-2.wav", eSFX::FIREBALL2);
}

CAudioManager::~CAudioManager()
{
	std::ofstream file("Settings.txt");
	if (file.is_open())
	{
		file << std::to_string(m_nMasterVol) << '\n';//<< " Master Volume\n";
		file << std::to_string(m_nMusicVol) << '\n';//<< " Music Volume\n";
		file << std::to_string(m_nSFXVol) << '\n';//<< " SFX Volume\n
	}
	file.close();

	for (size_t i = 0; i < eMusic::COUNT; i++)
	{
		m_cMusicList[i]->SetVolume(0);
		m_cMusicList[i]->ResumeStream();
		if (GW::GReturn::FAILURE == m_cMusicList[i]->DecrementCount())
		{
			CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music failed to Decrement."));
		}
	}
	for (size_t i = 0; i < eSFX::COUNT; i++)
	{
		if (GW::GReturn::FAILURE == m_cSoundList[i]->DecrementCount())
		{
			CEventManager::SendDebugMessage(TDebugMessage("ERROR. Sound failed to Decrement."));
		}
	}
	unsigned int size = 0;
	m_pAudio->GetCount(size);
	for (size_t i = 0; i < size; i++)
	{
		m_pAudio->DecrementCount();
	}

	delete[] m_cMusicList;
	delete[] m_cSoundList;
}

void CAudioManager::AddMusic(const char* _pchFilepath, int nType, bool bLoop)
{
	GW::AUDIO::GMusic* pMusic;

	if (GW::GReturn::FAILURE == m_pAudio->CreateMusicStream(_pchFilepath, &pMusic))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music failed to initialize from: " + std::string(_pchFilepath)));
	}

	if (GW::GReturn::FAILURE == pMusic->SetVolume(m_nMusicVol))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music Vol failed to set from: " + std::string(_pchFilepath)));
	}

	if (GW::GReturn::FAILURE == pMusic->StreamStart(bLoop))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music stream failed to start from: " + std::string(_pchFilepath)));
	}

	m_cMusicList[nType] = pMusic;
	CEventManager::SendDebugMessage(TDebugMessage::TDebugMessage(std::string(_pchFilepath) + " added to music"));
}

void CAudioManager::ChangeMusicVol(float fVal)
{
	m_nMusicVol = fVal;
	for (size_t i = 0; i < eMusic::COUNT; i++)
	{
		if (GW::GReturn::FAILURE == m_cMusicList[i]->SetVolume(m_nMusicVol))
		{
			CEventManager::SendDebugMessage(TDebugMessage("ERROR. Music Vol failed to change."));
		}
	}
}

void CAudioManager::AddSFX(const char * _pchFilepath, int nType)
{
	GW::AUDIO::GSound* pSFX = nullptr;

	if (GW::GReturn::FAILURE == m_pAudio->CreateSound(_pchFilepath, &pSFX))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. SFX stream failed to start from: " + std::string(_pchFilepath)));
	}

	if (GW::GReturn::FAILURE == pSFX->SetVolume(m_nSFXVol))
	{
		CEventManager::SendDebugMessage(TDebugMessage("ERROR. SFX failed to set vol from: " + std::string(_pchFilepath)));
	}
	m_cSoundList[nType] = pSFX;
	CEventManager::SendDebugMessage(TDebugMessage(std::string(_pchFilepath) + " added to sounds."));
}

void CAudioManager::ReceiveSoundRequest(bool bTrue, int nType, int nSound)
{
	switch (nType)
	{
	case eAudio::SFX:
	{
		if (bTrue)
		{
			if (nSound == eSFX::BUTTON,eSFX::PLAYERMOVE)
			{
				bool bTemp = false;
				m_cSoundList[nSound]->isSoundPlaying(bTemp);
				if (!bTemp)
				{
					m_cSoundList[nSound]->Play();
				}
			}
			else
			{
				m_cSoundList[nSound]->Play();
			}
			//else
			//{
			//	bool bTemp = false;
			//	m_cSoundList[nSound]->isSoundPlaying(bTemp);
			//	if (!bTemp)
			//	{
			//		m_cSoundList[nSound]->Play();
			//	}
			//}
		}
		else
		{
			m_cSoundList[nSound]->Pause();
		}
	}
	break;
	case eAudio::MUSIC:
	{
		if (bTrue)
		{
			m_cMusicList[nSound]->StreamStart(true);
		}
		else
		{
			m_cMusicList[nSound]->StopStream();
		}
	}
	break;

	default:
		break;
	}
}

void CAudioManager::ChangeSFXVol(float fVal)
{
	m_nSFXVol = fVal;
	for (size_t i = 0; i < eSFX::COUNT; i++)
	{
		m_cSoundList[i]->SetVolume(m_nSFXVol);
	}
}

void CAudioManager::ChangeMasterVol(float fVal)
{
	m_nMasterVol = fVal;
	m_pAudio->SetMasterVolume(fVal);
	CEventManager::SendDebugMessage(TDebugMessage(std::string("Master volume changed to ") + std::to_string(m_nMasterVol)));
	ChangeMusicVol(m_nMusicVol);
	ChangeSFXVol(m_nSFXVol);
}

void CAudioManager::ToggleMusic()
{

	m_bMuted = !m_bMuted;
	if (m_bMuted)
	{
		for (size_t i = 0; i < eMusic::COUNT; i++)
		{
			m_cMusicList[i]->ResumeStream();
		}
	}
	else
	{
		for (size_t i = 0; i < eMusic::COUNT; i++)
		{
			m_cMusicList[i]->PauseStream();
		}
	}
}

float CAudioManager::GetMasterVol()
{
	return m_nMasterVol;
}

float CAudioManager::GetMusicVol()
{
	return m_nMusicVol;
}

float CAudioManager::GetSFXVol()
{
	return m_nSFXVol;
}
