#pragma once
#include "Component.h"

class CProjectileComponent : public IComponent
{
	float m_fTimeLeft;
	float m_fDamage;
	int   m_nCasterEntityType;

public:
	CProjectileComponent(IEntity* pcOwner);
	
	void SetLifeTime(float fTime);
	void SetDamage(float fDamage);
	void SetCasterEntity(int nType);

	float GetTimeLeft();
	float GetDamage();
	int	  GetCasterEntity();

	void Update();

	CProjectileComponent& operator=(CProjectileComponent& pCComponentToCopy);
	
	~CProjectileComponent();
};

