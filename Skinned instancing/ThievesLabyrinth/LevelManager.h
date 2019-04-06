#pragma once
#include "Level.h"

class CLevelManager
{
private:

public:
	static CLevel* m_pcCurrentLevel;

	static std::vector<CRoomEntity*> m_pRoomsType;

	void CreateLevel();

	CLevelManager();
	~CLevelManager();
};

