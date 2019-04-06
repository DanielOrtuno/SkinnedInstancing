#include "AIManager.h"
#include "Time.h"
#include "Rigidbody.h"
#include "ComponentManager.h"
#include "MageController.h"
#include "Transform.h"

#include "EnumTypes.h"
#include "Stats.h"
#include "Entity.h"

CMath::TVECTOR3 CAIManager::GetSeparationAccel(IEnemyController * pcEnemy)
{
	CMath::TVECTOR3 tSum = CMath::TVECTOR3(0, 0, 0);
	CTransform* pcTransform = pcEnemy->m_pcOwner->GetComponent<CTransform>();
	CRigidbody* pcRigidBody = pcEnemy->m_pcOwner->GetComponent<CRigidbody>();

	for (IEnemyController* controller : m_pcEnemyControllers)
	{
		if (controller == pcEnemy)
			continue;

		CTransform* pcOtherTransform = controller->m_pcOwner->GetComponent<CTransform>();
		CRigidbody* pcOtherRigidbody = controller->m_pcOwner->GetComponent<CRigidbody>();

		CMath::TVECTOR3 tDistanceVector = pcTransform->GetPosition() - pcOtherTransform->GetPosition();

		float fDistance = CMath::Vector3Magnitude(tDistanceVector);

		float fSafeDistance = pcEnemy->m_fSafeDistance + controller->m_fSafeDistance;

		float fSpeed = CMath::Vector3Magnitude(pcRigidBody->GetVelocity());
		float fOtherSpeed = CMath::Vector3Magnitude(pcRigidBody->GetVelocity());

		float fSumSpeed = fSpeed + fOtherSpeed;
		if (fSumSpeed == 0)
			return CMath::TVECTOR3(0, 0, 0);

		if (fDistance < fSafeDistance)
		{
			CMath::TVECTOR3 tDirectionVector = CMath::Vector3Normalize(tDistanceVector);
			tDirectionVector = tDirectionVector * (fSafeDistance - fDistance) / fSafeDistance;
			tDirectionVector = tDirectionVector * (fSumSpeed - fSpeed) / fSumSpeed;
			tSum = tSum + tDirectionVector;
		}
	}
	if (CMath::Vector3Magnitude(tSum) > 1.0f)
		tSum = CMath::Vector3Normalize(tSum);

	return tSum * m_fSeparationStrength;
}

CAIManager::CAIManager(CComponentManager* pcComponentManager)
{
	m_pcComponentManager = pcComponentManager;
	m_fSeparationStrength = 500.0f;
}


CAIManager::~CAIManager()
{
}

void CAIManager::UpdateEnemyBehavior()
{
	m_pcEnemyControllers = m_pcComponentManager->GetEnemyControllers();

	for (IEnemyController* controller : m_pcEnemyControllers)
	{
		if (controller->IsActive())
		{
			int nType = controller->m_nEnemyType;

			switch (nType)
			{
				case eEnemyType::MAGE:
				{
					CMageController* pcMageController = dynamic_cast<CMageController*>(controller);
					if (pcMageController)
					{
						pcMageController->Update();
					}
					break;
				}

				default:
				{
					break;
				}
			}
		}

	}
}


void CAIManager::UpdateFlockingBehavior(float fDeltaTime)
{
	for (IEnemyController* controller : m_pcEnemyControllers)
	{
		CMath::TVECTOR3 acceleration = GetSeparationAccel(controller);
		acceleration = acceleration * controller->m_pcOwner->GetComponent<CStats>()->GetMovementSpeed() * fDeltaTime;
		
		CRigidbody* tRigidbody = controller->m_pcOwner->GetComponent<CRigidbody>();
		CTransform* tTransform = controller->m_pcOwner->GetComponent<CTransform>();

		CMath::TVECTOR3 tVelocity = tRigidbody->GetVelocity();
		tVelocity = tVelocity + acceleration;

		CMath::TVECTOR3 tNormalVelocity = CMath::Vector3Normalize(tVelocity);
		CMath::TVECTOR3 tFinalVelocity = tNormalVelocity * controller->m_pcOwner->GetComponent<CStats>()->GetMovementSpeed();
		tRigidbody->SetVelocity(tVelocity);
		tTransform->TurnTo(tNormalVelocity);
	}
}
