#pragma once
//#include "Transform.h"
//#include "Rigidbody.h"
//#include "AnimatorComponent.h"
//#include "FireDemonController.h"
//#include "Entity.h"
//#include "Component.h"
#include "EnemyController.h"

class CStats;
class CAnimatorComponent;
class IEntity;
class CProjectileEntity;
class CRigidbody;
class CTransform;

class CMageController : public IEnemyController
{
private:
	CTransform*				m_pcTransform;
	CTransform*				m_pcTarget;
	CRigidbody*				m_pcRigidbody;
	CAnimatorComponent*		m_pcAnimator;
	CStats*					m_pcStats;

	IEntity*				m_pcOwner;

	CProjectileEntity*		m_pcProjectileRef;

	float					m_fLastAttackTime;

	bool					m_bIsAttacking;
public:
	CMageController(IEntity* pcOwner);
	~CMageController();
	
	void GetPlayerReference();

	void Update();

	void Attack();

	CMageController& operator=(CMageController& cCopy);

	void SetTarget(CTransform* pcTarget);
};
