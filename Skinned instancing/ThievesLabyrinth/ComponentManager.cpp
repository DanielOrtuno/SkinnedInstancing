#include "ComponentManager.h"
#include "InputManager.h"

#include "FireDemonController.h"
#include "CameraController.h"
#include "MageController.h"

#include "Rigidbody.h"
#include "AnimatorComponent.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "CapsuleCollider.h"
#include "EnumTypes.h"

#include "Stats.h"
#include "Entity.h"

CComponentManager::CComponentManager()
{
	m_bRenderColliders = false;
	m_bRenderTransforms = false;
	m_pcMinotaurController = nullptr;
}

IComponent* CComponentManager::CreateComponent(IEntity* pcOwner, int nType)
{ 
	IComponent* newComponent;

	switch(nType)
	{
		case eComponent::TRANSFORM:
		{
			CTransform* newTransform = new CTransform(pcOwner);
			m_pcTransforms.push_back(newTransform);
			newComponent = newTransform;

			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			CBoxCollider* newCollider = new CBoxCollider(pcOwner);
			m_pcColliders.push_back((ICollider*)newCollider);
			newComponent = newCollider;

			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			CCapsuleCollider* newCollider = new CCapsuleCollider(pcOwner);
			m_pcColliders.push_back((ICollider*)newCollider);
			newComponent = newCollider;

			break;
		}
		case eComponent::RIGIDBODY:
		{
			CRigidbody* newRigidbody = new CRigidbody(pcOwner);
			m_pcRigidbodies.push_back(newRigidbody);
			newComponent = newRigidbody;

			break;
		}
		case eComponent::MESH_RENDERER:
		{
			CMeshRenderer* newMesh = new CMeshRenderer(pcOwner);
			m_pcMeshes.push_back(newMesh);
			newComponent = newMesh;

			break;
		}

		case eComponent::STATS:
		{
			CStats* newStats = new CStats(pcOwner);
			m_pcStats.push_back(newStats);
			newComponent = newStats;

			break;
		}

		case eComponent::FIRE_DEMON_CONTROLLER:
		{
			m_pcMinotaurController = new CFireDemonController(pcOwner);
			newComponent = m_pcMinotaurController;
			break;
		}

		case eComponent::CAMERA_CONTROLLER:
		{
			CCameraController* newCamController = new CCameraController(pcOwner);
			m_pcCameras.push_back(newCamController);
			newComponent = newCamController;

			break;
		}

		case eComponent::ANIMATOR:
		{
			CAnimatorComponent* newAnimator = new CAnimatorComponent(pcOwner);
			m_pcAnimators.push_back(newAnimator);
			newComponent = newAnimator;

			break;
		}

		case eComponent::MAGE_CONTROLLER:
		{
			CMageController* newMageController = new CMageController(pcOwner);
			newComponent = newMageController;
			m_pcEnemyControllers.push_back(dynamic_cast<IEnemyController*>(newMageController));
			break;
		}

		case eComponent::PROJECTILE_COMPONENT:
		{
			CProjectileComponent* newProjectileComponent = new CProjectileComponent(pcOwner);
			m_pcProjectileComponents.push_back(newProjectileComponent);

			newComponent = newProjectileComponent;
			break;
		}

		case eComponent::INVENTORY:
		{
			CInventory* newInventoryComponent = new CInventory(pcOwner);
			m_pcInventories.push_back(newInventoryComponent);
			newComponent = newInventoryComponent;
			break;
		}

		case eComponent::LIGHT:
		{
			CLightComponent* newLight = new CLightComponent(pcOwner);
			m_pcLights.push_back(newLight);
			newComponent = newLight;

			if(pcOwner->m_pcRoom)
			{
				pcOwner->m_pcRoom->AddLight(newLight);
			}

			break;
		}

		default:
		{
			newComponent = nullptr;
			break;
		}
	}

	newComponent->m_pcOwner = pcOwner;
	newComponent->m_nComponentType = nType;
	newComponent->SetActiveState(pcOwner->IsActive());

	return newComponent;
}

int CComponentManager::DeleteComponent(int nType, IComponent * pcComponent)
{
	switch(nType)
	{
		case eComponent::TRANSFORM:
		{
			for(unsigned int i = 0; i < m_pcTransforms.size(); i++)
			{
				if(m_pcTransforms[i] == pcComponent)
				{
					m_pcTransforms.erase(m_pcTransforms.begin() + i);
					break;
				}
			}

			delete (CTransform*)pcComponent;
			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			for(unsigned int i = 0; i < m_pcColliders.size(); i++)
			{
				if(m_pcColliders[i] == pcComponent)
				{
					m_pcColliders.erase(m_pcColliders.begin() + i);
					break;
				}
			}

			delete (CBoxCollider*)pcComponent;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			for (unsigned int i = 0; i < m_pcColliders.size(); i++)
			{
				if (m_pcColliders[i] == pcComponent)
				{
					m_pcColliders.erase(m_pcColliders.begin() + i);
					break;
				}
			}

			delete (CCapsuleCollider*)pcComponent;
			break;
		}
		case eComponent::RIGIDBODY:
		{
			for(unsigned int i = 0; i < m_pcRigidbodies.size(); i++)
			{
				if(m_pcRigidbodies[i] == pcComponent)
				{
					m_pcRigidbodies.erase(m_pcRigidbodies.begin() + i);
					break;
				}
			}

			delete (CRigidbody*)pcComponent;
			break;
		}
		case eComponent::MESH_RENDERER:
		{
			for(unsigned int i = 0; i < m_pcMeshes.size(); i++)
			{
				if(m_pcMeshes[i] == pcComponent)
				{
					m_pcMeshes.erase(m_pcMeshes.begin() + i);
					break;
				}
			}

			delete (CMeshRenderer*)pcComponent;

			break;
		}
		case eComponent::FIRE_DEMON_CONTROLLER:
		{
			delete m_pcMinotaurController;
			m_pcMinotaurController = nullptr;
			break;
		}

		case eComponent::CAMERA_CONTROLLER:
		{
			for(unsigned int i = 0; i < m_pcCameras.size(); i++)
			{
				if(m_pcCameras[i] == pcComponent)
				{
					m_pcCameras.erase(m_pcCameras.begin() + i);
					break;
				}
			}

			delete (CCameraController*)pcComponent;

			break;
		}

		case eComponent::STATS:
		{
			for(unsigned int i = 0; i < m_pcStats.size(); i++)
			{
				if(m_pcStats[i] == pcComponent)
				{
					m_pcStats.erase(m_pcStats.begin() + i);
					break;
				}
			}

			delete (CStats*)pcComponent;

			break;
		}

		case eComponent::ANIMATOR:
		{
			for(unsigned int i = 0; i < m_pcAnimators.size(); i++)
			{
				if(m_pcAnimators[i] == pcComponent)
				{
					m_pcAnimators.erase(m_pcAnimators.begin() + i);
					break;
				}
			}

			delete (CAnimatorComponent*)pcComponent;

			break;
		}

		case eComponent::MAGE_CONTROLLER:
		{
			for (unsigned int i = 0; i < m_pcEnemyControllers.size(); i++)
			{
				if (m_pcEnemyControllers[i] == pcComponent)
				{
					m_pcEnemyControllers.erase(m_pcEnemyControllers.begin() + i);
					break;
				}
			}

			delete (CMageController*)pcComponent;

			break;
		}

		case eComponent::PROJECTILE_COMPONENT:
		{
			for(unsigned int i = 0; i < m_pcProjectileComponents.size(); i++)
			{
				if(m_pcProjectileComponents[i] == pcComponent)
				{
					m_pcProjectileComponents.erase(m_pcProjectileComponents.begin() + i);
					break;
				}
			}

			delete (CProjectileComponent*)pcComponent;
			break;
		}

		case eComponent::INVENTORY:
		{
			for (unsigned int i = 0; i < m_pcInventories.size(); i++)
			{
				if (m_pcInventories[i] == pcComponent)
				{
					m_pcInventories.erase(m_pcInventories.begin() + i);
					break;
				}
			}

			delete (CInventory*)pcComponent;

			break;
		}

		case eComponent::LIGHT:
		{
			for(unsigned int i = 0; i < m_pcLights.size(); i++)
			{
				if(m_pcLights[i] == pcComponent)
				{
					m_pcLights.erase(m_pcLights.begin() + i);
					break;
				}
			}

			if(pcComponent->m_pcOwner->m_pcRoom)
			{
				pcComponent->m_pcOwner->m_pcRoom->RemoveLight((CLightComponent*)pcComponent);
			}

			delete (CLightComponent*)pcComponent;

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

int CComponentManager::GetComponentCountOfType(int nType)
{
	switch(nType)
	{
		case eComponent::TRANSFORM:
		{
			return (int)m_pcTransforms.size();
			break;
		}

		case eComponent::BOX_COLLIDER:
		{
			int nResult = 0;
			for (ICollider* collider : m_pcColliders)
			{
				if (collider->m_nComponentType == eComponent::BOX_COLLIDER)
					nResult++;
			}
			return nResult;
			break;
		}

		case eComponent::CAPSULE_COLLIDER:
		{
			int nResult = 0;
			for (ICollider* collider : m_pcColliders)
			{
				if (collider->m_nComponentType == eComponent::CAPSULE_COLLIDER)
					nResult++;
			}
			return nResult;
			break;
		}

		case eComponent::RIGIDBODY:
		{
			return (int)m_pcRigidbodies.size();
			break;
		}
		case eComponent::MESH_RENDERER:
		{
			return (int)m_pcMeshes.size();
			break;
		}
		case eComponent::FIRE_DEMON_CONTROLLER:
		{
			if(m_pcMinotaurController != nullptr)
			{
				return 1;
			}
			else
			{
				return 0;
			}
			break;
		}

		case eComponent::CAMERA_CONTROLLER:
		{
			return (int)m_pcCameras.size();
			break;
		}

		case eComponent::PROJECTILE_COMPONENT:
		{
			return (int)m_pcProjectileComponents.size();
			break;
		}

		case eComponent::INVENTORY:
		{
			return (int)m_pcInventories.size();
			break;
		}

		case eComponent::LIGHT:
		{
			return (int)m_pcLights.size();
			break;
		}

		default:
		{
			return -1;
			break;
		}
	}
}

void CComponentManager::UpdateControllers()
{
	if(m_pcMinotaurController)
	{
		if(m_pcMinotaurController->IsActive())
		{
			m_pcMinotaurController->Update();
		}
	}

	for ( CCameraController* pcController : m_pcCameras)
	{
		if(pcController->IsActive())
		{
			pcController->Update();
		}
	}

	for(CProjectileComponent* pcProjectile : m_pcProjectileComponents)
	{
		if(pcProjectile->IsActive())
		{
			pcProjectile->Update();
		}
	}

	if(CInputManager::GetKeyPress('C'))
	{
		m_bRenderColliders = !m_bRenderColliders;
	}

	if(CInputManager::GetKeyPress('T'))
	{
		m_bRenderTransforms = !m_bRenderTransforms;
	}

	if(m_bRenderTransforms)
	{
		for(CTransform* pcTransform : m_pcTransforms)
		{
			if(pcTransform->IsActive())
			{
				pcTransform->RenderTransform();
			}
		}
	}

	if(m_bRenderColliders)
	{
		for(ICollider* pcCollider : m_pcColliders)
		{
			if(pcCollider->IsActive())
			{
				pcCollider->RenderCollider();
			}
		}
	}
}

std::vector<CTransform*>* CComponentManager::GetTransforms()
{
	return &m_pcTransforms;
}

std::vector<ICollider*>* CComponentManager::GetColliders()
{
	return &m_pcColliders;
}

std::vector<CRigidbody*>* CComponentManager::GetRigidbodies()
{
	return &m_pcRigidbodies;
}

std::vector<CMeshRenderer*> CComponentManager::GetMeshes()
{
	return m_pcMeshes;
}

std::vector<IEnemyController*> CComponentManager::GetEnemyControllers()
{
	return m_pcEnemyControllers;
}

std::vector<CCameraController*> CComponentManager::GetCameraController()
{
	return m_pcCameras;
}

CComponentManager::~CComponentManager()
{
	if(m_pcMinotaurController != nullptr)
	{
		DeleteComponent(m_pcMinotaurController->m_nComponentType, m_pcMinotaurController);
	}

	for(CTransform* pcComponent : m_pcTransforms)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(ICollider* pcComponent : m_pcColliders)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CRigidbody* pcComponent : m_pcRigidbodies)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CMeshRenderer* pcComponent : m_pcMeshes)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CCameraController* pcComponent : m_pcCameras)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for(CProjectileComponent* pcComponent : m_pcProjectileComponents)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CInventory* pcComponent : m_pcInventories)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CAnimatorComponent* pcComponent : m_pcAnimators)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (IEnemyController* pcComponent : m_pcEnemyControllers)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}

	for (CStats* pcComponent : m_pcStats)
	{
		DeleteComponent(pcComponent->m_nComponentType, pcComponent);
	}
}
