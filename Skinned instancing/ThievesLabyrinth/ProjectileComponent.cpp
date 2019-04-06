#include "ProjectileComponent.h"
#include "EntityManager.h"
#include "Time.h"
#include "EnumTypes.h"

CProjectileComponent::CProjectileComponent(IEntity* pcOwner) : IComponent(pcOwner)
{  
	m_nComponentType = eComponent::PROJECTILE_COMPONENT;
	m_fDamage = 0.0f;
	m_fTimeLeft = 10.0f;
}

void CProjectileComponent::SetLifeTime(float fTime)
{
	m_fTimeLeft = fTime;
}

float CProjectileComponent::GetTimeLeft()
{
	return m_fTimeLeft;
}

void CProjectileComponent::SetDamage(float fDamage)
{
	m_fDamage = fDamage;
}

void CProjectileComponent::SetCasterEntity(int nType)
{
	m_nCasterEntityType = nType;
}

float CProjectileComponent::GetDamage()
{
	return m_fDamage;
}

int CProjectileComponent::GetCasterEntity()
{
	return m_nCasterEntityType;
}

void CProjectileComponent::Update()
{
	m_fTimeLeft -= (float)CTime::GetDelta();

	if(m_fTimeLeft <= 0)
	{
		CEntityManager::AddEntityToDeletionQueue(m_pcOwner);
	}
}

CProjectileComponent& CProjectileComponent::operator=(CProjectileComponent & pCComponentToCopy)
{
	SetLifeTime(pCComponentToCopy.GetTimeLeft());
	SetDamage(pCComponentToCopy.GetDamage());
	SetCasterEntity(pCComponentToCopy.GetCasterEntity());

	return *this;
}

CProjectileComponent::~CProjectileComponent()
{
}
