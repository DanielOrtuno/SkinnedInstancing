#include "AnimatorComponent.h"
#include "EnumTypes.h"


CAnimatorComponent::CAnimatorComponent(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::ANIMATOR;
	m_nCurrentAnimation = 0;
	m_dCurrentTime = 0.0;
	m_fSpeed = 1.0f;
}

int CAnimatorComponent::GetAnimator()
{
	return m_nAnimator;
}

int CAnimatorComponent::GetCurrentAnimation()
{
	return m_nCurrentAnimation;
}

double CAnimatorComponent::GetCurrentAnimationTime()
{
	return m_dCurrentTime;
}

float CAnimatorComponent::GetSpeed()
{
	return m_fSpeed;
}

void CAnimatorComponent::SetAnimator(int nAnimator)
{
	m_nAnimator = nAnimator;
}

void CAnimatorComponent::SetAnimation(int nAnimation)
{
	m_nCurrentAnimation = nAnimation;
}

void CAnimatorComponent::SetAnimationTime(double dTime)
{
	m_dCurrentTime = dTime;
}

void CAnimatorComponent::SetSpeed(float fSpeed)
{
	m_fSpeed = fSpeed;
}

CAnimatorComponent & CAnimatorComponent::operator=(CAnimatorComponent & cCopy)
{
	m_nAnimator = cCopy.GetAnimator();
	m_nCurrentAnimation = cCopy.GetCurrentAnimation();
	m_dCurrentTime = cCopy.GetCurrentAnimationTime();
	m_fSpeed = cCopy.m_fSpeed;

	return *this;
}

CAnimatorComponent::~CAnimatorComponent()
{
}
