#include "EntityManager.h"
#include "Entity.h"

#include "MageController.h"

#include "Rigidbody.h"
#include "AnimatorComponent.h"
#include "MeshRenderer.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "Transform.h"

#include "EnumTypes.h"
#include "Stats.h"

CComponentManager*				CEntityManager::m_pcComponentManager;

CPlayerEntity*					CEntityManager::m_pcPlayer;

std::vector<CEnemyEntity*>		 CEntityManager::m_pcEnemies;
std::vector<CProjectileEntity*>	 CEntityManager::m_pcProjectiles;
CCameraEntity*					 CEntityManager::m_pcCamera;
std::vector<CRoomEntity*>		 CEntityManager::m_pcRooms;
std::vector<CEnvironmentEntity*> CEntityManager::m_pcEnvironmentEntities;
std::vector<CDoorEntity*>		 CEntityManager::m_pcDoorEntities;
std::vector<IEntity*>			 CEntityManager::m_pcDeletionQueue;

int								 CEntityManager::m_nIdCount;


CEntityManager::CEntityManager(CComponentManager* pcComponentManager)
{
	m_nIdCount = 1;
	m_pcComponentManager = pcComponentManager;
}

IEntity* CEntityManager::CreateEntity(int nType, int nOption)
{
	//USE ALLOCATOR
	IEntity* pcResult;

	switch(nType)
	{
		case eEntity::PLAYER:
		{
			m_pcPlayer = new CPlayerEntity(0);
			pcResult = m_pcPlayer;

			break;
		}

		case eEntity::ENVIRONMENT:
		{
			pcResult = new CEnvironmentEntity(m_nIdCount++);
			m_pcEnvironmentEntities.push_back((CEnvironmentEntity*)pcResult);
			break;
		}

		case eEntity::ENEMY:
		{
			pcResult = new CEnemyEntity(m_nIdCount++);
			m_pcEnemies.push_back((CEnemyEntity*)pcResult);
		/*	switch(nOption)
			{
				case eEnemyType::KNIGHT:
				{
					break;
				}
				case eEnemyType::BERSERKER:
				{
					break;
				}
				case eEnemyType::MAGE:
				{
					break;
				}
				case eEnemyType::ARCHER:
				{
					break;
				}
			
				case eEnemyType::CHICKEN:
				{
					break;
				}

			default:
				break;
			}*/
			break;

		}

		case eEntity::PROJECTILE:
		{	
			pcResult = new CProjectileEntity(m_nIdCount++);
			m_pcProjectiles.push_back((CProjectileEntity*)pcResult);
			break;
		}

		case eEntity::CAMERA:
		{
			m_pcCamera = new CCameraEntity(m_nIdCount++);
			pcResult = m_pcCamera;

			break;
		}

		case eEntity::ROOM:
		{
			pcResult = new CRoomEntity(m_nIdCount++);
			m_pcRooms.push_back((CRoomEntity*)pcResult);

			break;
		}

		case eEntity::DOOR:
		{
			pcResult = new CDoorEntity(m_nIdCount++);
			m_pcDoorEntities.push_back((CDoorEntity*)pcResult);

			break;
		}

		default:
		{
			pcResult = nullptr;
			break;
		}
	}

	return pcResult;
}

IComponent* CEntityManager::AddComponentToEntity(IEntity* pcEntity, int nComponentType)
{
	IComponent* pcNewComponent = m_pcComponentManager->CreateComponent(pcEntity, nComponentType);
	if(pcNewComponent != nullptr)
	{
		pcEntity->m_pcComponents.push_back(pcNewComponent);
	}

	return pcNewComponent;
}

int CEntityManager::RemoveComponentFromEntity(IEntity * pcEntity, int nComponentType)
{
	for(unsigned int i = 0; i < pcEntity->m_pcComponents.size(); i++)
	{
		if(pcEntity->m_pcComponents[i]->m_nComponentType == nComponentType)
		{
			m_pcComponentManager->DeleteComponent(pcEntity->m_pcComponents[i]->m_nComponentType, pcEntity->m_pcComponents[i]);

			pcEntity->m_pcComponents.erase(pcEntity->m_pcComponents.begin() + i);

			
			return 0;
		}
	}

	return - 1;
}

void CEntityManager::AddEntityToDeletionQueue(IEntity * pcEntity)
{
	m_pcDeletionQueue.push_back(pcEntity);
}

int CEntityManager::DeleteEntity(IEntity* pcEntity)
{
	for(IComponent* component : pcEntity->m_pcComponents)
	{
		m_pcComponentManager->DeleteComponent(component->m_nComponentType, component);
	}

	switch(pcEntity->m_nEntityType)
	{
		case eEntity::PLAYER:
		{
			delete m_pcPlayer;
			m_pcPlayer = nullptr;

			break;
		}

		case eEntity::ENEMY:
		{
			for(unsigned int i = 0; i < m_pcEnemies.size(); i++)
			{
				if(m_pcEnemies[i] == pcEntity)
				{
					m_pcEnemies.erase(m_pcEnemies.begin() + i);
					break;
				}
			}

			delete (CEnemyEntity*)pcEntity;
			break;
		}

		case eEntity::PROJECTILE:
		{
			for(unsigned int i = 0; i < m_pcProjectiles.size(); i++)
			{
				if(m_pcProjectiles[i] == pcEntity)
				{
					m_pcProjectiles.erase(m_pcProjectiles.begin() + i);
					break;
				}
			}

			delete (CProjectileEntity*)pcEntity;
			break;
		}

		case eEntity::CAMERA:
		{
			
			delete m_pcCamera;

			m_pcCamera = nullptr;
			break;
		}

		case eEntity::ROOM:
		{
			for (unsigned int i = 0; i < m_pcRooms.size(); i++)
			{
				if (m_pcRooms[i] == pcEntity)
				{
					m_pcRooms.erase(m_pcRooms.begin() + i);
					break;
				}
			}

			delete (CRoomEntity*)pcEntity;
			break;
		}

		case eEntity::ENVIRONMENT:
		{
			for (unsigned int i = 0; i < m_pcEnvironmentEntities.size(); i++)
			{
				if (m_pcEnvironmentEntities[i] == pcEntity)
				{
					m_pcEnvironmentEntities.erase(m_pcEnvironmentEntities.begin() + i);
					break;
				}
			}
		
			delete (CEnvironmentEntity*)pcEntity;
			break;
		}

		case eEntity::DOOR:
		{
			for (unsigned int i = 0; i < m_pcDoorEntities.size(); i++)
			{
				if (m_pcDoorEntities[i] == pcEntity)
				{
					m_pcDoorEntities.erase(m_pcDoorEntities.begin() + i);

					break;
				}
			}

			delete (CDoorEntity*)pcEntity;
			break;
		}

		default:
		{
			return -1;

			break;
		}
	}

	return 0;
}

int CEntityManager::GetEntityCount(int nType)
{
	switch(nType)
	{
		case eEntity::PLAYER:
		{
			return m_pcPlayer != nullptr ? 1 : 0;

			break;
		}

		case eEntity::ENEMY:
		{
			return (int)m_pcEnemies.size();
			break;
		}

		case eEntity::PROJECTILE:
		{
			return (int)m_pcProjectiles.size();
			break;
		}

		case eEntity::CAMERA:
		{
			return m_pcCamera != nullptr ? 1 : 0;
			break;
		}

		case eEntity::ROOM:
		{
			return (int)m_pcRooms.size();
			break;
		}

		case eEntity::DOOR:
		{
			return (int)m_pcDoorEntities.size();
			break;
		}

		default:
		{
			return -1;
			break;
		}

	}
}

IEntity* CEntityManager::GetEntity(int nID)
{
	if(m_pcPlayer->m_nEntityId == nID)
	{
		return m_pcPlayer;
	}

	if(m_pcCamera->m_nEntityId == nID)
	{
		return m_pcCamera;
	}

	for(int i = 0; i < m_pcEnemies.size(); i++)
	{
		if(m_pcEnemies[i]->m_nEntityId == nID)
		{
			return m_pcEnemies[i];
		}
	}

	for(int i = 0; i < m_pcProjectiles.size(); i++)
	{
		if(m_pcProjectiles[i]->m_nEntityId == nID)
		{
			return m_pcProjectiles[i];
		}

	}

	for(int i = 0; i < m_pcRooms.size(); i++)
	{
		if(m_pcRooms[i]->m_nEntityId == nID)
		{
			return m_pcRooms[i];
		}
	}

	for (int i = 0; i < m_pcDoorEntities.size(); i++)
	{
		if (m_pcDoorEntities[i]->m_nEntityId == nID)
		{
			return m_pcDoorEntities[i];
		}
	}

	for (int i = 0; i < m_pcEnvironmentEntities.size(); i++)
	{
		if (m_pcEnvironmentEntities[i]->m_nEntityId == nID)
		{
			return m_pcEnvironmentEntities[i];
		}
	}

	return nullptr;
}

void CEntityManager::DeleteAllEntities()
{
	if (m_pcPlayer != nullptr)
	{
		DeleteEntity(m_pcPlayer);
	}

	if (m_pcCamera != nullptr)
	{
		DeleteEntity(m_pcCamera);
	}

	for (int i = (int)m_pcEnemies.size() - 1; i >= 0; i--)
	{
		DeleteEntity(m_pcEnemies[i]);
	}

	for (int i = (int)m_pcProjectiles.size() - 1; i >= 0; i--)
	{
		DeleteEntity(m_pcProjectiles[i]);
	}


	for (int i = (int)m_pcEnvironmentEntities.size() - 1; i >= 0; i--)
	{
		DeleteEntity(m_pcEnvironmentEntities[i]);
	}

	for (int i = (int)m_pcRooms.size() - 1; i >= 0; i--)
	{
		DeleteEntity(m_pcRooms[i]);
	}

	for (int i = (int)m_pcDoorEntities.size() - 1; i >= 0; i--)
	{
		DeleteEntity(m_pcDoorEntities[i]);
	}
	
}

IEntity* CEntityManager::CloneEntity(IEntity * pcEntityToCopy)
{
	IEntity* pNewEntity = CEntityManager::CreateEntity(pcEntityToCopy->m_nEntityType);
	
	for(IComponent* pcComponent : pcEntityToCopy->m_pcComponents)
	{
		IComponent* pcNewComponent = CEntityManager::AddComponentToEntity(pNewEntity, pcComponent->m_nComponentType);
		
		switch(pcComponent->m_nComponentType)
		{
			case eComponent::TRANSFORM:
			{
				CTransform* temp = (CTransform*)pcNewComponent;
				*temp = *(CTransform*)pcComponent;
				break;
			}

			case eComponent::BOX_COLLIDER:
			{
				CBoxCollider* temp = (CBoxCollider*)pcNewComponent;
				*temp = *(CBoxCollider*)pcComponent;

				break;
			}

			case eComponent::CAPSULE_COLLIDER:
			{
				CCapsuleCollider* temp = (CCapsuleCollider*)pcNewComponent;
				*temp = *(CCapsuleCollider*)pcComponent;

				break;
			}
			case eComponent::RIGIDBODY:
			{
				CRigidbody* temp = (CRigidbody*)pcNewComponent;
				*temp = *(CRigidbody*)pcComponent;

				break;
			}
			case eComponent::MESH_RENDERER:
			{
				CMeshRenderer* temp = (CMeshRenderer*)pcNewComponent;
				*temp = *(CMeshRenderer*)pcComponent;

				break;
			}

			case eComponent::STATS:
			{
				CStats* temp = (CStats*)pcNewComponent;
				*temp = *(CStats*)pcComponent;

				break;
			}

			case eComponent::ANIMATOR:
			{
				CAnimatorComponent* temp = (CAnimatorComponent*)pcNewComponent;
				*temp = *(CAnimatorComponent*)pcComponent;

				break;
			}

			case eComponent::MAGE_CONTROLLER:
			{
				CMageController* temp = (CMageController*)pcNewComponent;
				*temp = *(CMageController*)pcComponent;

				break;
			}

			case eComponent::PROJECTILE_COMPONENT:
			{
				CProjectileComponent* temp = (CProjectileComponent*)pcNewComponent;
				*temp = *(CProjectileComponent*)pcComponent;

				break;
			}

			case eComponent::LIGHT:
			{
				CLightComponent* temp = (CLightComponent*)pcNewComponent;
				*temp = *(CLightComponent*)pcComponent;

				break;
			}
		}
	}

	if (pNewEntity->m_nEntityType == eEntity::ROOM)
	{
		CRoomEntity* pcNewRoom = (CRoomEntity*)pNewEntity;
		CRoomEntity* pcRoomToCopy = (CRoomEntity*)pcEntityToCopy;
		
		pcNewRoom->m_nEnemyCount = pcRoomToCopy->m_nEnemyCount;

		for (CEnemyEntity* pcEnemyEntity : pcRoomToCopy->m_pcEnemies)
		{
			pcNewRoom->m_pcEnemies.push_back((CEnemyEntity*)CloneEntity(pcEnemyEntity));
		}

		for (CEnvironmentEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcWalls)
		{
			pcNewRoom->m_pcWalls.push_back((CEnvironmentEntity*)CloneEntity(pcEnvironmentEntity));
		}

		for (CEnvironmentEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcPillars)
		{
			pcNewRoom->m_pcPillars.push_back((CEnvironmentEntity*)CloneEntity(pcEnvironmentEntity));
		}

		for (CDoorEntity* pcEnvironmentEntity : pcRoomToCopy->m_pcDoors)
		{
			pcNewRoom->m_pcDoors.push_back((CDoorEntity*)CloneEntity(pcEnvironmentEntity));
		}

		

		if (pcRoomToCopy->IsActive() == true)
		{
			pcNewRoom->SetActiveState(true);
		}
		else
		{
			pcNewRoom->SetActiveState(false);
		}

		return pcNewRoom;
	}
	
	pNewEntity->m_pcRoom = pcEntityToCopy->m_pcRoom;

	if (pcEntityToCopy->IsActive() == true)
	{
		pNewEntity->SetActiveState(true);
	}
	else
	{
		pNewEntity->SetActiveState(false);
	}

	
	return pNewEntity;
}

void CEntityManager::NotifyCollisionMessage(int nCollider, int nCollidingWith, int nCollisionType)
{
	IEntity* pcEntityA = GetEntity(nCollider);
	IEntity* pcEntityB = GetEntity(nCollidingWith);

	switch(nCollisionType)
	{
		case eCollisionState::COLLISION_ENTER:
		{
			if(pcEntityA && pcEntityB)
			{
				pcEntityA->OnCollisionEnter(pcEntityB);
			}

			if(pcEntityA && pcEntityB)
			{
				pcEntityB->OnCollisionEnter(pcEntityA);
			}
			break;
		}

		case eCollisionState::COLLISION_STAY:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnCollisionStay(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnCollisionStay(pcEntityA);

			break;
		}

		case eCollisionState::COLLISION_EXIT:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnCollisionExit(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnCollisionExit(pcEntityA);

			break;
		}

		case eCollisionState::TRIGGER_ENTER:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnTriggerEnter(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnTriggerEnter(pcEntityA);

			break;
		}

		case eCollisionState::TRIGGER_STAY:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnTriggerStay(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnTriggerStay(pcEntityA);

			break;
		}

		case eCollisionState::TRIGGER_EXIT:
		{
			if(pcEntityA && pcEntityB)
			pcEntityA->OnTriggerExit(pcEntityB);
			if(pcEntityA && pcEntityB)
			pcEntityB->OnTriggerExit(pcEntityA);

			break;
		}

		default:
			break;
		}
	}


void CEntityManager::ProcessDeletionQueue()
{
	for(IEntity* pEntity : m_pcDeletionQueue)
	{
		if(pEntity)
		{
			DeleteEntity(pEntity);
		}
	}

	m_pcDeletionQueue.clear();
}

CEntityManager::~CEntityManager()
{
	DeleteAllEntities();
}
