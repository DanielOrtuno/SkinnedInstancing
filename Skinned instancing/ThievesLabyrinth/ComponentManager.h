#pragma once

//#include "Stats.h"
//#include "MageController.h"
#include "Inventory.h"
#include "LightComponent.h"
#include <vector>
#include "ProjectileComponent.h"

class CFireDemonController;
class CCameraController;
class CTransform;
class CMeshRenderer;
class ICollider;
class CAnimatorComponent;
class IEnemyController;
class CRigidbody;
class CStats;

class CComponentManager
{

	CFireDemonController*				m_pcMinotaurController;
	std::vector<CTransform*>			m_pcTransforms;
	std::vector<ICollider*>				m_pcColliders;
	std::vector<CRigidbody*>			m_pcRigidbodies;
	std::vector<CMeshRenderer*>			m_pcMeshes;
	std::vector<CCameraController*>		m_pcCameras;
	std::vector<IEnemyController*>		m_pcEnemyControllers;
	std::vector<CStats*>				m_pcStats;
	std::vector<CAnimatorComponent*>	m_pcAnimators;
	std::vector<CProjectileComponent*>  m_pcProjectileComponents;
	std::vector<CInventory*>			m_pcInventories;
	std::vector<CLightComponent*>		m_pcLights;

	bool								m_bRenderTransforms;
	bool								m_bRenderColliders;

public:
	CComponentManager();
	
	IComponent* CreateComponent(IEntity* pcOwner, int nType);

	int DeleteComponent(int nType, IComponent* pcComponent);

	int GetComponentCountOfType(int nType);

	void UpdateControllers();

	std::vector<CTransform*>*			 GetTransforms();
	std::vector<ICollider*>*			 GetColliders();
	std::vector<CRigidbody*>*			 GetRigidbodies();
	std::vector<CMeshRenderer*>			 GetMeshes();
	std::vector<CCameraController*>		 GetCameraController();
	std::vector<IEnemyController*>		 GetEnemyControllers();

	~CComponentManager();
};

// Entity Manager
	// Component Manager
		// Memory Manager


