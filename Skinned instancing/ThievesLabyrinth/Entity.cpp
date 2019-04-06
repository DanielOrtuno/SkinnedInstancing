#include "Entity.h"
#include "MageController.h"

#include "EnumTypes.h"

#include "Component.h"
#include "Level.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Stats.h"
#include "Transform.h"

#include <random>
#include <time.h>


class IComponent;
class ICollider;
class CRigidbody;
class CMeshRenderer;
class CFireDemonController;
class CCameraController;
class CAnimatorComponent;

#pragma region Base Clase

IEntity::IEntity(int nId)
{
	m_nEntityId = nId;
	m_bState = true;
	m_pcRoom = nullptr;
}

bool IEntity::IsActive()
{
	return m_bState;
}

void IEntity::SetActiveState(bool bState)
{
	m_bState = bState;

	for(IComponent* pcComponent : m_pcComponents)
	{
		pcComponent->SetActiveState(m_bState);
	}
}

void IEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
}

void IEntity::OnTriggerStay(IEntity * pcCollidingEntity)
{
}

void IEntity::OnTriggerExit(IEntity * pcCollidingEntity)
{
}

void IEntity::OnCollisionEnter(IEntity * pcCollidingEntity)
{
}

void IEntity::OnCollisionStay(IEntity * pcCollidingEntity)
{
}

void IEntity::OnCollisionExit(IEntity * pcCollidingEntity)
{
}

IEntity::~IEntity()
{
}

#pragma endregion

#pragma region Player Entity

CPlayerEntity::CPlayerEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::PLAYER;

	//(CTransform*)CEntityManager::AddComponentToEntity(m_pcPlayerLight, eComponent::TRANSFORM);
}

void CPlayerEntity::Update()
{
	//Run controller update;
}

void CPlayerEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
  	if(pcCollidingEntity == nullptr)
		return;

	if(pcCollidingEntity->m_nEntityType == eEntity::PROJECTILE && pcCollidingEntity->m_nEntityId != eEntity::PLAYER)
	{
		CProjectileComponent* pcProjectileComponent = pcCollidingEntity->GetComponent<CProjectileComponent>();
		if(pcProjectileComponent->GetCasterEntity() != eEntity::PLAYER)
		{
			CStats* pcStats = GetComponent<CStats>();

			pcStats->SetHP((float)pcStats->GetHP() - pcCollidingEntity->GetComponent<CProjectileComponent>()->GetDamage());

			//CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::PLAYERDEATH));
			srand((unsigned)time(NULL));
			int nRand = rand() % (eSFX::HIT4 - eSFX::HIT + 1) + eSFX::HIT;
			CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, nRand));

			if(pcStats->GetHP() <= 0)
			{

			}
		}
	}
}

void CPlayerEntity::OnTriggerExit(IEntity * pcCollidingEntity)
{
	if (pcCollidingEntity == nullptr)
	{
		return;
	}

	if (pcCollidingEntity->m_nEntityType == eEntity::DOOR)
	{
		CMath::TVECTOR3 PlayerPosition = GetComponent<CTransform>()->GetPosition();
		CMath::TVECTOR3 ColliderPosition = pcCollidingEntity->GetComponent<CTransform>()->GetPosition();

		CLightComponent* pcLight = GetComponent<CLightComponent>();

		if(pcLight)
		{
			m_pcRoom->RemoveLight(pcLight);
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[0]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Right Door (Camera Right) 
			if (PlayerPosition.x > ColliderPosition.x)
			{
				for (int i = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size() - 1; i >= 0; i--)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Top == true)
					{
						for (int j = 0; j < pcCollidingEntity->m_pcRoom->m_pcNeighbors.size(); j++)
						{
							
								pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
								pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();
							
						}

						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();
						
						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);

						this->m_pcRoom = CLevel::m_pcCurrentRoom;

						if(pcLight)
						{
							m_pcRoom->AddLight(pcLight);
						}
						return;
					}
				}
			}

			if (PlayerPosition.x < ColliderPosition.x)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);

				this->m_pcRoom = CLevel::m_pcCurrentRoom;
				if(pcLight)
				{
					m_pcRoom->AddLight(pcLight);
				}
				return;
			}
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[3]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Bottom Door (Camera Bottom)
			if (PlayerPosition.z < ColliderPosition.z)
			{
				for (int i = 0; i < pcCollidingEntity->m_pcRoom->m_pcNeighbors.size(); i++)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Right == true)
					{
						for (int j = 0; j < pcCollidingEntity->m_pcRoom->m_pcNeighbors.size(); j++)
						{

							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();

						}

						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();

						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);

						this->m_pcRoom = CLevel::m_pcCurrentRoom;
						if(pcLight)
						{
							m_pcRoom->AddLight(pcLight);
						}

						return;
					}
				}
			}

			if (PlayerPosition.z > ColliderPosition.z)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);

				this->m_pcRoom = CLevel::m_pcCurrentRoom;
				if(pcLight)
				{
					m_pcRoom->AddLight(pcLight);
				}

				return;
			}
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[2]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Top Door (Camera Top)
			if (PlayerPosition.z > ColliderPosition.z)
			{
				for (int i = (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size() - 1; i >= 0; i--)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Left == true)
					{
						for (int j = 0; j < pcCollidingEntity->m_pcRoom->m_pcNeighbors.size(); j++)
						{
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();
						}


						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();




						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);

						this->m_pcRoom = CLevel::m_pcCurrentRoom;
						if(pcLight)
						{
							m_pcRoom->AddLight(pcLight);
						}

						return;
					}
				}
			}

			if (PlayerPosition.z < ColliderPosition.z)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);

				this->m_pcRoom = CLevel::m_pcCurrentRoom;
				if(pcLight)
				{
					m_pcRoom->AddLight(pcLight);
				}

				return;
			}
		}

		if (pcCollidingEntity->m_pcRoom->m_pcDoors[1]->m_nEntityId == pcCollidingEntity->m_nEntityId)
		{
			//Left Door (Camera Left)
			if (PlayerPosition.x < ColliderPosition.x)
			{
				for (int i = 0; i < (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size(); i++)
				{
					if (pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->Bottom == true)
					{
						for (int j = 0; j < (int)pcCollidingEntity->m_pcRoom->m_pcNeighbors.size(); j++)
						{

							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnDoors();
							pcCollidingEntity->m_pcRoom->m_pcNeighbors[j]->DeSpawnEntities();

						}


						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnEntities();
						pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]->SpawnDoors();




						CLevel::Update(pcCollidingEntity->m_pcRoom->m_pcNeighbors[i]);

						this->m_pcRoom = CLevel::m_pcCurrentRoom;

						return;
					}
				}
			}

			if (PlayerPosition.x > ColliderPosition.x)
			{
				CLevel::Update(pcCollidingEntity->m_pcRoom);
				CLevel::SetDoorsTrue(pcCollidingEntity->m_pcRoom);

				this->m_pcRoom = CLevel::m_pcCurrentRoom;

				if(pcLight)
				{
					m_pcRoom->AddLight(pcLight);
				}

				return;
			}
		}
	}

	
}

CPlayerEntity::~CPlayerEntity()
{

}

#pragma endregion

#pragma region Enemy Entity

CEnemyEntity::CEnemyEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::ENEMY;
}

void CEnemyEntity::Update()
{
	//Run controller update;
}

void CEnemyEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
	if(!pcCollidingEntity)
		return;

	if(pcCollidingEntity->m_nEntityType == eEntity::PROJECTILE)
	{ 
		CProjectileComponent* pcProjectileComponent = pcCollidingEntity->GetComponent<CProjectileComponent>();
		if(pcProjectileComponent->GetCasterEntity() != eEntity::ENEMY)
		{
			CStats* pcStats = GetComponent<CStats>();
			pcStats->SetHP((float)pcStats->GetHP() - pcCollidingEntity->GetComponent<CProjectileComponent>()->GetDamage());
			//CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::PLAYERDEATH));
			srand((unsigned)time(NULL));
			int nRand = rand() % (eSFX::HIT4 - eSFX::HIT + 1) + eSFX::HIT;
			CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, nRand));

			if(pcStats->GetHP() <= 0)
			{
				m_pcRoom->m_nEnemyCount--;
				CLevel::UpdateDoors(this->m_pcRoom);

				CEntityManager::AddEntityToDeletionQueue(this);	
			}
		}
	}
}

void CEnemyEntity::OnTriggerStay(IEntity * pcCollidingEntity)
{
	if (pcCollidingEntity->m_nEntityType == eEntity::PROJECTILE )
	{
		CProjectileComponent* pcProjectileComponent = pcCollidingEntity->GetComponent<CProjectileComponent>();
		if (pcProjectileComponent->GetCasterEntity() != eEntity::ENEMY)
		{
			CStats* pcStats = GetComponent<CStats>();
			pcStats->SetHP((float)pcStats->GetHP() - pcCollidingEntity->GetComponent<CProjectileComponent>()->GetDamage());

			//CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, eSFX::HIT));
			//CEventManager::ReceiveSoundRequest(true, eAudio::SFX, eSFX::HIT);

			if (pcStats->GetHP() <= 0)
			{
				m_pcRoom->m_nEnemyCount--;
				CLevel::UpdateDoors(this->m_pcRoom);

				CEntityManager::AddEntityToDeletionQueue(this);
			}

		}
	}

}

CEnemyEntity::~CEnemyEntity()
{

}

#pragma endregion

#pragma region Projectile Entity

CProjectileEntity::CProjectileEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::PROJECTILE;
}

void CProjectileEntity::OnTriggerEnter(IEntity * pcCollidingEntity)
{
	if(GetComponent<CProjectileComponent>()->GetCasterEntity() != pcCollidingEntity->m_nEntityType && pcCollidingEntity->m_nEntityType != eEntity::PROJECTILE && this->GetComponent<CBoxCollider>() != nullptr)
	{
		CEntityManager::AddEntityToDeletionQueue(this);
	}
}

CProjectileEntity::~CProjectileEntity()
{

}

#pragma endregion

#pragma region Camera Entity

CCameraEntity::CCameraEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::CAMERA;
}

void CCameraEntity::Update()
{
	//Run controller update;
}

CCameraEntity::~CCameraEntity()
{
}

#pragma endregion

#pragma region Room Entity

#define MAX_LIGHTS 16

CRoomEntity::CRoomEntity(int nId) : IEntity(nId)
{
	m_nEntityType = eEntity::ROOM;
	Top = false;
	Bottom = false;
	Right = false;
	Left = false;
}

void CRoomEntity::SpawnEntities()
{
	if (Entities == true)
	{
		return;
	}

	CTransform* pRoomTransform = GetComponent<CTransform>();

	for (int i = 0; i < m_nEnemyCount; i++)
	{
		m_pcEnemies[i]->m_pcRoom = this;

		if (m_pcEnemies[i]->m_pcRoom != CLevel::m_pcCurrentRoom)
		{
			m_pcEnemies[i]->SetActiveState(false);
			m_pcEnemies[i]->GetComponent<CMeshRenderer>()->SetActiveState(true);
		}
		else
		{
			m_pcEnemies[i]->SetActiveState(true);
		}

		m_pcEnemies[i]->GetComponent<CTransform>()->SetPosition(m_pcEnemies[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcEnemies[i]->GetComponent<CMageController>()->GetPlayerReference();
	}

	int Wallsize = (int)m_pcWalls.size();
	for (int i = 0; i < Wallsize; i++)
	{
		m_pcWalls[i]->m_pcRoom = this;
		m_pcWalls[i]->GetComponent<CTransform>()->SetPosition(m_pcWalls[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcWalls[i]->GetComponent<CTransform>()->SetScale(CMath::TVECTOR3(1.3f, 1.0f, 1.3f));
		
		m_pcWalls[i]->SetActiveState(true);
	}

	for (int i = 0; i < m_pcPillars.size(); i++)
	{
		m_pcPillars[i]->m_pcRoom = this;
		m_pcPillars[i]->GetComponent<CTransform>()->SetPosition(m_pcPillars[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		m_pcPillars[i]->SetActiveState(true);
	}

	Entities = true;
}

void CRoomEntity::DeSpawnEntities()
{
	if (Entities == false)
	{
		return;
	}

	CTransform* pRoomTransform = GetComponent<CTransform>();

	for (int i = 0; i < m_nEnemyCount; i++)
	{
		m_pcEnemies[i]->GetComponent<CTransform>()->SetPosition(m_pcEnemies[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcEnemies[i]->SetActiveState(false);
	}

	int Wallsize = (int)m_pcWalls.size();
	for (int i = 0; i < Wallsize; i++)
	{
		m_pcWalls[i]->GetComponent<CTransform>()->SetPosition(m_pcWalls[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcWalls[i]->SetActiveState(false);
	}

	for (int i = 0; i < m_pcPillars.size(); i++)
	{
		
		m_pcPillars[i]->GetComponent<CTransform>()->SetPosition(m_pcPillars[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcPillars[i]->SetActiveState(false);
	}

	Entities = false;
}

void CRoomEntity::SpawnDoors()
{
	if (Doors == true)
	{
		return;
	}

	CTransform* pRoomTransform = GetComponent<CTransform>();

	int DoorSize = (int)m_pcDoors.size();
	for (int i = 0; i < DoorSize; i++)
	{
		m_pcDoors[i]->m_pcRoom = this;
		m_pcDoors[i]->GetComponent<CTransform>()->SetPosition(m_pcDoors[i]->GetComponent<CTransform>()->GetPosition() + pRoomTransform->GetPosition());
		
		m_pcDoors[i]->SetActiveState(true);
		
	}

	Doors = true;
}

void CRoomEntity::DeSpawnDoors()
{
	if (Doors == false)
	{
		return;
	}

	CTransform* pRoomTransform = GetComponent<CTransform>();

	int DoorSize = (int)m_pcDoors.size();
	for (int i = 0; i < DoorSize; i++)
	{
		m_pcDoors[i]->m_pcRoom = this;
		m_pcDoors[i]->GetComponent<CTransform>()->SetPosition(m_pcDoors[i]->GetComponent<CTransform>()->GetPosition() - pRoomTransform->GetPosition());
		m_pcDoors[i]->GetComponent<CBoxCollider>()->SetTrigger(false);
		m_pcDoors[i]->SetActiveState(false);
	}

	Doors = false;
}

void CRoomEntity::TurnEnemiesOn()
{
	for (int i = 0; i < m_nEnemyCount; i++)
	{
		m_pcEnemies[i]->SetActiveState(true);
	}
}

void CRoomEntity::AddLight(CLightComponent * pNewLight)
{
	if(m_pLights.size() < MAX_LIGHTS)
	{
		m_pLights.push_back(pNewLight);
	}
}

void CRoomEntity::RemoveLight(CLightComponent * pLight)
{
	for (int i = 0; i < m_pLights.size(); i++)
	{
		if (m_pLights[i] == pLight)
		{
			m_pLights.erase(m_pLights.begin() + i);
			break;
		}
	}
}

std::vector<CLightComponent*> CRoomEntity::GetLights()
{
	return m_pLights;
}

CRoomEntity::~CRoomEntity()
{

}

#pragma endregion

#pragma region Environment Entity

CEnvironmentEntity::CEnvironmentEntity(int nID) : IEntity(nID)
{
	m_nEntityType = eEntity::ENVIRONMENT;
}

CEnvironmentEntity::~CEnvironmentEntity()
{

}

#pragma endregion

#pragma region DoorEntity

CDoorEntity::CDoorEntity(int nID) : IEntity(nID)
{
	m_nEntityType = eEntity::DOOR;
}

CDoorEntity::~CDoorEntity()
{
}

#pragma endregion