#include "FireDemonController.h"
#include "AnimatorComponent.h"

#include "PhysicsManager.h"
#include "InputManager.h"
#include "EventManager.h"

#include "Entity.h"

#include "Time.h"
#include "Transform.h"
#include "Rigidbody.h"
#include "Stats.h"
#include "MeshRenderer.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"

#include <iostream>


CFireDemonController::CFireDemonController(IEntity* pcOwner) : IComponent(pcOwner)
{
	Init();

	m_nComponentType = eComponent::FIRE_DEMON_CONTROLLER;
	m_fLastAttackTime = 0.0f;
	m_fMeleeAttacktime = 0.0f;
	m_fAttackAnimTime = 0.0f;

	m_pEnemyTransform = nullptr;
	m_pcRigidbody = this->m_pcOwner->GetComponent<CRigidbody>();
	m_pcTransform = this->m_pcOwner->GetComponent<CTransform>();
	m_pcStats = this->m_pcOwner->GetComponent<CStats>();
	m_pcAnimator = this->m_pcOwner->GetComponent<CAnimatorComponent>();
	m_pcInventory = this->m_pcOwner->GetComponent<CInventory>();

	m_bIsAttacking = false;
	m_bMoving = false;
}

void CFireDemonController::Update()
{
	if (!m_pcRigidbody)
	{
		// TODO: Add debug error message
		return;
	}
	float fDelta = (float)CTime::GetDelta();
	if (m_fLastAttackTime > 0)
	{
		m_fLastAttackTime -= fDelta;
	}
	if (m_fMeleeAttacktime > 0)
	{
		m_fMeleeAttacktime -= fDelta;
	}
	if (m_fAttackAnimTime > 0)
	{
		m_fAttackAnimTime -= fDelta;
	}
	if (m_fAttackAnimTime < .10f && m_bIsAttacking)
	{
		m_bIsAttacking = false;
		Melee();
	}

	Movement();


	if (CInputManager::GetKeyPress('1'))
	{
		if (m_fLastAttackTime <= 0)
		{
			if (m_pcStats->GetMP() > 0)
			{
				CMath::TVECTOR2 vMouse;
				CMath::TVECTOR3 vStart, vEnd, vSegment, tVel, tNormalVelocity, tFloorPosition;
				CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
				CInputManager::GetMouseWorldSpace(vMouse, vStart, vEnd);
				vSegment = vEnd - vStart;

				if (CPhysicsManager::RaycastMouseToFloor(vStart, vSegment, OUT tFloorPosition))
				{
					//std::cout << "Raycast to floor returned: (" << tFloorPosition.x << ", " << tFloorPosition.z << ")" << std::endl;
					tVel = tFloorPosition - m_pcTransform->GetPosition();
				}

				tNormalVelocity = CMath::Vector3Normalize(tVel);
				m_pcTransform->LookAt(tNormalVelocity);

				Attack();
			}
			else
			{
				TDebugMessage m_cMessage(std::string("You ran out of the blue thing"));
				CEventManager::SendDebugMessage(m_cMessage);
			}
		}
	}

	if (CInputManager::GetKeyPress('2'))
	{
		if (m_fLastAttackTime <= 0)
		{
			if (m_pcStats->GetMP() > 0)
			{
				AOE();
			}
			else
			{
				TDebugMessage m_cMessage(std::string("You ran out of the blue thing"));
				CEventManager::SendDebugMessage(m_cMessage);
			}
		}
	}

	if (CInputManager::GetKeyDown(VK_SPACE))
	{
		if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::MELEE && m_fMeleeAttacktime <= 0)
		{
			m_pcAnimator->SetAnimation(ePlayerAnimation::MELEE);
			m_pcAnimator->SetAnimationTime(0.0);
			m_pcAnimator->SetSpeed(2.0f);
			m_fAttackAnimTime = 0.6f;

			m_bIsAttacking = true;
		}
	}

	if (CInputManager::GetKeyPress('M'))
	{
		m_pcStats->SetMP(3);
		TDebugMessage m_cMessage(std::string("You drank some Redbull. +3 blue thingy"));
		CEventManager::SendDebugMessage(m_cMessage);
	}
}

void CFireDemonController::Movement()
{
	CMath::TVECTOR3 tVel, tNormalVelocity;

	if (m_fAttackAnimTime <= 0)
	{
		if (CInputManager::GetKeyDown('W'))
		{
			tVel.z = 0.5f;
			tVel.x = 0.5f;
			m_pEnemyTransform = nullptr;
		}
		else if (CInputManager::GetKeyDown('S'))
		{
			tVel.z = -0.5f;
			tVel.x = -0.5f;
			m_pEnemyTransform = nullptr;
		}
		if (CInputManager::GetKeyDown('D'))
		{
			tVel.x += 0.5f;
			tVel.z += -0.5f;
			m_pEnemyTransform = nullptr;
		}
		else if (CInputManager::GetKeyDown('A'))
		{
			tVel.x += -0.5f;
			tVel.z += 0.5f;
			m_pEnemyTransform = nullptr;
		}

		/*
		if (CInputManager::GetKeyDown(VK_LBUTTON))
		{
			CMath::TVECTOR2 vMouse;
			CMath::TVECTOR3 vStart, vEnd;
			CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
			tVel.x = vMouse.x;
			tVel.z = vMouse.y;
		}
		*/

		if (CInputManager::GetKeyDown(VK_LBUTTON))
		{
			CMath::TVECTOR2 vMouse;
			CMath::TVECTOR3 vStart, vEnd, vSegment, tFloorPosition;
			CInputManager::GetMousePosNDC(vMouse.x, vMouse.y);
			CInputManager::GetMouseWorldSpace(vMouse, vStart, vEnd);
			vSegment = vEnd - vStart;

			if (CPhysicsManager::RaycastMouseToFloor(vStart, vSegment, OUT tFloorPosition))
			{
				//std::cout << "Raycast to floor returned: (" << tFloorPosition.x << ", " << tFloorPosition.z << ")" << std::endl;
				tVel = tFloorPosition - m_pcTransform->GetPosition();
			}

			if (CInputManager::GetKeyPress(VK_LBUTTON))
			{


				TRayHit hit;
				if (CPhysicsManager::Raycast(vStart, vSegment, OUT hit, CMath::Vector3Magnitude(vSegment), CEntityMask(eEntity::PLAYER)))
				{
					// TODO: Do stuff to the enemy
					if (hit.tEntity->m_nEntityType == eEntity::ENEMY)
					{
						m_pEnemyTransform = hit.tEntity->GetComponent<CTransform>();
					}
				}


			}
		}

		if (m_pEnemyTransform)
		{
			float fDis = fabsf(CMath::Vector3Magnitude(m_pcTransform->GetPosition() - m_pEnemyTransform->GetPosition()));
			//CEventManager::SendDebugMessage(TDebugMessage(std::to_string(fDis)));
			if (fDis > m_pcOwner->GetComponent<CCapsuleCollider>()->GetRadius() + 1.2f)
			{
				tVel = m_pEnemyTransform->GetPosition() - m_pcTransform->GetPosition();
			}
			else
			{
				if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::MELEE && m_fMeleeAttacktime <= 0)
				{
					m_pcAnimator->SetAnimation(ePlayerAnimation::MELEE);
					m_pcAnimator->SetAnimationTime(0.0);
					m_pcAnimator->SetSpeed(2.0f);
					m_fAttackAnimTime = 0.6f;

					m_bIsAttacking = true;
				}
				m_pEnemyTransform = nullptr;
			}
		}
	}

	tNormalVelocity = CMath::Vector3Normalize(tVel);
	m_pcRigidbody->SetVelocity(tNormalVelocity * m_pcStats->GetMovementSpeed());

	if (CMath::Vector3Magnitude(tNormalVelocity) > 0)
	{
		m_pcTransform->TurnTo(tNormalVelocity, 0.25f);
		CEventManager::SendAudioMessage(TAudioMessage::TAudioMessage(true, eAudio::SFX, eSFX::PLAYERMOVE));
		m_bMoving = false;

		if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::WALK && !m_bIsAttacking)
		{
			m_pcAnimator->SetAnimation(ePlayerAnimation::WALK);
			m_pcAnimator->SetAnimationTime(0.0);
			m_pcAnimator->SetSpeed(1.8f);
		}
	}
	else
	{
		if (!m_bMoving)
		{
			CEventManager::SendAudioMessage(TAudioMessage::TAudioMessage(false, eAudio::SFX, eSFX::PLAYERMOVE));
			m_bMoving = true;
		}

		if (m_pcAnimator->GetCurrentAnimation() != ePlayerAnimation::IDLE && m_fAttackAnimTime <= 0)
		{
			m_pcAnimator->SetAnimation(ePlayerAnimation::IDLE);
			m_pcAnimator->SetAnimationTime(0.0);
			m_pcAnimator->SetSpeed(1.0f);
		}
	}

	//Updating light data
	//((CPlayerEntity*)m_pcOwner)->GetLight()->GetComponent<CTransform>()->SetPosition(m_pcTransform->GetPosition() + CMath::TVECTOR3(0, 2.5f, 0));
}

void CFireDemonController::Melee()
{
	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcMeleeRef);
	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage()); //Modify damage based on items/stats/buffs/debuffs
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward(pcOwnerTransform->GetMatrix().r[2].x, pcOwnerTransform->GetMatrix().r[2].y, pcOwnerTransform->GetMatrix().r[2].z);

	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition() + CMath::Vector3Normalize(tForward) * 1.5f);
	pcProjTransform->SetRotation(pcOwnerTransform->GetRotation());

	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity(CMath::TVECTOR3(tForward) * 10);

	pcNewProjectile->SetActiveState(true);
	m_fMeleeAttacktime = 0.5f;
}

void CFireDemonController::Attack()
{
	int nRand = rand() % (eSFX::FIREBALL2 - eSFX::FIREBALL1 + 1) + eSFX::FIREBALL1;
	CEventManager::SendAudioMessage(TAudioMessage(true, eAudio::SFX, nRand));
	//CAudioManager::ReceiveSoundRequest(true, eAudio::SFX, nRand);

	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcProjectileRef);
	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage()); //Modify damage based on items/stats/buffs/debuffs
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward(pcOwnerTransform->GetMatrix().r[2].x, pcOwnerTransform->GetMatrix().r[2].y, pcOwnerTransform->GetMatrix().r[2].z);

	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition() + CMath::Vector3Normalize(tForward) * 1.5f + CMath::TVECTOR3(0, 3, 0));
	pcProjTransform->SetRotation(pcOwnerTransform->GetRotation());

	pcNewProjectile->GetComponent<CRigidbody>()->SetVelocity(CMath::TVECTOR3(tForward) * 30);

	pcNewProjectile->SetActiveState(true);

	m_fLastAttackTime = m_pcStats->GetAttackSpeed();

	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CLightComponent* pcProjLight = pcNewProjectile->GetComponent<CLightComponent>();

	if(pcProjLight)
	{
		m_pcOwner->m_pcRoom->AddLight(pcProjLight);
	}

	m_pcStats->SetMP(m_pcStats->GetMP() - 1);
}

void CFireDemonController::AOE()
{
	CProjectileEntity* pcNewProjectile = (CProjectileEntity*)CEntityManager::CloneEntity(m_pcAOERef);
	pcNewProjectile->GetComponent<CProjectileComponent>()->SetDamage(m_pcStats->GetBaseDamage());
	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CTransform* pcProjTransform = pcNewProjectile->GetComponent<CTransform>();
	CTransform* pcOwnerTransform = m_pcOwner->GetComponent<CTransform>();
	CMath::TVECTOR3 tForward(pcOwnerTransform->GetMatrix().r[2].x, pcOwnerTransform->GetMatrix().r[2].y, pcOwnerTransform->GetMatrix().r[2].z);

	pcProjTransform->SetPosition(pcOwnerTransform->GetPosition());

	pcNewProjectile->SetActiveState(true);

	m_fLastAttackTime = m_pcStats->GetAttackSpeed();
	m_pcStats->SetMP(m_pcStats->GetMP() - 1);

	pcNewProjectile->m_pcRoom = m_pcOwner->m_pcRoom;

	CLightComponent* pcProjLight = pcNewProjectile->GetComponent<CLightComponent>();

	if(pcProjLight)
	{
		m_pcOwner->m_pcRoom->AddLight(pcProjLight);
	}
}

void CFireDemonController::Init()
{

#pragma region Projectile

	m_pcProjectileRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::TRANSFORM);

	CBoxCollider* pcProjCollider = (CBoxCollider*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::BOX_COLLIDER);
	pcProjCollider->SetExtent({ 1,1,1 });
	pcProjCollider->SetTrigger(true);

	CLightComponent* pcProjLight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::LIGHT);

	CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::RIGIDBODY);

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::SPHERE);
	pcMesh->SetIndexBuffer(eIndexBuffer::SPHERE);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eTexture::FIRE_DEMON_DIFFUSE, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::CLAMP);

	CProjectileComponent* pcProjComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcProjectileRef, eComponent::PROJECTILE_COMPONENT);
	pcProjComponent->SetDamage(10.0f);
	pcProjComponent->SetLifeTime(1.0f);
	pcProjComponent->SetCasterEntity(eEntity::PLAYER);

	pcProjLight->SetColor({ 229 / 255.0f, 137 / 255.0f, 39 / 255.0f, 1 });
	pcProjLight->SetPosition({ 0, 0, 0 });
	pcProjLight->SetRadius(15.0f);

	m_pcProjectileRef->SetActiveState(false);
#pragma endregion

#pragma region Melee

	m_pcMeleeRef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CEntityManager::AddComponentToEntity(m_pcMeleeRef, eComponent::TRANSFORM);

	CBoxCollider* pcMeleeCollider = (CBoxCollider*)CEntityManager::AddComponentToEntity(m_pcMeleeRef, eComponent::BOX_COLLIDER);
	pcMeleeCollider->SetExtent({ 3,1,1 });
	pcMeleeCollider->SetTrigger(true);

	CEntityManager::AddComponentToEntity(m_pcMeleeRef, eComponent::RIGIDBODY);

	CProjectileComponent* pcMeleeComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcMeleeRef, eComponent::PROJECTILE_COMPONENT);
	pcMeleeComponent->SetDamage(10.0f);
	pcMeleeComponent->SetLifeTime(0.1f);
	pcMeleeComponent->SetCasterEntity(eEntity::PLAYER);
	m_pcMeleeRef->SetActiveState(false);

#pragma endregion


#pragma region AOE

	m_pcAOERef = (CProjectileEntity*)CEntityManager::CreateEntity(eEntity::PROJECTILE);
	CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::TRANSFORM);

	CCapsuleCollider* pcAOECollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::CAPSULE_COLLIDER);
	pcAOECollider->SetHeight(0.1f);
	pcAOECollider->SetRadius(8.0f);
	pcAOECollider->SetTrigger(true);

	CLightComponent* pcAOELight = (CLightComponent*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::LIGHT);

	CProjectileComponent* pcAOEComponent = (CProjectileComponent*)CEntityManager::AddComponentToEntity(m_pcAOERef, eComponent::PROJECTILE_COMPONENT);
	pcAOEComponent->SetDamage(1.0f);
	pcAOEComponent->SetLifeTime(3.0f);
	pcAOEComponent->SetCasterEntity(eEntity::PLAYER);

	pcAOELight->SetColor({1, 0, 0, 1});
	pcAOELight->SetPosition({ 0, 4, 0 });
	pcAOELight->SetRadius(25.0f);

	m_pcAOERef->SetActiveState(false);

#pragma endregion

}

float CFireDemonController::GetAttackTime()
{
	return m_fLastAttackTime;
}

CFireDemonController::~CFireDemonController()
{

}