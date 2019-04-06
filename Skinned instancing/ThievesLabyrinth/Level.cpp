//#include "Level.h"
#include "LevelManager.h"
#include "GUIManager.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"

#include "Transform.h"
#include "Entity.h"

#include <time.h>
#include <iostream>
#include <vector>
#include <list>

std::vector<CRoomEntity*> CLevel::m_pcRooms;
CRoomEntity* CLevel::m_pcCurrentRoom;
CRoomEntity* CLevel::m_pcStartRoom;
CRoomEntity* CLevel::m_pcEndRoom;
Spot CLevel::CurrentSpot;

int CLevel::nTotalEnemyCount = 0;

std::vector < std::vector < Spot > > cbMaze;
std::list < std::pair < int, int> > cnTunnels;

int nWidth = 100;
int nRows = 10;
int nColumns = 10;

CLevel::CLevel()
{
	cbMaze.resize(nColumns);
	for (int y = 0; y < nColumns; y++)
	{
		cbMaze[y].resize(nRows);
	}

	for (int x = 0; x < nRows; x++)
	{
		for (int y = 0; y < nColumns; y++)
		{
			cbMaze[y][x].bRoom = false;
		}
	}

	cnTunnels.push_back(std::make_pair(nRows / 2, nColumns / 2));

	while (cnTunnels.size() > 0)
	{
		std::list < std::pair < int, int> >::iterator  cnBegin, cnEnd, cnTemp;
		cnBegin = cnTunnels.begin();
		cnEnd = cnTunnels.end();

		while (cnBegin != cnEnd)
		{
			bool bRemoveTunnel = false;
			switch (rand() % 4)
			{
			case 0:
			{
				(*cnBegin).second -= 2;

				if ((*cnBegin).second < 0 || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;
					break;
				}

				cbMaze[(*cnBegin).second + 1][(*cnBegin).first].bRoom = true;

				break;
			}

			case 1:
			{
				(*cnBegin).second += 2;

				if ((*cnBegin).second >= nColumns || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;

					break;
				}

				cbMaze[(*cnBegin).second - 1][(*cnBegin).first].bRoom = true;

				break;
			}

			case 2:
			{
				(*cnBegin).first -= 2;

				if ((*cnBegin).first < 0 || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;
					break;
				}

				cbMaze[(*cnBegin).second][(*cnBegin).first + 1].bRoom = true;

				break;
			}

			case 3:
			{
				(*cnBegin).first += 2;

				if ((*cnBegin).first >= nRows || cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom == true)
				{
					bRemoveTunnel = true;

					break;
				}

				cbMaze[(*cnBegin).second][(*cnBegin).first - 1].bRoom = true;

				break;
			}

			}
			if (bRemoveTunnel)
			{
				cnBegin = cnTunnels.erase(cnBegin);
			}
			else
			{
				cnTunnels.push_back(std::make_pair((*cnBegin).first, (*cnBegin).second));
				// uncomment to make easier 
				/*if (rand() % 2)
				{
					Tunnels.push_back(std::make_pair((*Begin).first, (*Begin).second));
				}*/

				cnTunnels.push_back(std::make_pair((*cnBegin).first, (*cnBegin).second));

				cbMaze[(*cnBegin).second][(*cnBegin).first].bRoom = true;

				++cnBegin;
			}
		}
	}

	for (int x = 0; x < nRows; x++)
	{
		for (int y = 0; y < nColumns; y++)
		{
			if (cbMaze[y][x].bRoom == true)
			{
				CRoomEntity* room = (CRoomEntity*)CEntityManager::CloneEntity(CLevelManager::m_pRoomsType[0]);
				

				switch (rand() % 4)
				{
				case 1:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
					break;
				}

				case 2:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90.0f));
					break;
				}

				case 3:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 180.0f));
					break;
				}

				case 4:
				{
					room->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 270.0f));
					break;
				}
				default:

					break;
				}

				cbMaze[y][x].Room = room;

				room->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3((float)y * nWidth, 0, (float)x * nWidth));

				room->SetActiveState(true);
				

				m_pcRooms.push_back(room);
			}
			else
			{
				//Spawn wall prefab
			}
		}
	}

	for (int i = 0; i < m_pcRooms.size(); i++)
	{
		nTotalEnemyCount += m_pcRooms[i]->m_nEnemyCount;
	}

	for (int x = 0; x < nRows; x++)
	{
		for (int y = 0; y < nColumns; y++)
		{
			if (cbMaze[y][x].bRoom == true)
			{
				if (y - 1 >= 0)
				{
					if (cbMaze[y - 1][x].bRoom == true)
					{
						cbMaze[y - 1][x].Room->Bottom = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y - 1][x].Room);
						cbMaze[y][x].Room->Top = true;
					}
				}

				if (y + 1 < nColumns)
				{
					if (cbMaze[y + 1][x].bRoom == true)
					{
						cbMaze[y + 1][x].Room->Top = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y + 1][x].Room);
						cbMaze[y][x].Room->Bottom = true;
					}
				}

				if (x - 1 >= 0)
				{
					if (cbMaze[y][x - 1].bRoom == true)
					{
						cbMaze[y][x - 1].Room->Right = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y][x - 1].Room);
						cbMaze[y][x].Room->Left = true;
					}
				}

				if (x + 1 < nRows)
				{
					if (cbMaze[y][x + 1].bRoom == true)
					{
						cbMaze[y][x + 1].Room->Left = true;
						cbMaze[y][x].Room->m_pcNeighbors.push_back(cbMaze[y][x + 1].Room);
						cbMaze[y][x].Room->Right = true;
					}
				}

				if (nstart == 0)
				{
					CurrentSpot = cbMaze[y][x];
					m_pcStartRoom = cbMaze[y][x].Room;
					nstart++;
				}

				m_pcEndRoom = cbMaze[y][x].Room;

			}
			else
			{

			}
		}
	}

	m_pcCurrentRoom = CurrentSpot.Room;

	CurrentSpot.Room->SpawnEntities();
	CurrentSpot.Room->SpawnDoors();

	CGUIManager::SetMiniMap(cbMaze);

	for (int i = 0; i < CurrentSpot.Room->m_pcNeighbors.size(); i++)
	{
		CurrentSpot.Room->m_pcNeighbors[i]->SpawnEntities();
		CurrentSpot.Room->m_pcNeighbors[i]->SpawnDoors();
	}
}


CLevel::~CLevel()
{
	/*for (int i = 0; i < m_pcRooms.size(); i++)
	{
		CEntityManager::AddEntityToDeletionQueue(m_pcRooms[i]);

		for (int j = 0; j < m_pcRooms[i]->m_pcEnemies.size(); j++)
		{
			CEntityManager::AddEntityToDeletionQueue(m_pcRooms[i]->m_pcEnemies[j]);
		}
	}*/
	CEntityManager::DeleteAllEntities();

	for (int i = 0; i < nColumns; i++)
	{
		for (int j = 0; j < nRows; j++)
		{
			cbMaze[i][j].m_cNeighbors.clear();
		}
	}

	m_pcRooms.clear();

	nEndX = 0;
	nEndY = 0;
	nstart = 0;



}

void CLevel::Update(CRoomEntity* Room)
{
	m_pcCurrentRoom = Room;

	for (int i = 0; i < Room->m_pcNeighbors.size(); i++)
	{
		Room->m_pcNeighbors[i]->SpawnDoors();
		Room->m_pcNeighbors[i]->SpawnEntities();
	}

	if (Room->m_nEnemyCount != 0)
	{
		if (Room->Right == true)
		{
			Room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Left == true)
		{
			Room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Top == true)
		{
			Room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Bottom == true)
		{
			Room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}

		Room->TurnEnemiesOn();
	}

	if (Room->m_nEnemyCount == 0)
	{
		if (Room->Right == true)
		{
			Room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Left == true)
		{
			Room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Top == true)
		{
			Room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Bottom == true)
		{
			Room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
	}

	nTotalEnemyCount = 0;
	for (int i = 0; i < m_pcRooms.size(); i++)
	{
		nTotalEnemyCount += m_pcRooms[i]->m_nEnemyCount;
	}

}

void CLevel::UpdateDoors(CRoomEntity* Room)
{
	if (Room->m_nEnemyCount == 0)
	{
		CEventManager::SendDebugMessage(TDebugMessage("Doors Open"));

		if (Room->Right == true)
		{
			Room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Left == true)
		{
			Room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Top == true)
		{
			Room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
		if (Room->Bottom == true)
		{
			Room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(true);
		}
	}
}

void CLevel::SetDoorsTrue(CRoomEntity * room)
{
	
	if (room->m_nEnemyCount != 0)
	{
		CEventManager::SendDebugMessage(TDebugMessage("Doors Closed"));

		if (room->Right == true)
		{
			room->m_pcDoors[2]->GetComponent<CBoxCollider>()->SetTrigger(false);
		}
		if (room->Left == true)
		{
			room->m_pcDoors[3]->GetComponent<CBoxCollider>()->SetTrigger(false);
		}
		if (room->Top == true)
		{
			room->m_pcDoors[1]->GetComponent<CBoxCollider>()->SetTrigger(false);
		}
		if (room->Bottom == true)
		{
			room->m_pcDoors[0]->GetComponent<CBoxCollider>()->SetTrigger(false);
		}
	}

	
}

int CLevel::GetTotalEnemyCount()
{
	return nTotalEnemyCount;
}
