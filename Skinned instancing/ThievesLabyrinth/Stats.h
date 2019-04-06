#pragma once
#include "Component.h"

class CStats : public IComponent
{
	float		m_fHP;
	float		m_fMP;
	float		m_fMovementSpeed;
	float		m_fBaseDamage;
	float		m_fAttackSpeed;

public:
	CStats(IEntity* pcOwner);

	CStats(IEntity* pcOwner, float nHP, float nMP, float fMovementSpeed, float nBaseDamage, float m_fAttackSpeed);

	/***************
	* Accessors
	***************/

	float GetHP();
	float GetMP();
	float GetMovementSpeed();
	float GetBaseDamage();
	float GetAttackSpeed();

	/***************;
	* Mutators
	***************/

	void SetHP(float fHP);
	void SetMP(float fMP);
	void SetMovementSpeed(float fMovementSpeed);
	void SetBaseDamage(float fDamage);
	void SetAttackSpeed(float fAttackSpeed);

	CStats& operator=(CStats& cCopy);

	~CStats();
};

