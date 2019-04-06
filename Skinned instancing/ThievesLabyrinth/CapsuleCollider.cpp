#include "CapsuleCollider.h"
#include "EnumTypes.h"

CCapsuleCollider::CCapsuleCollider(IEntity* pcOwner) : ICollider(pcOwner)
{
	m_pcOwner = pcOwner;
	m_nComponentType = eComponent::CAPSULE_COLLIDER;
}

CCapsuleCollider::~CCapsuleCollider()
{
}

CMath::TVECTOR3 CCapsuleCollider::GetCenter()
{
	return m_tCenter;
}

float CCapsuleCollider::GetHeight()
{
	return m_fHeight;
}

float CCapsuleCollider::GetRadius()
{
	return m_fRadius;
}

void CCapsuleCollider::SetCenter(CMath::TVECTOR3 tCenter)
{
	m_tCenter = tCenter;
}

void CCapsuleCollider::SetHeight(float fHeight)
{
	m_fHeight = fHeight;
}

void CCapsuleCollider::SetRadius(float fRadius)
{
	m_fRadius = fRadius;
}

void CCapsuleCollider::RenderCollider()
{

}
