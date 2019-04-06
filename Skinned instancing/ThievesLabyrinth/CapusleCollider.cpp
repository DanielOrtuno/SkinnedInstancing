#include "CapsuleCollider.h"
#include "EventManager.h"
#include "Transform.h"

CCapsuleCollider::CCapsuleCollider(IEntity* pcOwner) : ICollider(pcOwner)
{
	m_pcOwner = pcOwner;
	m_tCenter = CMath::TVECTOR3(0, 0, 0);
	m_fHeight = 5.0f;
	m_fRadius = 2.0f;
}

CCapsuleCollider & CCapsuleCollider::operator=(CCapsuleCollider & cCopy)
{
	m_tCenter = cCopy.GetCenter();
	m_fHeight = cCopy.GetHeight();
	m_fRadius = cCopy.GetRadius();
	m_bIsTrigger = cCopy.IsTrigger();

	return *this;
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

void CCapsuleCollider::GetMiddleSegment(CMath::TVECTOR3 & tBottom, CMath::TVECTOR3 & tTop)
{
	CMath::TMATRIX tTransformMatrix = m_pcTransform->GetMatrix();
	CMath::TVECTOR3 tYAxis = CMath::TVECTOR3(tTransformMatrix.r[1].x, tTransformMatrix.r[1].y, tTransformMatrix.r[1].z);

	tBottom = m_pcTransform->GetPosition() + m_tCenter - tYAxis * (m_fHeight / 2.0f);
	tTop = m_pcTransform->GetPosition() + m_tCenter + tYAxis * (m_fHeight / 2.0f);
}

void CCapsuleCollider::SetCenter(CMath::TVECTOR3 tCenter)
{
	m_tCenter = tCenter;
}

void CCapsuleCollider::SetHeight(float fHeight)
{
	m_fHeight = fHeight - m_fRadius * 2.0f;
}

void CCapsuleCollider::SetRadius(float fRadius)
{
	m_fHeight += m_fRadius * 2.0f;
	m_fRadius = fRadius;
	m_fHeight -= m_fRadius * 2.0f;
}

void CCapsuleCollider::RenderCollider()
{
	CMath::TMATRIX tTransformMatrix = m_pcTransform->GetMatrix();

	CMath::TVECTOR3 m_tExtent = CMath::TVECTOR3(m_fRadius, m_fHeight / 2.0f, m_fRadius);

	CMath::TVECTOR3 AA = CMath::Vector3Transform(m_tExtent, tTransformMatrix) + m_tCenter;
	CMath::TVECTOR3 BA = CMath::Vector3Transform(CMath::TVECTOR3{ -m_tExtent.x,  m_tExtent.y,   m_tExtent.z }, tTransformMatrix) + m_tCenter;
	CMath::TVECTOR3 CA = CMath::Vector3Transform(CMath::TVECTOR3{ -m_tExtent.x,  m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + m_tCenter;
	CMath::TVECTOR3 DA = CMath::Vector3Transform(CMath::TVECTOR3{ m_tExtent.x,  m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + m_tCenter;
	CMath::TVECTOR3 AB = CMath::Vector3Transform(CMath::TVECTOR3{ m_tExtent.x, -m_tExtent.y,   m_tExtent.z }, tTransformMatrix) + m_tCenter;
	CMath::TVECTOR3 BB = CMath::Vector3Transform(CMath::TVECTOR3{ -m_tExtent.x, -m_tExtent.y,   m_tExtent.z }, tTransformMatrix) + m_tCenter;
	CMath::TVECTOR3 CB = CMath::Vector3Transform(CMath::TVECTOR3{ -m_tExtent.x, -m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + m_tCenter;
	CMath::TVECTOR3 DB = CMath::Vector3Transform(CMath::TVECTOR3{ m_tExtent.x, -m_tExtent.y,  -m_tExtent.z }, tTransformMatrix) + m_tCenter;

	CMath::TVECTOR3 Top = m_pcTransform->GetPosition() + m_tCenter + CMath::TVECTOR3(0, (0.5f * m_fHeight) + m_fRadius, 0);
	CMath::TVECTOR3 Bottom = m_pcTransform->GetPosition() + m_tCenter - CMath::TVECTOR3(0, (0.5f * m_fHeight) + m_fRadius, 0);

	//Top square 
	TDebugLineMessage vertex = TDebugLineMessage(AA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(BA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(CA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(DA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(DA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(AA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	//Bottom square 
	vertex = TDebugLineMessage(AB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(BB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(CB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(DB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);


	vertex = TDebugLineMessage(DB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(AB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	//Connectors

	vertex = TDebugLineMessage(AA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(AB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(BB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(CB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);


	vertex = TDebugLineMessage(DA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(DB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	// Top
	vertex = TDebugLineMessage(AA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(DA, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Top, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	// Bottom
	vertex = TDebugLineMessage(AB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Bottom, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(BB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Bottom, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(CB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Bottom, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);

	vertex = TDebugLineMessage(DB, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
	vertex = TDebugLineMessage(Bottom, CMath::TVECTOR4{ 0, 1, 0, 1 });
	CEventManager::SendDebugLineMessage(vertex);
}
