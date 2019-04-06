#pragma once
#include "Component.h"
#include <vector>
//#include "Math.h"

class CTransform;
class CProjectileEntity;
class CRigidbody;
class CAnimatorComponent;
class CStats;
class CInventory;

class CFireDemonController : public IComponent
{
	CRigidbody*				m_pcRigidbody;
	CTransform*				m_pcTransform;
	CTransform*				m_pEnemyTransform;
	CAnimatorComponent*		m_pcAnimator;
	CStats*					m_pcStats;
	CInventory*				m_pcInventory;

	float					m_fLastAttackTime;
	float					m_fMeleeAttacktime;
	float					m_fAttackAnimTime;

	CProjectileEntity*		m_pcProjectileRef;
	CProjectileEntity*		m_pcMeleeRef;
	CProjectileEntity*		m_pcAOERef;

	bool					m_bIsAttacking;
	bool					m_bMoving;

	CMath::TVECTOR3			m_tClickPos;
public:
	CFireDemonController(IEntity* pcOwner);

	void Update();

	void Movement();

	void Melee();

	void Attack();

	void AOE();

	void Init();

	float GetAttackTime();

	~CFireDemonController();
};

