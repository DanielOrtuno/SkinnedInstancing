#pragma once
#include "GAudio.h"
#include "System.h"


class CAudioManager : public ISystem
{
	

	static GW::AUDIO::GAudio*	 m_pAudio;
	static GW::AUDIO::GSound**	 m_cSoundList;
	static GW::AUDIO::GMusic**	 m_cMusicList;
	
	static bool m_bMuted;
	static float m_nMusicVol, m_nSFXVol, m_nMasterVol;

public:
	CAudioManager();
	~CAudioManager();

	///////////////////////////////////////////////////////////////////
	// AddMusic()	 
	// in:	const char*, bool
	// out: void

	// adds music from passed in file path and adds it to a vector, loops with bool
	///////////////////////////////////////////////////////////////////

	void AddMusic(const char*, int, bool = false);

	///////////////////////////////////////////////////////////////////
	// AddSFX()	 
	// in:	const char*
	// out: void
	// adds SFX from passed in file path and adds it to a vector
	///////////////////////////////////////////////////////////////////
	void AddSFX(const char*,int);

	///////////////////////////////////////////////////////////////////
	// ChangeMusicVol()	 
	// in:	float
	// out: void
	// Changes music's vol from passed in float Value
	///////////////////////////////////////////////////////////////////
	static void ChangeMusicVol(float);


	///////////////////////////////////////////////////////////////////
	// ChangeSFXVol()	 
	// in:	float
	// out: void
	// Changes SFX's vol from passed in float Value
	///////////////////////////////////////////////////////////////////
	static void ChangeSFXVol(float);

	///////////////////////////////////////////////////////////////////
	// ChangeMasterVol()	 
	// in:	float
	// out: void
	// Changes Master vol from passed in float Value
	///////////////////////////////////////////////////////////////////
	static void ChangeMasterVol(float);

	///////////////////////////////////////////////////////////////////
	// ReceiveSoundRequest()	 
	// in:	bool, int, int (optional)
	// out: void
	// Prases sound request and plays music/sound
	///////////////////////////////////////////////////////////////////
	static void ReceiveSoundRequest(bool bTrue, int nType, int nSFX = 0);


	///////////////////////////////////////////////////////////////////
	// ToggleMusic()	 
	// in:	void
	// out: void
	// Mutes and unmutes music
	///////////////////////////////////////////////////////////////////
	static void ToggleMusic();


	static float GetMasterVol();
	static float GetMusicVol();
	static float GetSFXVol();
};
