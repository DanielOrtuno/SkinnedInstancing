#pragma once
#include "Component.h"

class CAnimatorComponent : public IComponent
{
	int		m_nAnimator;
	int		m_nCurrentAnimation;
	double	m_dCurrentTime;
	float	m_fSpeed;
public:
	CAnimatorComponent(IEntity* pcOwner);

	int GetAnimator();
	int GetCurrentAnimation();
	double GetCurrentAnimationTime();
	float GetSpeed();

	void SetAnimator(int nAnimator);
	void SetAnimation(int nAnimation);
	void SetAnimationTime(double dTime);
	void SetSpeed(float fSpeed);

	CAnimatorComponent& operator=(CAnimatorComponent& cCopy);

	~CAnimatorComponent();
};

