#pragma once
#include "Component.h"

class IEntity;

class IEnemyController : public IComponent
{
public:
	IEnemyController(IEntity* pcOwner);

	virtual void Update() = 0;

	int m_nEnemyType;

	float m_fSafeDistance;
};