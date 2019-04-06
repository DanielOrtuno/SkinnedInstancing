#pragma once
#include "EnumTypes.h"
#include "Math.h"

#include <vector>

class IComponent;


class CBoxCollider;
class CCapsuleCollider;
class CRigidbody;
class CMeshRenderer;
class CFireDemonController;
class CCameraController;
class CAnimatorComponent;
class CMageController;
class CStats;
class CProjectileComponent;
class CRoomEntity;
class CInventory;
class CTransform;
class CLightComponent;

class IEntity
{
	bool m_bState;

public:
	IEntity(int nId);
	
	int m_nEntityId;
	int m_nEntityType;

	CRoomEntity* m_pcRoom;

	std::vector<IComponent*> m_pcComponents;

	template <typename T>
	T* GetComponent()
	{
		for (IComponent* pnComponent : m_pcComponents)
		{
			switch (pnComponent->m_nComponentType)
			{
				case eComponent::TRANSFORM:
				{
					if (std::is_same<T, CTransform>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

			case eComponent::BOX_COLLIDER:
				{
				if (std::is_same<T, CBoxCollider>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

			case eComponent::CAPSULE_COLLIDER:
			{
				if (std::is_same<T, CCapsuleCollider>::value)
				{
					return (T*)pnComponent;
				}

				break;
			}

				case eComponent::RIGIDBODY:
				{
					if (std::is_same<T, CRigidbody>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::MESH_RENDERER:
				{
					if (std::is_same<T, CMeshRenderer>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::FIRE_DEMON_CONTROLLER:
				{
					if (std::is_same<T, CFireDemonController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::CAMERA_CONTROLLER:
				{
					if (std::is_same<T, CCameraController>::value)
					{
						return (T*)pnComponent;
					}
					break;
				}
			
				case eComponent::ANIMATOR:
				{
					if(std::is_same<T, CAnimatorComponent>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::MAGE_CONTROLLER:
				{
					if(std::is_same<T, CMageController>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::STATS:
				{
					if(std::is_same<T, CStats>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::PROJECTILE_COMPONENT:
				{
					if(std::is_same<T, CProjectileComponent>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::INVENTORY:
				{
					if (std::is_same<T, CInventory>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}

				case eComponent::LIGHT:
				{
					if(std::is_same<T, CLightComponent>::value)
					{
						return (T*)pnComponent;
					}

					break;
				}
			}
		}

		return nullptr;
	}


	bool IsActive();

	void SetActiveState(bool bState);

	virtual void OnTriggerEnter(IEntity* pcCollidingEntity);
	virtual void OnTriggerStay(IEntity* pcCollidingEntity);
	virtual void OnTriggerExit(IEntity* pcCollidingEntity);

	virtual void OnCollisionEnter(IEntity* pcCollidingEntity);
	virtual void OnCollisionStay(IEntity* pcCollidingEntity);
	virtual void OnCollisionExit(IEntity* pcCollidingEntity);

	virtual~IEntity() = 0;
};

class CPlayerEntity : public IEntity
{
public:
	CPlayerEntity(int nId = -1);

	void Update();

	void OnTriggerEnter(IEntity* pcCollidingEntity);

	void OnTriggerExit(IEntity* pcCollidingEntity);

	~CPlayerEntity();
};

class CEnemyEntity : public IEntity
{
public:
	CEnemyEntity(int nId = -1);

	void Update();

	void OnTriggerEnter(IEntity* pcCollidingEntity);
	void OnTriggerStay(IEntity* pcCollidingEntity);
	//void OnTriggerExit(IEntity* pcCollidingEntity);

	//void OnCollisionEnter(IEntity* pcCollidingEntity);
	//void OnCollisionStay(IEntity* pcCollidingEntity);
	//void OnCollisionExit(IEntity* pcCollidingEntity);

	~CEnemyEntity();
};

class CProjectileEntity : public IEntity
{
public:
	CProjectileEntity(int nId = -1);

	void OnTriggerEnter(IEntity* pcCollidingEntity);

	~CProjectileEntity();
};

class CCameraEntity : public IEntity
{
public:
	CCameraEntity(int nId = -1);

	void Update();

	~CCameraEntity();
};

class CEnvironmentEntity : public IEntity
{
public:
	CEnvironmentEntity(int nID = -1);

	~CEnvironmentEntity();
};

class CDoorEntity : public IEntity
{
public:
	CDoorEntity(int nID = -1);

	~CDoorEntity();
};

class CRoomEntity : public IEntity
{
	std::vector<CLightComponent*>	m_pLights;
	

public:
	int m_nEnemyCount;
	std::vector<CEnemyEntity*>		 m_pcEnemies;
	std::vector<CEnvironmentEntity*> m_pcWalls;
	std::vector<CDoorEntity*> m_pcDoors;
	std::vector<CEnvironmentEntity*> m_pcPillars;

	std::vector<CRoomEntity*> m_pcNeighbors;

	bool Top;
	bool Bottom;
	bool Left;
	bool Right;

	bool Entities;
	bool Doors;

	CRoomEntity(int nId = -1);

	void SpawnEntities();

	void DeSpawnEntities();

	void SpawnDoors();

	void DeSpawnDoors();

	void TurnEnemiesOn();

	

	void AddLight(CLightComponent* pNewLight);

	void RemoveLight(CLightComponent* pLight);

	

	std::vector<CLightComponent*>	GetLights();

	~CRoomEntity();

};
