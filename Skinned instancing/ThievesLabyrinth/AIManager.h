#pragma once
#include "System.h"

#include "Math.h"

#include <vector>

class CComponentManager;
class IEnemyController;

class CAIManager : public ISystem
{
	CComponentManager* m_pcComponentManager;
	std::vector<IEnemyController*> m_pcEnemyControllers;
	float m_fSeparationStrength;

	CMath::TVECTOR3 GetSeparationAccel(IEnemyController * pcEnemy);
public:
	CAIManager(CComponentManager* pcComponentManager);
	~CAIManager();

	void UpdateEnemyBehavior();
	void UpdateFlockingBehavior(float fDeltaTime);
};

