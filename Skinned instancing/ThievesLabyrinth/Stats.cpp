#include "Stats.h"
#include "EnumTypes.h"



CStats::CStats(IEntity* pcOwner) : IComponent(pcOwner)
{
}

CStats::CStats(IEntity* pcOwner, float fHP, float fMP, float fMovementSpeed, float fBaseDamage, float fAttackSpeed) : IComponent(pcOwner)
{
	m_fHP = fHP;
	m_fMP = fMP;
	m_fMovementSpeed = fMovementSpeed;
	m_fBaseDamage = fBaseDamage;
	m_fAttackSpeed = fAttackSpeed;

	m_nComponentType = eComponent::STATS;
}

float CStats::GetHP()
{
	return m_fHP;
}

float CStats::GetMP()
{
	return m_fMP;
}

float CStats::GetMovementSpeed()
{
	return m_fMovementSpeed;
}

float CStats::GetBaseDamage()
{
	return m_fBaseDamage;
}

float CStats::GetAttackSpeed()
{
	return m_fAttackSpeed;
}

void CStats::SetHP(float nHP)
{
	m_fHP = nHP;
}

void CStats::SetMP(float nMP)
{
	m_fMP = nMP;
}

void CStats::SetMovementSpeed(float nMovementSpeed)
{
	m_fMovementSpeed = nMovementSpeed;
}

void CStats::SetBaseDamage(float nDamage)
{
	m_fBaseDamage = nDamage;
}

void CStats::SetAttackSpeed(float fAttackSpeed)
{
	m_fAttackSpeed = fAttackSpeed;
}

CStats & CStats::operator=(CStats & cCopy)
{
	m_fHP = cCopy.GetHP();
	m_fMP = cCopy.GetMP();
	m_fMovementSpeed = cCopy.GetMovementSpeed();
	m_fBaseDamage = cCopy.GetBaseDamage();

	return *this;
}


CStats::~CStats()
{
}
