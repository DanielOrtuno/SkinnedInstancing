#include "PhysicsManager.h"
#include "InputManager.h"
#include "EventManager.h"

#include "Time.h"
#include "Level.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Rigidbody.h"
#include "Entity.h"
#include "Transform.h"

#include <iostream>
#include <limits>

#define OUT
#define COLLISION_OFFSET 0.02f

std::vector<ICollider*> CPhysicsManager::m_cDynamicColliders;
std::vector<ICollider*> CPhysicsManager::m_cStaticColliders;
CPhysicsManager::TPlane CPhysicsManager::m_FloorPlane;

CPhysicsManager::CPhysicsManager(CComponentManager* pComponentManager)
{
	m_pcComponentManager = pComponentManager;
	m_pcCurrentRoom = nullptr;
	m_pcPlayerEntity = nullptr;
	m_pcTransforms = m_pcComponentManager->GetTransforms();
	m_pcRigidbodies = m_pcComponentManager->GetRigidbodies();
	m_pcColliders = m_pcComponentManager->GetColliders();
	m_FloorPlane.tNormal = CMath::TVECTOR3(0, 1, 0);
	m_FloorPlane.fDistance = 0.0f;

#ifdef MULTI_THREADING

	m_iFinishedThreads = 0; 
	m_iNumThreads = 0;
	m_dDelta = 0.0;
	m_bShutDownFlag = false;

	for (size_t i = 0; i < 5; i++)
	{
		threads.push_back(std::thread(&CPhysicsManager::UpdateRigidBodies, this, 0.0));
		m_iNumThreads++;
	}

#endif // MULTI_THREADING

}
CPhysicsManager::~CPhysicsManager()
{

#ifdef MULTI_THREADING

	m_cMainMutex.lock();
	m_bShutDownFlag = true;
	m_cGunCondition.notify_all();
	m_cMainMutex.unlock();

	for (auto& thread : threads)
	{
		thread.join();
	}

#endif // 

	
}


void CPhysicsManager::UpdateComponentLists()
{
#pragma region Debug Draw Grid

	if (m_pcCurrentRoom != nullptr)
	{
		static const int nCellCountX = ROOM_WIDTH / CELL_SIZE;
		static const int nCellCountZ = ROOM_HEIGHT / CELL_SIZE;
		static const int nCellSize = CELL_SIZE;

		CMath::TVECTOR3 RoomPosition = m_pcCurrentRoom->GetComponent<CTransform>()->GetPosition();

		for (int i = 0; i <= nCellCountX; i++)
		{
			float offset = i - (float)(nCellCountX) / 2.0f;
			float fXPosition = RoomPosition.x + (float)nCellSize * offset;

			TDebugLineMessage vertex = TDebugLineMessage(CMath::TVECTOR3(fXPosition, 1.0f, RoomPosition.z + (float)ROOM_HEIGHT / 2.0f), CMath::TVECTOR4{ 0, 0.5f, 1, 1 });
			CEventManager::SendDebugLineMessage(vertex);

			vertex = TDebugLineMessage(CMath::TVECTOR3(fXPosition, 1.0f, RoomPosition.z - (float)ROOM_HEIGHT / 2.0f), CMath::TVECTOR4{ 0, 0.5f, 1, 1 });
			CEventManager::SendDebugLineMessage(vertex);

		}

		for (int i = 0; i <= nCellCountZ; i++)
		{
			float offset = i - (float)(nCellCountZ) / 2.0f;
			float fZPosition = RoomPosition.z + (float)nCellSize * offset;

			TDebugLineMessage vertex = TDebugLineMessage(CMath::TVECTOR3(RoomPosition.x + (float)ROOM_WIDTH / 2.0f, 1.0f, fZPosition), CMath::TVECTOR4{ 0, 0.5f, 1, 1 });
			CEventManager::SendDebugLineMessage(vertex);

			vertex = TDebugLineMessage(CMath::TVECTOR3(RoomPosition.x - (float)ROOM_WIDTH / 2.0f, 1.0f, fZPosition), CMath::TVECTOR4{ 0, 0.5f, 1, 1 });
			CEventManager::SendDebugLineMessage(vertex);

		}
	}
#pragma endregion

	if (!m_pcPlayerEntity)
	{
		TEntityMessage message = TEntityMessage(0);
		m_pcPlayerEntity = (CPlayerEntity*)CEventManager::SendEntityMessage(message);
	}

	if (m_pcCurrentRoom != CLevel::m_pcCurrentRoom)
	{
		m_pcCurrentRoom = CLevel::m_pcCurrentRoom;
		m_tRoomPosition = m_pcCurrentRoom->GetComponent<CTransform>()->GetPosition();
		m_StaticGridPositions.clear();
	}

	m_DynamicGridPositions.clear();
	m_cDynamicColliders.clear();
	m_cStaticColliders.clear();

	for (ICollider* collider : *m_pcColliders)
	{
		if (collider->IsActive())
		{
			if (collider->m_pcOwner->m_pcRoom == m_pcCurrentRoom)
			{
				if (!collider->m_pcOwner->GetComponent<CRigidbody>())
				{
					m_cStaticColliders.push_back(collider);

					if (m_StaticGridPositions.count(collider) < 1)
					{
						CMath::TVECTOR3 tColliderPosition = collider->m_pcOwner->GetComponent<CTransform>()->GetPosition();
						int nGridXPosition = FindGridPositionX(tColliderPosition, 0, ROOM_WIDTH / CELL_SIZE - 1);
						int nGridZPosition = FindGridPositionZ(tColliderPosition, 0, ROOM_HEIGHT / CELL_SIZE - 1);

						m_StaticGridPositions[collider].push_back(TCoordinate(nGridXPosition, nGridZPosition));

						int nMaxGridXPosition, nMaxGridZPosition;
						int nMinGridXPosition, nMinGridZPosition;
						nMaxGridXPosition = nMinGridXPosition = nGridXPosition;
						nMaxGridZPosition = nMinGridZPosition = nGridZPosition;

						if (collider->m_nComponentType == eComponent::BOX_COLLIDER)
						{
							CBoxCollider* pBoxCollider = (CBoxCollider*)collider;

							CTransform* pcTransform = collider->m_pcOwner->GetComponent<CTransform>();

							CMath::TMATRIX tMatrix = pcTransform->GetMatrix();
							tMatrix._41 = tMatrix._42 = tMatrix._43 = 0.0f;

							float fExtentX = CMath::Vector3Dot(CMath::TVECTOR3(1, 0, 0), CMath::Vector3Transform(pBoxCollider->GetExtent(), tMatrix)) + 0.2f;
							float fExtentZ = CMath::Vector3Dot(CMath::TVECTOR3(0, 0, 1), CMath::Vector3Transform(pBoxCollider->GetExtent(), tMatrix)) + 0.2f;

							nMaxGridXPosition = FindGridPositionX(pcTransform->GetPosition() + CMath::TVECTOR3(1, 0, 0) * fabs(fExtentX), 0, ROOM_WIDTH / CELL_SIZE - 1);
							nMaxGridZPosition = FindGridPositionZ(pcTransform->GetPosition() + CMath::TVECTOR3(0, 0, 1) * fabs(fExtentZ), 0, ROOM_HEIGHT / CELL_SIZE - 1);

							nMinGridXPosition = FindGridPositionX(pcTransform->GetPosition() - CMath::TVECTOR3(1, 0, 0) * fabsf(fExtentX), 0, ROOM_WIDTH / CELL_SIZE - 1);
							nMinGridZPosition = FindGridPositionZ(pcTransform->GetPosition() - CMath::TVECTOR3(0, 0, 1) * fabsf(fExtentZ), 0, ROOM_HEIGHT / CELL_SIZE - 1);
						}

						else if (collider->m_nComponentType == eComponent::CAPSULE_COLLIDER)
						{
							CCapsuleCollider* pCapCollider = (CCapsuleCollider*)collider;
							CTransform* pcTransform = collider->m_pcOwner->GetComponent<CTransform>();

							float fExtent = pCapCollider->GetRadius() + 0.2f;

							nMaxGridXPosition = FindGridPositionX(pcTransform->GetPosition() + CMath::TVECTOR3(1, 0, 0) * fExtent, 0, ROOM_WIDTH / CELL_SIZE - 1);
							nMaxGridZPosition = FindGridPositionZ(pcTransform->GetPosition() + CMath::TVECTOR3(0, 0, 1) * fExtent, 0, ROOM_HEIGHT / CELL_SIZE - 1);

							nMinGridXPosition = FindGridPositionX(pcTransform->GetPosition() - CMath::TVECTOR3(1, 0, 0) * fExtent, 0, ROOM_WIDTH / CELL_SIZE - 1);
							nMinGridZPosition = FindGridPositionZ(pcTransform->GetPosition() - CMath::TVECTOR3(0, 0, 1) * fExtent, 0, ROOM_HEIGHT / CELL_SIZE - 1);
						}

						if (nGridXPosition != nMaxGridXPosition)
						{
							for (int i = nGridXPosition; i <= nMaxGridXPosition; i++)
							{
								bool bTraversedZ = false;
								if (nGridZPosition != nMaxGridZPosition)
								{
									for (int j = nGridZPosition; j <= nMaxGridZPosition; j++)
									{
										if (!(i == nGridXPosition && j == nGridZPosition))
											m_StaticGridPositions[collider].push_back(TCoordinate(i, j));
										//m_Grid[i][j].push_back(collider);
									}

									bTraversedZ = true;
								}

								if (nGridZPosition != nMinGridZPosition)
								{
									for (int j = nGridZPosition; j >= nMinGridZPosition; j--)
									{
										if (!(i == nGridXPosition && j == nGridZPosition) && !(j == nGridZPosition && bTraversedZ))
											m_StaticGridPositions[collider].push_back(TCoordinate(i, j));
										//m_Grid[i][j].push_back(collider);
									}

									bTraversedZ = true;
								}

								if (!bTraversedZ)
								{
									if (i != nGridXPosition)
										m_StaticGridPositions[collider].push_back(TCoordinate(i, nGridZPosition));
									//m_Grid[i][nGridZPosition].push_back(collider);
								}
							}
						}

						else if (nGridZPosition != nMaxGridZPosition)
						{
							for (int j = nGridZPosition + 1; j <= nMaxGridZPosition; j++)
							{
								m_StaticGridPositions[collider].push_back(TCoordinate(nGridXPosition, j));
								//m_Grid[nGridXPosition][j].push_back(collider);
							}
						}

						if (nGridXPosition != nMinGridXPosition)
						{
							int nOffset = nGridXPosition != nMaxGridXPosition ? 1 : 0;
							for (int i = nGridXPosition - nOffset; i >= nMinGridXPosition; i--)
							{
								bool bTraversedZ = false;
								if (nGridZPosition != nMaxGridZPosition)
								{
									for (int j = nGridZPosition; j <= nMaxGridZPosition; j++)
									{
										if (i != nGridXPosition || j != nGridZPosition)
											m_StaticGridPositions[collider].push_back(TCoordinate(i, j));
										//m_Grid[i][j].push_back(collider);
									}

									bTraversedZ = true;
								}

								if (nGridZPosition != nMinGridZPosition)
								{
									for (int j = nGridZPosition; j >= nMinGridZPosition; j--)
									{
										if (!(i == nGridXPosition && j == nGridZPosition) && !(j == nGridZPosition && bTraversedZ))
											m_StaticGridPositions[collider].push_back(TCoordinate(i, j));
										//m_Grid[i][j].push_back(collider);
									}

									bTraversedZ = true;
								}

								if (!bTraversedZ)
								{
									if (i != nGridXPosition)
										m_StaticGridPositions[collider].push_back(TCoordinate(i, nGridZPosition));
									//m_Grid[i][nGridZPosition].push_back(collider);
								}
							}
						}

						else if (nGridZPosition != nMinGridZPosition)
						{
							for (int j = nGridZPosition - 1; j >= nMinGridZPosition; j--)
							{
								m_StaticGridPositions[collider].push_back(TCoordinate(nGridXPosition, j));
								//m_Grid[nGridXPosition][j].push_back(collider);
							}
						}
					}
				}

				else
				{
					m_cDynamicColliders.push_back(collider);

					CMath::TVECTOR3 tColliderPosition = collider->m_pcOwner->GetComponent<CTransform>()->GetPosition();
					int nGridXPosition = FindGridPositionX(tColliderPosition, 0, ROOM_WIDTH / CELL_SIZE - 1);
					int nGridZPosition = FindGridPositionZ(tColliderPosition, 0, ROOM_HEIGHT / CELL_SIZE - 1);

					m_DynamicGridPositions[collider].push_back(TCoordinate(nGridXPosition, nGridZPosition));

					int nMaxGridXPosition, nMaxGridZPosition;
					int nMinGridXPosition, nMinGridZPosition;
					nMaxGridXPosition = nMinGridXPosition = nGridXPosition;
					nMaxGridZPosition = nMinGridZPosition = nGridZPosition;

					if (collider->m_nComponentType == eComponent::BOX_COLLIDER)
					{
						CBoxCollider* pBoxCollider = (CBoxCollider*)collider;

						CTransform* pcTransform = collider->m_pcOwner->GetComponent<CTransform>();

						CMath::TMATRIX tMatrix = pcTransform->GetMatrix();
						tMatrix._41 = tMatrix._42 = tMatrix._43 = 0.0f;

						float fExtentX = CMath::Vector3Dot(CMath::TVECTOR3(1, 0, 0), CMath::Vector3Transform(pBoxCollider->GetExtent(), tMatrix)) + 0.2f;
						float fExtentZ = CMath::Vector3Dot(CMath::TVECTOR3(0, 0, 1), CMath::Vector3Transform(pBoxCollider->GetExtent(), tMatrix)) + 0.2f;

						nMaxGridXPosition = FindGridPositionX(pcTransform->GetPosition() + CMath::TVECTOR3(1, 0, 0) * fabs(fExtentX), 0, ROOM_WIDTH / CELL_SIZE - 1);
						nMaxGridZPosition = FindGridPositionZ(pcTransform->GetPosition() + CMath::TVECTOR3(0, 0, 1) * fabs(fExtentZ), 0, ROOM_HEIGHT / CELL_SIZE - 1);

						nMinGridXPosition = FindGridPositionX(pcTransform->GetPosition() - CMath::TVECTOR3(1, 0, 0) * fabsf(fExtentX), 0, ROOM_WIDTH / CELL_SIZE - 1);
						nMinGridZPosition = FindGridPositionZ(pcTransform->GetPosition() - CMath::TVECTOR3(0, 0, 1) * fabsf(fExtentZ), 0, ROOM_HEIGHT / CELL_SIZE - 1);
					}

					else if (collider->m_nComponentType == eComponent::CAPSULE_COLLIDER)
					{
						CCapsuleCollider* pCapCollider = (CCapsuleCollider*)collider;
						CTransform* pcTransform = collider->m_pcOwner->GetComponent<CTransform>();

						float fExtent = pCapCollider->GetRadius() + 0.2f;

						nMaxGridXPosition = FindGridPositionX(pcTransform->GetPosition() + CMath::TVECTOR3(1, 0, 0) * fExtent, 0, ROOM_WIDTH / CELL_SIZE - 1);
						nMaxGridZPosition = FindGridPositionZ(pcTransform->GetPosition() + CMath::TVECTOR3(0, 0, 1) * fExtent, 0, ROOM_HEIGHT / CELL_SIZE - 1);

						nMinGridXPosition = FindGridPositionX(pcTransform->GetPosition() - CMath::TVECTOR3(1, 0, 0) * fExtent, 0, ROOM_WIDTH / CELL_SIZE - 1);
						nMinGridZPosition = FindGridPositionZ(pcTransform->GetPosition() - CMath::TVECTOR3(0, 0, 1) * fExtent, 0, ROOM_HEIGHT / CELL_SIZE - 1);
					}

					if (nGridXPosition != nMaxGridXPosition)
					{
						for (int i = nGridXPosition; i <= nMaxGridXPosition; i++)
						{
							bool bTraversedZ = false;
							if (nGridZPosition != nMaxGridZPosition)
							{
								for (int j = nGridZPosition; j <= nMaxGridZPosition; j++)
								{
									if (!(i == nGridXPosition && j == nGridZPosition))
										m_DynamicGridPositions[collider].push_back(TCoordinate(i, j));
								}

								bTraversedZ = true;
							}

							if (nGridZPosition != nMinGridZPosition)
							{
								for (int j = nGridZPosition; j >= nMinGridZPosition; j--)
								{
									if (!(i == nGridXPosition && j == nGridZPosition) && !(j == nGridZPosition && bTraversedZ))
										m_DynamicGridPositions[collider].push_back(TCoordinate(i, j));
								}

								bTraversedZ = true;
							}

							if (!bTraversedZ)
							{
								if (i != nGridXPosition)
									m_DynamicGridPositions[collider].push_back(TCoordinate(i, nGridZPosition));
							}
						}
					}

					else if (nGridZPosition != nMaxGridZPosition)
					{
						for (int j = nGridZPosition + 1; j <= nMaxGridZPosition; j++)
						{
							m_DynamicGridPositions[collider].push_back(TCoordinate(nGridXPosition, j));
						}
					}

					if (nGridXPosition != nMinGridXPosition)
					{
						int nOffset = nGridXPosition != nMaxGridXPosition ? 1 : 0;
						for (int i = nGridXPosition - nOffset; i >= nMinGridXPosition; i--)
						{
							bool bTraversedZ = false;
							if (nGridZPosition != nMaxGridZPosition)
							{
								for (int j = nGridZPosition; j <= nMaxGridZPosition; j++)
								{
									if (i != nGridXPosition || j != nGridZPosition)
										m_DynamicGridPositions[collider].push_back(TCoordinate(i, j));
								}

								bTraversedZ = true;
							}

							if (nGridZPosition != nMinGridZPosition)
							{
								for (int j = nGridZPosition; j >= nMinGridZPosition; j--)
								{
									if (!(i == nGridXPosition && j == nGridZPosition) && !(j == nGridZPosition && bTraversedZ))
										m_DynamicGridPositions[collider].push_back(TCoordinate(i, j));
									//m_Grid[i][j].push_back(collider);
								}

								bTraversedZ = true;
							}

							if (!bTraversedZ)
							{
								if (i != nGridXPosition)
									m_DynamicGridPositions[collider].push_back(TCoordinate(i, nGridZPosition));
								//m_Grid[i][nGridZPosition].push_back(collider);
							}
						}
					}

					else if (nGridZPosition != nMinGridZPosition)
					{
						for (int j = nGridZPosition - 1; j >= nMinGridZPosition; j--)
						{
							m_DynamicGridPositions[collider].push_back(TCoordinate(nGridXPosition, j));
							//m_Grid[nGridXPosition][j].push_back(collider);
						}
					}

					if (collider->m_pcOwner->m_nEntityId == 0)
					{
						/*std::cout << "Player is at position (" << m_DynamicGridPositions[collider][0].x << ", " << m_DynamicGridPositions[collider][0].y << ")";
						for (int i = 1; i < m_DynamicGridPositions[collider].size(); i++)
						{
							std::cout << " and (" << m_DynamicGridPositions[collider][i].x << ", " << m_DynamicGridPositions[collider][i].y << ")";
						}
						std::cout << std::endl;*/
					}
				}
			}

		}
	}
}
void CPhysicsManager::UpdateRigidBodies(double dDeltaTime)
{
#pragma region Debug Enemy Controller
	//float fForwardMovement = 0;
	//float fSideMovement = 0;

	//if (CInputManager::GetKeyDown('I'))
	//{
	//	fForwardMovement = 1;
	//}
	//else if (CInputManager::GetKeyDown('K'))
	//{
	//	fForwardMovement = -1;
	//}

	//if (CInputManager::GetKeyDown('L'))
	//{
	//	fSideMovement = 1;
	//}
	//else if (CInputManager::GetKeyDown('J'))
	//{
	//	fSideMovement = -1;
	//}

	//CMath::TVECTOR3 tVelocityVector = CMath::TVECTOR3{ fSideMovement, 0, fForwardMovement };
	//CMath::TVECTOR3 tNormalVelocity = CMath::Vector3Normalize(tVelocityVector);
	//CMath::TVECTOR3 tFinalVelocity = tNormalVelocity * 15.0f;
	//(*m_pcRigidbodies)[2]->SetVelocity(tFinalVelocity);
#pragma endregion

#ifdef MULTI_THREADING

	std::unique_lock<std::mutex> cGunLock(m_cMutexLock);

	std::cout << "Thread: " << std::this_thread::get_id() << " started in UpdateRigidBodies\n";
	m_cGunCondition.wait(cGunLock);

	while (!m_bShutDownFlag)
	{
		for (int i = 0; i < m_pcRigidbodies->size(); i++)
		{
			m_cMainMutex.lock();
			if (m_bBitMask[i] == false)
			{
				m_bBitMask[i] = true;
				m_cMainMutex.unlock();

				CTransform* pcTransform = (*m_pcRigidbodies)[i]->m_pcOwner->GetComponent<CTransform>();

				if (pcTransform)
				{
					CMath::TVECTOR3 tVelocity = (*m_pcRigidbodies)[i]->GetVelocity();

					CMath::TVECTOR3 tNewPosition = pcTransform->GetPosition();
					
					tNewPosition += tVelocity * (float)m_dDelta;
					
					/*tNewPosition.x += tVelocity.x * (float)dDeltaTime;
					tNewPosition.y += tVelocity.y * (float)dDeltaTime;
					tNewPosition.z += tVelocity.z * (float)dDeltaTime;*/
					
					pcTransform->SetPosition(tNewPosition);
					m_cMainMutex.lock();
				}
			}
			m_cMainMutex.unlock();
		}

		m_cMainMutex.lock();
		m_iFinishedThreads++;
		m_cMainWait.notify_all();
		m_cMainMutex.unlock();

		m_cGunCondition.wait(cGunLock);
	}
#endif

#ifndef MULTI_THREADING

	for (int i = 0; i < m_pcRigidbodies->size(); i++)
	{
		CTransform* pcTransform = (*m_pcRigidbodies)[i]->m_pcOwner->GetComponent<CTransform>();

		if (pcTransform && pcTransform->IsActive())
		{
			CMath::TVECTOR3 tVelocity = (*m_pcRigidbodies)[i]->GetVelocity();

			CMath::TVECTOR3 tNewPosition = pcTransform->GetPosition();
			tNewPosition.x += tVelocity.x * (float)dDeltaTime;
			tNewPosition.y += tVelocity.y * (float)dDeltaTime;
			tNewPosition.z += tVelocity.z * (float)dDeltaTime;

			pcTransform->SetPosition(tNewPosition);
		}
}

#endif // !MULTI_THREADING

}
void CPhysicsManager::UpdateCollisions()
{
	for (int i = 0; i < m_cDynamicColliders.size(); i++)
	{
		// Get the collider to be checked against the rest of colliders
		ICollider* pcA = m_cDynamicColliders[i];

		int nGridXPosition = FindGridPositionX(pcA->m_pcOwner->GetComponent<CTransform>()->GetPosition(), 0, ROOM_WIDTH / CELL_SIZE - 1);
		int nGridYPosition = FindGridPositionZ(pcA->m_pcOwner->GetComponent<CTransform>()->GetPosition(), 0, ROOM_HEIGHT / CELL_SIZE - 1);

		//std::cout << "X: " << nGridXPosition << "  Y: " << nGridYPosition << std::endl;

		//std::vector<TCoordinate> neighbors = GetNeighbors(TCoordinate(nGridXPosition, nGridYPosition));

		// Check against other dynamic objects that haven't been checked
		for (int j = i + 1; j < m_cDynamicColliders.size(); j++)
		{
			ICollider* pcB = m_cDynamicColliders[j];

			//Check if pcB is located in any of the neighboring cells of pcA
			bool found = false;
			//for (TCoordinate coor : neighbors)
			for (TCoordinate coor : m_DynamicGridPositions[pcA])
			{
				//for (ICollider* collider : m_Grid[coor.x][coor.y])
				for (TCoordinate coor2 : m_DynamicGridPositions[pcB])
				{
					if (coor.x == coor2.x && coor.y == coor2.y)
					{
						found = true;
						break;
					}
				}
				if (found)
					break;
			}

			//If not found, there is no need to check for collision
			if (!found)
				continue;

			int nCollisionType = GetCollisionType(pcA, pcB);

			bool bIsTriggerCollision = pcA->IsTrigger() || pcB->IsTrigger();

			PairwiseCollisionCheck(pcA, pcB, nCollisionType, bIsTriggerCollision);
		}

		// Check against all static objects
		for (int j = 0; j < m_cStaticColliders.size(); j++)
		{
			ICollider* pcB = m_cStaticColliders[j];

			if (pcB->m_pcOwner->m_nEntityType == eEntity::DOOR)
			{
				int debug = 0;
			}

			//Check if pcB is located in any of the neighboring cells of pcA
			bool found = false;
			//for (TCoordinate coor : neighbors)
			for (TCoordinate coor : m_DynamicGridPositions[pcA])
			{
				//for (ICollider* collider : m_Grid[coor.x][coor.y])
				for (TCoordinate coor2 : m_StaticGridPositions[pcB])
				{
					if (coor.x == coor2.x && coor.y == coor2.y)
					{
						found = true;
						break;
					}
				}
				if (found)
					break;
			}

			//If not found, there is no need to check for collision
			if (!found)
				continue;

			int nCollisionType = GetCollisionType(pcA, pcB);

			bool bIsTriggerCollision = pcA->IsTrigger() || pcB->IsTrigger();

			PairwiseCollisionCheck(pcA, pcB, nCollisionType, bIsTriggerCollision);
		}
	}
}


bool CPhysicsManager::Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, float fMaxDistance)
{
	for (ICollider* collider : m_cDynamicColliders)
	{
		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint))
			{
				return true;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (IsCollidingRayBox((CBoxCollider*)collider, tOrigin, CMath::Vector3Normalize(tDirection), fMaxDistance, OUT tCollisionPoint))
			{
				return true;
			}

			break;
		}

		default:
			break;
		}
	}

	for (ICollider* collider : m_cStaticColliders)
	{
		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint))
			{
				return true;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (IsCollidingRayBox((CBoxCollider*)collider, tOrigin, CMath::Vector3Normalize(tDirection), fMaxDistance, OUT tCollisionPoint))
			{
				return true;
			}

			break;
		}
		default:
			break;
		}
	}

	return false;
}
bool CPhysicsManager::Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit & tRayHit, CEntityMask nEntityMask)
{
	float fMinDistance = std::numeric_limits<float>::infinity();

	if (CMath::Vector3Dot(tDirection, tDirection) != 1.0f)
		tDirection = CMath::Vector3Normalize(tDirection);

	for (ICollider* collider : m_cDynamicColliders)
	{
		int nMaskCheck = (collider->m_pcOwner->m_nEntityType + 1) * (collider->m_pcOwner->m_nEntityType + 1);

		if (nEntityMask & nMaskCheck)
			continue;

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	for (ICollider* collider : m_cStaticColliders)
	{
		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, std::numeric_limits<float>::infinity(), OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	bool bHasRayCollided = fMinDistance < std::numeric_limits<float>::infinity();

	return bHasRayCollided;
}
bool CPhysicsManager::Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit & tRayHit, float fMaxDistance, CEntityMask nEntityMask)
{
	float fMinDistance = std::numeric_limits<float>::infinity();

	if (CMath::Vector3Dot(tDirection, tDirection) != 1.0f)
		tDirection = CMath::Vector3Normalize(tDirection);

	for (ICollider* collider : m_cDynamicColliders)
	{
		int nMaskCheck = (collider->m_pcOwner->m_nEntityType + 1) * (collider->m_pcOwner->m_nEntityType + 1);

		if (nEntityMask & nMaskCheck)
			continue;

		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	for (ICollider* collider : m_cStaticColliders)
	{
		switch (collider->m_nComponentType)
		{
		case eComponent::CAPSULE_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayCapsule((CCapsuleCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);
			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CMath::TVECTOR3 tCollisionPoint;
			if (!IsCollidingRayBox((CBoxCollider*)collider, tOrigin, tDirection, fMaxDistance, OUT tCollisionPoint))
				continue;

			CMath::TVECTOR3 tOriginToCollisionPoint = tCollisionPoint - tOrigin;
			float fSqDistanceOriginToCollisionPoint = CMath::Vector3Dot(tOriginToCollisionPoint, tOriginToCollisionPoint);

			if (fSqDistanceOriginToCollisionPoint < fMinDistance)
			{
				tRayHit.tEntity = collider->m_pcOwner;
				tRayHit.tCollisionPoint = tCollisionPoint;

				fMinDistance = fSqDistanceOriginToCollisionPoint;
			}

			break;
		}
		default:
			break;
		}
	}

	bool bHasRayCollided = fMinDistance < std::numeric_limits<float>::infinity();

	return bHasRayCollided;
}
bool CPhysicsManager::RaycastMouseToFloor(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, CMath::TVECTOR3 & tFloorPosition)
{
	/*
	An intersecting point in the plane would be:
	point = ray.origin + ray.direction * t;

	A point that lies in the plane satisfies the equation:
	dot(point, plane.normal) - plane.distance = 0

	Replacing the point with the intersecting point equation:
	dot((ray.origin + ray.direction * t), plane.normal) - plane.distance = 0;

	Solving for t:
	dot((ray.origin + ray.direction * t), plane.normal) = plane.distance;
	dot(ray.origin, plane.normal) + dot(ray.direction * t, plane.normal) = plane.distance;
	dot(ray.direction * t, plane.normal) = plane.distance - dot(ray.origin, plane.normal);
	t * dot(ray.direction, plane.normal) = plane.distance - dot(ray.origin, plane.normal);

	t = (plane.distance - dot(ray.origin, plane.normal) / dot(ray.direction, plane.normal);

	Then, return the result of the intersecting point equation given t.
	*/

	if (CMath::Vector3Dot(tDirection, tDirection) != 1.0f)
		tDirection = CMath::Vector3Normalize(tDirection);

	float fDotDirectionNorm = CMath::Vector3Dot(tDirection, m_FloorPlane.tNormal);

	if (fDotDirectionNorm >= 0)
	{
		CEventManager::SendDebugMessage(TDebugMessage(std::string("Why is the camera below the ground??")));
		return false;
	}

	float fDotOriginNorm = CMath::Vector3Dot(tOrigin, m_FloorPlane.tNormal);

	float fDistanceAlongRay = (m_FloorPlane.fDistance - fDotOriginNorm) / fDotDirectionNorm;

	if (fDistanceAlongRay < 0)
		return false;

	tFloorPosition = tOrigin + tDirection * fDistanceAlongRay;
	return true;
}

void CPhysicsManager::PairwiseCollisionCheck(ICollider * pcA, ICollider * pcB, int nCollisionType, bool bIsTriggerCollision)
{
	bool bIsColliding = false;
	if (nCollisionType == eCollisionTypes::BOX_BOX)
	{
		CBoxCollider* pcBoxA = (CBoxCollider*)pcA;
		CBoxCollider* pcBoxB = (CBoxCollider*)pcB;
		UpdateBoxBoxCollision(pcBoxA, pcBoxB, bIsTriggerCollision, OUT bIsColliding);
	}

	else if (nCollisionType == eCollisionTypes::CAPSULE_CAPSULE)
	{
		CCapsuleCollider* pcCapsuleA = (CCapsuleCollider*)pcA;
		CCapsuleCollider* pcCapsuleB = (CCapsuleCollider*)pcB;
		UpdateCapsuleCapsuleCollision(pcCapsuleA, pcCapsuleB, bIsTriggerCollision, OUT bIsColliding);
	}

	else if (nCollisionType == eCollisionTypes::BOX_CAPSULE)
	{
		if (pcA->m_nComponentType == eComponent::BOX_COLLIDER)
		{
			CBoxCollider* pcBoxColliderA = (CBoxCollider*)pcA;
			CCapsuleCollider* pcCapsuleColliderB = (CCapsuleCollider*)pcB;
			UpdateBoxCapsuleCollision(pcBoxColliderA, pcCapsuleColliderB, bIsTriggerCollision, false, bIsColliding);
		}

		else
		{
			CCapsuleCollider* pcCapsuleColliderA = (CCapsuleCollider*)pcA;
			CBoxCollider* pcBoxColliderB = (CBoxCollider*)pcB;
			UpdateBoxCapsuleCollision(pcBoxColliderB, pcCapsuleColliderA, bIsTriggerCollision, true, bIsColliding);
		}
	}

	if (bIsColliding)
	{
		if (IsNewCollision(pcA, pcB))
		{
			m_cCollisions[pcA].push_back(pcB);
			m_cCollisions[pcB].push_back(pcA);

			int nCollisionType = bIsTriggerCollision ? eCollisionState::TRIGGER_ENTER : eCollisionState::COLLISION_ENTER;
			TCollisionMessage message(pcA->m_pcOwner->m_nEntityId, pcB->m_pcOwner->m_nEntityId, nCollisionType);
			CEventManager::SendCollisionMessage(message);
		}

		else
		{
			int nCollisionType = bIsTriggerCollision ? eCollisionState::TRIGGER_STAY : eCollisionState::COLLISION_STAY;
			TCollisionMessage message(pcA->m_pcOwner->m_nEntityId, pcB->m_pcOwner->m_nEntityId, nCollisionType);
			CEventManager::SendCollisionMessage(message);
		}
	}
	else
	{
		int nIndexA, nIndexB;
		if (IsCollisionExit(pcA, pcB, OUT nIndexA, OUT nIndexB))
		{
			auto iterA = m_cCollisions[pcA].begin() + nIndexA;
			m_cCollisions[pcA].erase(iterA);

			auto iterB = m_cCollisions[pcB].begin() + nIndexB;
			m_cCollisions[pcB].erase(iterB);

			int nCollisionType = bIsTriggerCollision ? eCollisionState::TRIGGER_EXIT : eCollisionState::COLLISION_EXIT;
			TCollisionMessage message(pcA->m_pcOwner->m_nEntityId, pcB->m_pcOwner->m_nEntityId, nCollisionType);
			CEventManager::SendCollisionMessage(message);
		}
	}
}
int CPhysicsManager::GetCollisionType(ICollider * pcA, ICollider * pcB)
{
	int nComponentType = -1;
	switch (pcA->m_nComponentType)
	{
	case eComponent::BOX_COLLIDER:
	{
		switch (pcB->m_nComponentType)
		{

		case eComponent::BOX_COLLIDER:
		{
			nComponentType = eCollisionTypes::BOX_BOX;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			nComponentType = eCollisionTypes::BOX_CAPSULE;
			break;
		}

		default:
			break;
		}

		break;
	}

	case eComponent::CAPSULE_COLLIDER:
	{
		switch (pcB->m_nComponentType)
		{

		case eComponent::BOX_COLLIDER:
		{
			nComponentType = eCollisionTypes::BOX_CAPSULE;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			nComponentType = eCollisionTypes::CAPSULE_CAPSULE;
			break;
		}
		default:
			break;
		}

		break;
	}

	default:
		break;
	}

	return nComponentType;
}

void CPhysicsManager::UpdateBoxBoxCollision(CBoxCollider * pcA, CBoxCollider * pcB, bool bIsTriggerCollision, bool& bIsColliding)
{
	bIsColliding = false;

	TCollision tCollision;
	if (IsCollidingBoxBox(pcA, pcB, OUT tCollision))
	{
		if (!bIsTriggerCollision)
		{
			ResolveCollision(pcA, pcB, tCollision);
		}
		bIsColliding = true;
	}
}
void CPhysicsManager::UpdateCapsuleCapsuleCollision(CCapsuleCollider * pcA, CCapsuleCollider * pcB, bool bIsTriggerCollision, bool & bIsColliding)
{
	bIsColliding = false;

	TCollision tCollision;
	if (IsCollidingCapsuleCapsule(pcA, pcB, OUT tCollision))
	{
		//std::cout << "COLLIDED" << std::endl;
		if (!bIsTriggerCollision)
		{
			ResolveCollision(pcA, pcB, tCollision);
		}
		bIsColliding = true;
	}
}
void CPhysicsManager::UpdateBoxCapsuleCollision(CBoxCollider * pcA, CCapsuleCollider * pcB, bool bIsTriggerCollision, bool bReverseOrder, bool & bIsColliding)
{
	bIsColliding = false;

	TCollision tCollision;
	if (IsCollidingBoxCapsule(pcA, pcB, OUT tCollision))
	{
		//std::cout << "COLLISION" << std::endl;
		if (!bIsTriggerCollision)
		{
			if (!bReverseOrder)
			{
				//tCollision.tAxis = tCollision.tAxis * -1.0f;
			}

			ResolveCollision(pcA, pcB, tCollision);
		}

		bIsColliding = true;
	}
}

bool CPhysicsManager::IsCollidingBoxBox(CBoxCollider* pA, CBoxCollider* pB, TCollision& tCollision)
{
	// Only check X and Z, Y isn't necessary
	static const int nAxisCount = 2;
	CMath::TVECTOR3 tLocalAxesA[nAxisCount];
	CMath::TVECTOR3 tLocalAxesB[nAxisCount];

	CTransform* tTransformA = pA->m_pcOwner->GetComponent<CTransform>();
	CTransform* tTransformB = pB->m_pcOwner->GetComponent<CTransform>();

	CMath::TMATRIX tTransformMatrixA = tTransformA->GetMatrix();
	CMath::TMATRIX tTransformMatrixB = tTransformB->GetMatrix();

	tLocalAxesA[0] = CMath::TVECTOR3(tTransformMatrixA.r[0].x, tTransformMatrixA.r[0].y, tTransformMatrixA.r[0].z);
	tLocalAxesB[0] = CMath::TVECTOR3(tTransformMatrixB.r[0].x, tTransformMatrixB.r[0].y, tTransformMatrixB.r[0].z);

	tLocalAxesA[1] = CMath::TVECTOR3(tTransformMatrixA.r[2].x, tTransformMatrixA.r[2].y, tTransformMatrixA.r[2].z);
	tLocalAxesB[1] = CMath::TVECTOR3(tTransformMatrixB.r[2].x, tTransformMatrixB.r[2].y, tTransformMatrixB.r[2].z);

	// Values used to resolve collision
	float fMinOverlap = std::numeric_limits<float>::infinity();
	CMath::TVECTOR3 tMinAxis;

	// If there is a gap between the OOBBs in any of their local axes, they're not colliding
	for (CMath::TVECTOR3 axis : tLocalAxesA)
	{
		if (!IsIntersecting(axis, pA, pB, fMinOverlap, tMinAxis))
			return false;
	}

	for (CMath::TVECTOR3 axis : tLocalAxesB)
	{
		if (!IsIntersecting(axis, pA, pB, fMinOverlap, tMinAxis))
			return false;
	}

	// If the direction of collision and the resolution axis point in the same direction, flip the resolution axis
	// Otherwise the object would move further inside the other object
	if (CMath::Vector3Dot((tTransformA->GetPosition() + pA->GetCenter()) - (tTransformB->GetPosition() + pB->GetCenter()), tMinAxis) < 0)
		tMinAxis *= -1;

	// Offset the resolution amount so they are still colliding by a minimum amount after the resolution
	// This way it is still possible to check if a collision is still in progress for the event manager
	tCollision.fOffset = fMinOverlap - COLLISION_OFFSET;
	tCollision.tAxis = tMinAxis;

	return true;
}
bool CPhysicsManager::IsCollidingCapsuleCapsule(CCapsuleCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
	CTransform* cTransformA = pcA->m_pcOwner->GetComponent<CTransform>();
	CTransform* cTransformB = pcB->m_pcOwner->GetComponent<CTransform>();

	CMath::TVECTOR3 tCenterPositionA = cTransformA->GetPosition() + pcA->GetCenter();
	CMath::TVECTOR3 tCenterPositionB = cTransformB->GetPosition() + pcB->GetCenter();

	CMath::TMATRIX tMatrixA = cTransformA->GetMatrix();
	CMath::TMATRIX tMatrixB = cTransformB->GetMatrix();

	CMath::TVECTOR3 tYAxisA = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrixA.r[1].x, tMatrixA.r[1].y, tMatrixA.r[1].z));
	CMath::TVECTOR3 tYAxisB = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrixB.r[1].x, tMatrixB.r[1].y, tMatrixB.r[1].z));


	CMath::TVECTOR3 tP0 = tCenterPositionA + tYAxisA * 0.5f * pcA->GetHeight();
	CMath::TVECTOR3 tP1 = tCenterPositionA - tYAxisA * 0.5f * pcA->GetHeight();

	CMath::TVECTOR3 tQ0 = tCenterPositionB + tYAxisB * 0.5f * pcB->GetHeight();
	CMath::TVECTOR3 tQ1 = tCenterPositionB - tYAxisB * 0.5f * pcB->GetHeight();

	CMath::TVECTOR3 tClosestPointA;
	CMath::TVECTOR3 tClosestPointB;
	ClosestPtSegmentSegment(tP0, tP1, tQ0, tQ1, OUT tClosestPointA, OUT tClosestPointB);

	CMath::TVECTOR3 tCollisionVector = tClosestPointA - tClosestPointB;
	float fDistance = CMath::Vector3Magnitude(tCollisionVector);
	float fRadiusSum = pcA->GetRadius() + pcB->GetRadius();
	if (fDistance <= fRadiusSum)
	{
		tCollision.tAxis = CMath::Vector3Normalize(tCollisionVector);
		tCollision.fOffset = fRadiusSum - fDistance - COLLISION_OFFSET;
		return true;
	}

	return false;
}
bool CPhysicsManager::IsCollidingBoxCapsule(CBoxCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
#pragma region Accurate Code
	//CTransform* cTransformA = pcA->m_pcOwner->GetComponent<CTransform>();
	//CTransform* cTransformB = pcB->m_pcOwner->GetComponent<CTransform>();

	//CMath::TVECTOR3 tCenterPositionA = cTransformA->GetPosition() + pcA->GetCenter();
	//CMath::TVECTOR3 tCenterPositionB = cTransformB->GetPosition() + pcB->GetCenter();

	//CMath::TMATRIX tMatrixA = cTransformA->GetMatrix();
	//CMath::TMATRIX tMatrixB = cTransformB->GetMatrix();

	//CMath::TVECTOR3 tYAxisB = CMath::Vector3Normalize(CMath::TVECTOR3(tMatrixB.r[1].x, tMatrixB.r[1].y, tMatrixB.r[1].z));

	//CMath::TVECTOR3 Segment[2] = { tCenterPositionB + tYAxisB * 0.5f * pcB->GetHeight(),
	//	 tCenterPositionB - tYAxisB * 0.5f * pcB->GetHeight() };

	//CMath::TVECTOR3 tExtent = pcA->GetExtent();
	//CMath::TVECTOR3 tCenter = pcA->GetCenter();

	//CMath::TVECTOR3 Box[8];
	//Box[0] = CMath::Vector3Transform(tExtent, tMatrixA) + tCenter;
	//Box[1] = CMath::Vector3Transform(CMath::TVECTOR3{ -tExtent.x,  tExtent.y,   tExtent.z }, tMatrixA) + tCenter;
	//Box[2] = CMath::Vector3Transform(CMath::TVECTOR3{ -tExtent.x,  tExtent.y,  -tExtent.z }, tMatrixA) + tCenter;
	//Box[3] = CMath::Vector3Transform(CMath::TVECTOR3{ tExtent.x,  tExtent.y,  -tExtent.z }, tMatrixA) + tCenter;
	//Box[4] = CMath::Vector3Transform(CMath::TVECTOR3{ tExtent.x, -tExtent.y,   tExtent.z }, tMatrixA) + tCenter;
	//Box[5] = CMath::Vector3Transform(CMath::TVECTOR3{ -tExtent.x, -tExtent.y,   tExtent.z }, tMatrixA) + tCenter;
	//Box[6] = CMath::Vector3Transform(CMath::TVECTOR3{ -tExtent.x, -tExtent.y,  -tExtent.z }, tMatrixA) + tCenter;
	//Box[7] = CMath::Vector3Transform(CMath::TVECTOR3{ tExtent.x, -tExtent.y,  -tExtent.z }, tMatrixA) + tCenter;


	//std::vector<CMath::TVECTOR3> tSimplexSet;

	//CMath::TVECTOR3 S = Support(Box, 8, Segment, 2, CMath::TVECTOR3(1, 0, 0));
	//tSimplexSet.push_back(S);

	//while (true)
	//{
	//	CMath::TVECTOR3 tPoint = GetClosestToOrigin(tSimplexSet);
	//	if (tPoint.x == 0.0f && tPoint.y == 0.0f && tPoint.z == 0.0f)
	//	{
	//		// There is collision... I wouldn't know what to do.
	//	}

	//	ReduceSimplexSet(OUT tSimplexSet, tPoint);

	//	CMath::TVECTOR3 tNewPoint = Support(Box, 8, Segment, 2, tPoint * -1.0f);

	//	if (CMath::Vector3Dot(tPoint, tNewPoint) >= 0)
	//	{
	//		// We know the origin it's outside, we can return the angle and the distance
	//		if (CMath::Vector3Magnitude(tPoint) < pcB->GetRadius())
	//		{
	//			tCollision.tAxis = 
	//		}
	//	}

	//	else
	//	{
	//		tSimplexSet.push_back(tNewPoint);
	//	}

	//}
#pragma endregion

	// This code assumes that the capsule is vertical and only rotates along the y axis!!
	// Basically treats the capsule as a sphere with the same center and radius
	CTransform* tTransform = pcB->m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tCenter = tTransform->GetPosition() + pcB->GetCenter();

	CMath::TVECTOR3 tClosestPoint;
	ClosestPtPointOBB(tCenter, pcA, tClosestPoint);

	if (CMath::Vector3Magnitude(tClosestPoint - tCenter) <= pcB->GetRadius())
	{
		tCollision.fOffset = pcB->GetRadius() - CMath::Vector3Magnitude(tClosestPoint - tCenter) - COLLISION_OFFSET;
		tCollision.tAxis = CMath::Vector3Normalize(tClosestPoint - tCenter);
		return true;
	}
	return false;
}
bool CPhysicsManager::IsIntersecting(CMath::TVECTOR3 tAxis, CBoxCollider* pA, CBoxCollider* pB, float& fMinOverlap, CMath::TVECTOR3& tMinAxis)
{
	// Get only the top four vertices, since the Y axis is not being checked
	CMath::TVECTOR3 tTopVerticesA[4];
	CMath::TVECTOR3 tTopVerticesB[4];

	tTopVerticesA[0] = CMath::Vector3Transform(pA->GetExtent(), pA->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pA->GetCenter();
	tTopVerticesA[1] = CMath::Vector3Transform(CMath::TVECTOR3{ -pA->GetExtent().x,  pA->GetExtent().y,   pA->GetExtent().z }, pA->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pA->GetCenter();
	tTopVerticesA[2] = CMath::Vector3Transform(CMath::TVECTOR3{ -pA->GetExtent().x,  pA->GetExtent().y,  -pA->GetExtent().z }, pA->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pA->GetCenter();
	tTopVerticesA[3] = CMath::Vector3Transform(CMath::TVECTOR3{ pA->GetExtent().x,  pA->GetExtent().y,  -pA->GetExtent().z }, pA->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pA->GetCenter();

	tTopVerticesB[0] = CMath::Vector3Transform(pB->GetExtent(), pB->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pB->GetCenter();
	tTopVerticesB[1] = CMath::Vector3Transform(CMath::TVECTOR3{ -pB->GetExtent().x,  pB->GetExtent().y,   pB->GetExtent().z }, pB->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pB->GetCenter();
	tTopVerticesB[2] = CMath::Vector3Transform(CMath::TVECTOR3{ -pB->GetExtent().x,  pB->GetExtent().y,  -pB->GetExtent().z }, pB->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pB->GetCenter();
	tTopVerticesB[3] = CMath::Vector3Transform(CMath::TVECTOR3{ pB->GetExtent().x,  pB->GetExtent().y,  -pB->GetExtent().z }, pB->m_pcOwner->GetComponent<CTransform>()->GetMatrix()) + pB->GetCenter();

	// Get the min and max values between the projection of each vertex on the specified axis to check for a collision
	float boundA[2] = { std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() };
	float boundB[2] = { std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() };

	for (int i = 0; i < 4; i++)
	{
		float tempA = CMath::Vector3Dot(tAxis, tTopVerticesA[i]);
		float tempB = CMath::Vector3Dot(tAxis, tTopVerticesB[i]);

		if (tempA < boundA[0])
			boundA[0] = tempA;
		if (tempA > boundA[1])
			boundA[1] = tempA;

		if (tempB < boundB[0])
			boundB[0] = tempB;
		if (tempB > boundB[1])
			boundB[1] = tempB;
	}


	if (boundA[1] > boundB[0] && boundB[1] > boundA[0])
	{
		float fOverlap = std::fminf(boundA[1], boundB[1]) - std::fmaxf(boundA[0], boundB[0]);
		if (fOverlap < fMinOverlap)
		{
			fMinOverlap = fOverlap;
			tMinAxis = tAxis;
		}
		return true;
	}
	else
		return false;
}

void CPhysicsManager::ResolveCollision(CBoxCollider * pcA, CBoxCollider * pcB, TCollision & tCollision)
{
	CTransform* pcTransforms[2];
	CRigidbody* pcRigidbodies[2];
	pcTransforms[0] = pcA->m_pcOwner->GetComponent<CTransform>();
	pcTransforms[1] = pcB->m_pcOwner->GetComponent<CTransform>();
	pcRigidbodies[0] = pcA->m_pcOwner->GetComponent<CRigidbody>();
	pcRigidbodies[1] = pcB->m_pcOwner->GetComponent<CRigidbody>();

	// No need to move second object
	if (!pcRigidbodies[1] || CMath::Vector3Magnitude(pcRigidbodies[1]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[0]->GetPosition();
		tPosition.x += tCollision.tAxis.x * tCollision.fOffset;
		tPosition.y += tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z += tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[0]->SetPosition(tPosition);
	}

	// No need to move first object
	else if (!pcRigidbodies[0] || CMath::Vector3Magnitude(pcRigidbodies[0]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[1]->GetPosition();
		tPosition.x -= tCollision.tAxis.x * tCollision.fOffset;
		tPosition.y -= tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z -= tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[1]->SetPosition(tPosition);
	}

	// Both objects are moving, so they need to be updated
	else
	{
		CMath::TVECTOR3 tVelocities[2] = { pcRigidbodies[0]->GetVelocity(), pcRigidbodies[1]->GetVelocity() };
		float fOverlap = tCollision.fOffset;

		float fSumX = fabsf(tVelocities[0].x) + fabsf(tVelocities[1].x);
		float fSumY = fabsf(tVelocities[0].y) + fabsf(tVelocities[1].y);
		float fSumZ = fabsf(tVelocities[0].z) + fabsf(tVelocities[1].z);

		CMath::TVECTOR3 tPositions[2] = { pcTransforms[0]->GetPosition(), pcTransforms[1]->GetPosition() };

		if (fSumX > 0)
		{
			if ((tVelocities[0].x > 0 && tVelocities[1].x > 0) || (tVelocities[0].x < 0 && tVelocities[1].x < 0))
			{
				if (fabsf(tVelocities[0].x) > fabsf(tVelocities[1].x))
					tVelocities[0].x *= -1.0f;
				else
					tVelocities[1].x *= -1.0f;
			}

			tPositions[0].x += tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[0].x));
			tPositions[1].x -= tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[1].x));
		}

		if (fSumY > 0)
		{
			if ((tVelocities[0].y > 0 && tVelocities[1].y > 0) || (tVelocities[0].y < 0 && tVelocities[1].y < 0))
			{
				if (fabsf(tVelocities[0].y) > fabsf(tVelocities[1].y))
					tVelocities[0].y *= -1.0f;
				else
					tVelocities[1].y *= -1.0f;
			}

			tPositions[0].y += tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[0].y));
			tPositions[1].y -= tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[1].y));
		}

		if (fSumZ > 0)
		{
			if ((tVelocities[0].z > 0 && tVelocities[1].z > 0) || (tVelocities[0].z < 0 && tVelocities[1].z < 0))
			{
				if (fabsf(tVelocities[0].z) > fabsf(tVelocities[1].z))
					tVelocities[0].z *= -1.0f;
				else
					tVelocities[1].z *= -1.0f;
			}

			tPositions[0].z += tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[0].z));
			tPositions[1].z -= tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[1].z));
		}

		pcTransforms[0]->SetPosition(tPositions[0]);
		pcTransforms[1]->SetPosition(tPositions[1]);
	}
}
void CPhysicsManager::ResolveCollision(CCapsuleCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
	CTransform* pcTransforms[2];
	CRigidbody* pcRigidbodies[2];
	pcTransforms[0] = pcA->m_pcOwner->GetComponent<CTransform>();
	pcTransforms[1] = pcB->m_pcOwner->GetComponent<CTransform>();
	pcRigidbodies[0] = pcA->m_pcOwner->GetComponent<CRigidbody>();
	pcRigidbodies[1] = pcB->m_pcOwner->GetComponent<CRigidbody>();

	// No need to move second object
	if (!pcRigidbodies[1] || CMath::Vector3Magnitude(pcRigidbodies[1]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[0]->GetPosition();
		tPosition.x += tCollision.tAxis.x * tCollision.fOffset;
		tPosition.y += tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z += tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[0]->SetPosition(tPosition);
	}

	// No need to move first object
	else if (!pcRigidbodies[0] || CMath::Vector3Magnitude(pcRigidbodies[0]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[1]->GetPosition();
		tPosition.x -= tCollision.tAxis.x * tCollision.fOffset;
		tPosition.y -= tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z -= tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[1]->SetPosition(tPosition);
	}

	// Both objects are moving, so they need to be updated
	else
	{
		CMath::TVECTOR3 tVelocities[2] = { pcRigidbodies[0]->GetVelocity(), pcRigidbodies[1]->GetVelocity() };
		float fOverlap = tCollision.fOffset;

		float fSumX = fabsf(tVelocities[0].x) + fabsf(tVelocities[1].x);
		float fSumY = fabsf(tVelocities[0].y) + fabsf(tVelocities[1].y);
		float fSumZ = fabsf(tVelocities[0].z) + fabsf(tVelocities[1].z);

		CMath::TVECTOR3 tPositions[2] = { pcTransforms[0]->GetPosition(), pcTransforms[1]->GetPosition() };

		if (fSumX > 0)
		{
			if ((tVelocities[0].x > 0 && tVelocities[1].x > 0) || (tVelocities[0].x < 0 && tVelocities[1].x < 0))
			{
				if (fabsf(tVelocities[0].x) > fabsf(tVelocities[1].x))
					tVelocities[0].x *= -1.0f;
				else
					tVelocities[1].x *= -1.0f;
			}

			tPositions[0].x += tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[0].x));
			tPositions[1].x -= tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[1].x));
		}

		if (fSumY > 0)
		{
			if ((tVelocities[0].y > 0 && tVelocities[1].y > 0) || (tVelocities[0].y < 0 && tVelocities[1].y < 0))
			{
				if (fabsf(tVelocities[0].y) > fabsf(tVelocities[1].y))
					tVelocities[0].y *= -1.0f;
				else
					tVelocities[1].y *= -1.0f;
			}

			tPositions[0].y += tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[0].y));
			tPositions[1].y -= tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[1].y));
		}

		if (fSumZ > 0)
		{
			if ((tVelocities[0].z > 0 && tVelocities[1].z > 0) || (tVelocities[0].z < 0 && tVelocities[1].z < 0))
			{
				if (fabsf(tVelocities[0].z) > fabsf(tVelocities[1].z))
					tVelocities[0].z *= -1.0f;
				else
					tVelocities[1].z *= -1.0f;
			}

			tPositions[0].z += tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[0].z));
			tPositions[1].z -= tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[1].z));
		}

		pcTransforms[0]->SetPosition(tPositions[0]);
		pcTransforms[1]->SetPosition(tPositions[1]);
	}
}
void CPhysicsManager::ResolveCollision(CBoxCollider * pcA, CCapsuleCollider * pcB, TCollision & tCollision)
{
	CTransform* pcTransforms[2];
	CRigidbody* pcRigidbodies[2];
	pcTransforms[0] = pcA->m_pcOwner->GetComponent<CTransform>();
	pcTransforms[1] = pcB->m_pcOwner->GetComponent<CTransform>();
	pcRigidbodies[0] = pcA->m_pcOwner->GetComponent<CRigidbody>();
	pcRigidbodies[1] = pcB->m_pcOwner->GetComponent<CRigidbody>();

	// No need to move second object
	if (!pcRigidbodies[1] || CMath::Vector3Magnitude(pcRigidbodies[1]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[0]->GetPosition();
		tPosition.x += tCollision.tAxis.x * tCollision.fOffset;
		tPosition.y += tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z += tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[0]->SetPosition(tPosition);
	}

	// No need to move first object
	else if (!pcRigidbodies[0] || CMath::Vector3Magnitude(pcRigidbodies[0]->GetVelocity()) == 0)
	{
		CMath::TVECTOR3 tPosition = pcTransforms[1]->GetPosition();
		tPosition.x -= tCollision.tAxis.x * tCollision.fOffset;
		tPosition.y -= tCollision.tAxis.y * tCollision.fOffset;
		tPosition.z -= tCollision.tAxis.z * tCollision.fOffset;
		pcTransforms[1]->SetPosition(tPosition);
	}

	// Both objects are moving, so they need to be updated
	else
	{
		CMath::TVECTOR3 tVelocities[2] = { pcRigidbodies[0]->GetVelocity(), pcRigidbodies[1]->GetVelocity() };
		float fOverlap = tCollision.fOffset;

		float fSumX = fabsf(tVelocities[0].x) + fabsf(tVelocities[1].x);
		float fSumY = fabsf(tVelocities[0].y) + fabsf(tVelocities[1].y);
		float fSumZ = fabsf(tVelocities[0].z) + fabsf(tVelocities[1].z);

		CMath::TVECTOR3 tPositions[2] = { pcTransforms[0]->GetPosition(), pcTransforms[1]->GetPosition() };

		if (fSumX > 0)
		{
			if ((tVelocities[0].x > 0 && tVelocities[1].x > 0) || (tVelocities[0].x < 0 && tVelocities[1].x < 0))
			{
				if (fabsf(tVelocities[0].x) > fabsf(tVelocities[1].x))
					tVelocities[0].x *= -1.0f;
				else
					tVelocities[1].x *= -1.0f;
			}

			tPositions[0].x += tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[0].x));
			tPositions[1].x -= tCollision.tAxis.x * (fOverlap / fSumX * fabs(tVelocities[1].x));
		}

		if (fSumY > 0)
		{
			if ((tVelocities[0].y > 0 && tVelocities[1].y > 0) || (tVelocities[0].y < 0 && tVelocities[1].y < 0))
			{
				if (fabsf(tVelocities[0].y) > fabsf(tVelocities[1].y))
					tVelocities[0].y *= -1.0f;
				else
					tVelocities[1].y *= -1.0f;
			}

			tPositions[0].y += tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[0].y));
			tPositions[1].y -= tCollision.tAxis.y * (fOverlap / fSumY * fabsf(tVelocities[1].y));
		}

		if (fSumZ > 0)
		{
			if ((tVelocities[0].z > 0 && tVelocities[1].z > 0) || (tVelocities[0].z < 0 && tVelocities[1].z < 0))
			{
				if (fabsf(tVelocities[0].z) > fabsf(tVelocities[1].z))
					tVelocities[0].z *= -1.0f;
				else
					tVelocities[1].z *= -1.0f;
			}

			tPositions[0].z += tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[0].z));
			tPositions[1].z -= tCollision.tAxis.z * (fOverlap / fSumZ * fabs(tVelocities[1].z));
		}

		pcTransforms[0]->SetPosition(tPositions[0]);
		pcTransforms[1]->SetPosition(tPositions[1]);
	}
}

bool CPhysicsManager::IsNewCollision(ICollider * pcA, ICollider * pcB)
{
	// If there are no registered collisions in either collider, the collision is new
	if (m_cCollisions.count(pcA) == 0 || m_cCollisions.count(pcB) == 0)
		return true;

	for (ICollider* collider : m_cCollisions[pcA])
	{
		// Collider B was found in collision vector of A, so the collision existed before
		if (collider == pcB)
		{
			return false;
		}
	}

	// Collider B wasn't found, so the collision is new
	return true;
}
bool CPhysicsManager::IsCollisionExit(ICollider * pcA, ICollider * pcB, int& nIndexA, int& nIndexB)
{
	if (pcB->m_pcOwner->m_nEntityType == eEntity::DOOR)
	{
		int debug = 0;
	}
	// If there are no registered collisions in either collider, there was no collision before
	if (m_cCollisions.count(pcA) == 0 || m_cCollisions.count(pcB) == 0)
		return false;

	int nSizeA = (int)m_cCollisions[pcA].size();
	int nSizeB = (int)m_cCollisions[pcB].size();

	// If there are no empty lists
	if (nSizeA != 0 && nSizeB != 0)
	{
		int nLargestSize = nSizeA > nSizeB ? nSizeA : nSizeB;

		bool bFoundA = false;
		bool bFoundB = false;

		for (int i = 0; i < nLargestSize; i++)
		{
			// A full list of collisions has been searched and the collider wasn't found, so there was no collision before
			if ((i == nSizeA && !bFoundA) || (i == nSizeB && !bFoundB))
				return false;

			// Collision was found but is not colliding now, so the collision just ended
			if (!bFoundA && m_cCollisions[pcA][i] == pcB)
			{
				bFoundA = true;
				nIndexA = i;
			}

			// Collision was found but is not colliding now, so the collision just ended
			if (!bFoundB && m_cCollisions[pcB][i] == pcA)
			{
				bFoundB = true;
				nIndexB = i;
			}
		}

		// Collision was found
		if (bFoundA && bFoundB)
			return true;
	}

	// Collision wasn't found
	return false;
}

float CPhysicsManager::Clamp(float fValue, float fMin, float fMax)
{
	if (fValue < fMin) return fMin;
	if (fValue > fMax) return fMax;
	return fValue;
}
float CPhysicsManager::GetRatio(float fValue, float fMin, float fMax)
{
	return (fValue - fMin) / (fMax - fMin);
}

void CPhysicsManager::ClosestPtSegmentSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3 & tPs, CMath::TVECTOR3 & tQt)
{
	// General equation for the closest points in a line is: 
	// P(s) = P0 + s * u where P0 = starting point of line 1, u = direction vector of line 1, s = scalar
	// Q(t) = Q0 + t * v where Q0 = starting point of line 2, v = direction vector of line 2, t = scalar
	// s = (be - cd) / (ac - b^2)
	// t (ae-bd) / (ac - b^2)

	CMath::TVECTOR3 u = tP1 - tP0;
	CMath::TVECTOR3 v = tQ1 - tQ0;

	float a = CMath::Vector3Dot(u, u);
	float b = CMath::Vector3Dot(u, v);
	float c = CMath::Vector3Dot(v, v);
	float d = CMath::Vector3Dot(u, CMath::TVECTOR3(tP0 - tQ0));
	float e = CMath::Vector3Dot(v, CMath::TVECTOR3(tP0 - tQ0));

	float s = 0;
	float t = 0;
	float denominator = (a*c - b * b);

	// If denominator is not zero, calculate closest point scalar s on Line 1 to Line 2
	// Clamp s to make sure it's within the segment limits (original formula assumes infinite line)
	if (denominator != 0.0f)
	{
		s = Clamp((b*e - c * d) / denominator, 0.0f, 1.0f);
	}

	// Calculate closest point scalar t on Line 2 to P(s)
	// t = Dot((P0 + u*s) - Q0, v) / Dot(v,v) = (b*s + e) / c
	t = (b*s + e) / c;

	// If t is within 0 and 1 (Q(t) is within the segment) then finish
	// Else, clamp t and recompute to find s
	// s = Dot((Q0 + v*t) - P0, u) / Dot(u,u) = (b*t - d) / a
	if (t < 0.0f)
	{
		t = 0.0f;
		s = Clamp(-d / a, 0.0f, 1.0f);
	}

	else if (t > 1.0f)
	{
		t = 1.0f;
		s = Clamp((b - d) / a, 0.0f, 1.0f);
	}

	tPs = tP0 + u * s;
	tQt = tQ0 + v * t;
}
void CPhysicsManager::ClosestPtLineSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tDirection, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3 & tPs, CMath::TVECTOR3 & tQt)
{
	CMath::TVECTOR3 u = CMath::TVECTOR3(tDirection);
	CMath::TVECTOR3 v = tQ1 - tQ0;

	float a = CMath::Vector3Dot(u, u);
	float b = CMath::Vector3Dot(u, v);
	float c = CMath::Vector3Dot(v, v);
	float d = CMath::Vector3Dot(u, CMath::TVECTOR3(tP0 - tQ0));
	float e = CMath::Vector3Dot(v, CMath::TVECTOR3(tP0 - tQ0));

	float s = 0;
	float t = 0;
	float denominator = (a*c - b * b);

	if (denominator != 0.0f)
	{
		s = (b*e - c * d) / denominator;
	}

	t = Clamp((b*s + e) / c, 0.0f, 1.0f);

	tPs = tP0 + u * s;
	tQt = tQ0 + v * t;
}
void CPhysicsManager::ClosestPtPointOBB(CMath::TVECTOR3 tPoint, CBoxCollider * pcB, CMath::TVECTOR3 & tResult)
{
	CTransform* tTransform = pcB->m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tCenter = tTransform->GetPosition() + pcB->GetCenter();

	CMath::TVECTOR3 tMinAxis = tPoint - tCenter;

	tResult = tCenter;

	static const int nAxisCount = 3;
	CMath::TVECTOR3 tLocalAxes[nAxisCount];

	CMath::TMATRIX tTransformMatrix = tTransform->GetMatrix();

	tLocalAxes[0] = CMath::Vector3Normalize(CMath::TVECTOR3(tTransformMatrix.r[0].x, tTransformMatrix.r[0].y, tTransformMatrix.r[0].z));
	tLocalAxes[1] = CMath::Vector3Normalize(CMath::TVECTOR3(0, 1, 0));
	tLocalAxes[2] = CMath::Vector3Normalize(CMath::TVECTOR3(tTransformMatrix.r[2].x, tTransformMatrix.r[2].y, tTransformMatrix.r[2].z));

	CMath::TMATRIX tRotationMatrix = tTransformMatrix;
	tRotationMatrix.r[3].x = 0;
	tRotationMatrix.r[3].y = 0;
	tRotationMatrix.r[3].z = 0;
	tRotationMatrix.r[3].w = 1;

	//CMath::TVECTOR3 tExtent = CMath::Vector3Transform(pcB->GetExtent(), tRotationMatrix);
	CMath::TVECTOR3 tExtent = pcB->GetExtent();

	for (int i = 0; i < nAxisCount; i++)
	{
		float fDistance = CMath::Vector3Dot(tMinAxis, tLocalAxes[i]);

		// Temp assume infinite y
		if (i != 1)
		{
			if (fDistance > tExtent.mData[i])
				fDistance = tExtent.mData[i];
			if (fDistance < -tExtent.mData[i])
				fDistance = -tExtent.mData[i];
		}

		tResult = tResult + tLocalAxes[i] * fDistance;
	}
}
void CPhysicsManager::ClosestPtPointSegment(CMath::TVECTOR3 tPoint, CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3 & tResult)
{
	CMath::TVECTOR3 tSegment = tP1 - tP0;
	float fScalar = CMath::Vector3Dot(tPoint - tP0, tSegment) / CMath::Vector3Dot(tSegment, tSegment);

	fScalar = Clamp(fScalar, 0.0f, 1.0f);

	tResult = tP0 + tSegment * fScalar;
}

int CPhysicsManager::FindGridPositionX(CMath::TVECTOR3 tPosition, int nMin, int nMax)
{
	const int nCellWidth = CELL_SIZE;
	const int nCellCount = ROOM_WIDTH / nCellWidth;

	int nMid = (int)roundf(((float)(nMin + nMax) * 0.5f));

	if (tPosition.x < m_tRoomPosition.x + (nMid - (float)nCellCount * 0.5f) * nCellWidth)
	{
		if (nMid == nMin)
			return nMid;

		if (nMid == nMax)
			return nMid - 1;

		return FindGridPositionX(tPosition, nMin, nMid);
	}

	else
	{
		if (nMid == nMin)
			return nMid + 1;

		if (nMid == nMax)
			return nMid;
		return FindGridPositionX(tPosition, nMid, nMax);
	}

}
int CPhysicsManager::FindGridPositionZ(CMath::TVECTOR3 tPosition, int nMin, int nMax)
{
	const int nCellHeight = CELL_SIZE;
	const int nCellCount = ROOM_WIDTH / nCellHeight;

	int nMid = (int)roundf(((float)(nMin + nMax) / 2.0f));

	if (tPosition.z < m_tRoomPosition.z + (nMid - (float)nCellCount / 2.0f) * nCellHeight)
	{
		if (nMid == nMin)
			return nMid;

		if (nMid == nMax)
			return nMid - 1;

		return FindGridPositionZ(tPosition, nMin, nMid);
	}

	else
	{
		if (nMid == nMin)
			return nMid + 1;

		if (nMid == nMax)
			return nMid;
		return FindGridPositionZ(tPosition, nMid, nMax);
	}
}

bool CPhysicsManager::IsCollidingRayBox(CBoxCollider * pBoxCollider, CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tNormDirection, float fMaxDistance, CMath::TVECTOR3 & tCollisionPoint)
{
	/*  The formula to find the point in which a ray is at a specific time t is:
		Point p = ray.origin + ray.direction * t;

		This can be expanded into:
		point.x = ray.origin.x + ray.direction.x * tx
		point.y = ray.origin.y + ray.direction.y * ty
		point.z = ray.origin.z + ray.direction.z * tz

		Since we need to find t along the ray, we solve for t:
		tx = (point.x - ray.origin.x) / ray.direction.x
		ty = (point.y - ray.origin.y) / ray.direction.y
		tz = (point.z - ray.origin.z) / ray.direction.z

		Basically we need to find the time t in which the ray enters the limits of each axis and the time it exits:
		MinTx = (MinX - ray.origin.x) / ray.direction.x;
		MaxTx = (MaxX - ray.origin.x) / ray.direction.x;
		MinTy = (MinY - ray.origin.y) / ray.direction.y;
		MaxTy = (MaxY - ray.origin.y) / ray.direction.y;
		MinTz = (MinZ - ray.origin.z) / ray.direction.z;
		MaxTz = (MaxZ - ray.origin.z) / ray.direction.z;

		If the ray has entered the limits of all axis before it has exited any of them, then there is an intersection.

		The formula would be complete for an AABB, but since we're working with OOBs, we need to find to project the values to the local axis of the OOB.
		*/

	CTransform* pcTransform = pBoxCollider->m_pcOwner->GetComponent<CTransform>();
	CMath::TMATRIX tMatrix = pcTransform->GetMatrix();

	CMath::TVECTOR3 tCenterPosition = pcTransform->GetPosition() + pBoxCollider->GetCenter();
	CMath::TVECTOR3 tExtents = pBoxCollider->GetExtent();

	// Get the local axes of the OOB based on the transform of the entity
	const int nAxesCount = 3;
	CMath::TVECTOR3 tLocalAxes[nAxesCount];
	tLocalAxes[0] = CMath::TVECTOR3(tMatrix.r[0].x, tMatrix.r[0].y, tMatrix.r[0].z);
	tLocalAxes[1] = CMath::TVECTOR3(tMatrix.r[1].x, tMatrix.r[1].y, tMatrix.r[1].z);
	tLocalAxes[2] = CMath::TVECTOR3(tMatrix.r[2].x, tMatrix.r[2].y, tMatrix.r[2].z);

	// The vector from the origin of the ray to the center of the OOB
	CMath::TVECTOR3 tOriginToCenter = tCenterPosition - tOrigin;

	// The dot product of the normalized direction of the ray and the local axes gives the angle between them
	CMath::TVECTOR3 tAngleDirectionToLocalAxes = CMath::TVECTOR3(
		CMath::Vector3Dot(tLocalAxes[0], tNormDirection),
		CMath::Vector3Dot(tLocalAxes[1], tNormDirection),
		CMath::Vector3Dot(tLocalAxes[2], tNormDirection));

	// The dot product of the local axes and the vector from origin of the ray to center of OOB allows us to fin the min and max limit of the OOB
	CMath::TVECTOR3 tProjectedOriginToCenter = CMath::TVECTOR3(
		CMath::Vector3Dot(tLocalAxes[0], tOriginToCenter),
		CMath::Vector3Dot(tLocalAxes[1], tOriginToCenter),
		CMath::Vector3Dot(tLocalAxes[2], tOriginToCenter));

	float fTime[6] = { 0,0,0,0,0,0 };
	for (int i = 0; i < nAxesCount; i++)
	{
		if (tAngleDirectionToLocalAxes.mData[i] == 0)
			tAngleDirectionToLocalAxes.mData[i] = 0.0000001f;

		fTime[i * 2] = (tProjectedOriginToCenter.mData[i] - tExtents.mData[i]) / tAngleDirectionToLocalAxes.mData[i];
		fTime[i * 2 + 1] = (tProjectedOriginToCenter.mData[i] + tExtents.mData[i]) / tAngleDirectionToLocalAxes.mData[i];
	}

	//Get the greatest of the entry points
	float fGreatestMin = fmaxf(fmaxf(fminf(fTime[0], fTime[1]), fminf(fTime[2], fTime[3])), fminf(fTime[4], fTime[5]));

	// Get the smallest of the exit points
	float fSmallestMax = fminf(fminf(fmaxf(fTime[0], fTime[1]), fmaxf(fTime[2], fTime[3])), fmaxf(fTime[4], fTime[5]));

	if (fSmallestMax < 0.0f)
		return false;

	if (fGreatestMin > fSmallestMax)
		return false;

	float fDistanceAlongRay = fGreatestMin < 0.0f ? fSmallestMax : fGreatestMin;

	if (fDistanceAlongRay > fMaxDistance)
		return false;

	tCollisionPoint = tOrigin + tNormDirection * fDistanceAlongRay;

	return true;

}
bool CPhysicsManager::IsCollidingRayCapsule(CCapsuleCollider * pCapCollider, CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tNormDirection, float fMaxDistance, CMath::TVECTOR3 & tCollisionPoint)
{
	CMath::TVECTOR3 tCapsuleBottom, tCapsuleTop;
	pCapCollider->GetMiddleSegment(OUT tCapsuleBottom, OUT tCapsuleTop);

	CMath::TVECTOR3 tClosestPointRay, tClosestPointCapsule;
	ClosestPtSegmentSegment(tOrigin, tOrigin + tNormDirection * fMaxDistance, tCapsuleBottom, tCapsuleTop, OUT tClosestPointRay, OUT tClosestPointCapsule);

	float fDistanceBetweenPoints = CMath::Vector3Magnitude(tClosestPointCapsule - tClosestPointRay);

	if (fDistanceBetweenPoints <= pCapCollider->GetRadius())
	{
		float fCollisionDepth = pCapCollider->GetRadius() - fDistanceBetweenPoints;
		tCollisionPoint = tClosestPointRay - (tNormDirection * -1.0f) * fCollisionDepth;

		return true;
	}

	return false;
}

CEntityMask::CEntityMask(const int nEntityType)
{
	mask = 0;
	*this += nEntityType;
}
CEntityMask::CEntityMask(const int * nEntityTypes, int nCount)
{
	mask = 0;
	for (int i = 0; i < nCount; i++)
	{
		*this += nEntityTypes[i];
	}
}
CEntityMask CEntityMask::operator+(const int nEntityType) const
{
	CEntityMask copy;
	copy.mask = mask;

	copy.mask |= (nEntityType + 1) * (nEntityType + 1);

	return copy;
}
CEntityMask CEntityMask::operator-(const int nEntityType) const
{
	const int nSqEntityType = ((nEntityType + 1) * (nEntityType + 1));
	CEntityMask copy = *this;
	int compare = *this & nSqEntityType;
	
	copy.mask ^= ~(compare);

	return copy;
}
void CEntityMask::operator+=(const int nEntityType)
{
	*this = *this + nEntityType;
}
void CEntityMask::operator-=(const int nEntityType)
{
	*this = *this - nEntityType;
}
int CEntityMask::operator&(const int nMaskCheck) const
{
	return mask & nMaskCheck;
}
