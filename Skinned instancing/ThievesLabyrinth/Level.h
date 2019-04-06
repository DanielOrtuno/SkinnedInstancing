#pragma once
#include "EntityManager.h"

struct Spot
{
	bool bRoom;

	CRoomEntity* Room;

	std::vector<Spot*> m_cNeighbors;
};

class CLevel
{
private:

	static std::vector<CRoomEntity*> m_pcRooms;
	
	int nWidth = 100;
	int nRows = 10;
	int nColumns = 10;

	int nEndY = 0;
	int nEndX = 0;
	int nstart = 0;

	
	
public:
	
	static CRoomEntity* m_pcCurrentRoom;
	static CRoomEntity* m_pcStartRoom;
	static CRoomEntity* m_pcEndRoom;
	static Spot CurrentSpot;

	static void Update(CRoomEntity* room);

	static void UpdateDoors(CRoomEntity* Room);

	static void SetDoorsTrue(CRoomEntity* room);

	static int GetTotalEnemyCount();

	static int nTotalEnemyCount;

	CLevel();
	~CLevel();
};

