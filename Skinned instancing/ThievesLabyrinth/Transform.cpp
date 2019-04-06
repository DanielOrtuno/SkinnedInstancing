#include "Transform.h"
#include "EventManager.h"
#include "EnumTypes.h"

CTransform::CTransform(IEntity* pcOwner) : IComponent(pcOwner)
{
	m_nComponentType = eComponent::TRANSFORM;
	m_tPosition = CMath::TVECTOR3(0, 0, 0);
	m_tRotation = CMath::TVECTOR4(0, 0, 0, 1);
	m_tScale = CMath::TVECTOR3(1, 1, 1);
}

CTransform::~CTransform()
{

}

void CTransform::SetPosition(CMath::TVECTOR3 tPosition)
{
	m_tPosition = tPosition;
}

void CTransform::SetRotation(CMath::TVECTOR4 tRotation)
{
	m_tRotation = tRotation;
}

void CTransform::SetScale(CMath::TVECTOR3 tScale)
{
	m_tScale = tScale;
}

CMath::TVECTOR3 CTransform::GetPosition()
{
	return m_tPosition;
}

CMath::TVECTOR4 CTransform::GetRotation()
{
	return m_tRotation;
}

CMath::TVECTOR3 CTransform::GetScale()
{
	return m_tScale;
}

CMath::TMATRIX CTransform::GetMatrix()
{
	CMath::TMATRIX Translation = CMath::MatrixTranslationFromVector(m_tPosition);
	CMath::TMATRIX Rotation = CMath::MatrixRotationQuaternion(m_tRotation);
	CMath::TMATRIX Scale = CMath::MatrixScaleFromVector(m_tScale);

	CMath::TMATRIX result =  Rotation * Scale;
//	DirectX::XMMATRIX r = *(DirectX::XMMATRIX*)&Translation **(DirectX::XMMATRIX*)&Rotation **(DirectX::XMMATRIX*)&Scale;

	result.mData[12] = m_tPosition.x;
	result.mData[13] = m_tPosition.y;
	result.mData[14] = m_tPosition.z;

	return result;
}

void CTransform::LookAt(CMath::TVECTOR3 newForward)
{
	CMath::TVECTOR3 forward = newForward;
	CMath::TVECTOR3 up = CMath::TVECTOR3(0, 1, 0);
	CMath::TVECTOR3 right = CMath::Vector3Cross(up, forward);
	up = CMath::Vector3Cross(forward, right);

	CMath::TVECTOR4 newRotation;
	float tr = right.x + up.y + forward.z;

	if (tr > 0)
	{
		float S = sqrtf(tr + 1.0f) * 2; // S=4*newRotation.w 
		newRotation.w = 0.25f * S;
		newRotation.x = (forward.y - up.z) / S;
		newRotation.y = (right.z - forward.x) / S;
		newRotation.z = (up.x - right.y) / S;
	}
	else if ((right.x > up.y) && (right.x > forward.z))
	{
		float S = sqrtf(1.0f + right.x - up.y - forward.z) * 2; // S=4*newRotation.x 
		newRotation.w = (forward.y - up.z) / S;
		newRotation.x = 0.25f * S;
		newRotation.y = (right.y + up.x) / S;
		newRotation.z = (right.z + forward.x) / S;
	}
	else if (up.y > forward.z)
	{
		float S = sqrtf(1.0f + up.y - right.x - forward.z) * 2; // S=4*newRotation.y
		newRotation.w = (right.z - forward.x) / S;
		newRotation.x = (right.y + up.x) / S;
		newRotation.y = 0.25f * S;
		newRotation.z = (up.z + forward.y) / S;
	}
	else
	{
		float S = sqrtf(1.0f + forward.z - right.x - up.y) * 2; // S=4*newRotation.z
		newRotation.w = (up.x - right.y) / S;
		newRotation.x = (right.z + forward.x) / S;
		newRotation.y = (up.z + forward.y) / S;
		newRotation.z = 0.25f * S;
	}

	this->SetRotation(newRotation);
}

void CTransform::TurnTo(CMath::TVECTOR3 newForward, float fSpeed)
{
	CMath::TMATRIX matrix = GetMatrix();
	float degrees = CMath::Vector3Dot(newForward, CMath::TVECTOR3(matrix.r[0].x, matrix.r[0].y, matrix.r[0].z));
	if(degrees < 0.05f && degrees > -0.05f && CMath::Vector3Dot(newForward, CMath::TVECTOR3(matrix.r[2].x, matrix.r[2].y, matrix.r[2].z)) < 0)
		degrees = -1.0f;
	degrees *= fSpeed * 90.0f;

	CMath::TVECTOR4 rotation = CMath::QuaternionFromAxisAngle(CMath::TVECTOR3(0, 1, 0), -degrees);
	CMath::TVECTOR4 newRotation = CMath::QuaternionMultiply(GetRotation(), rotation);

	this->SetRotation(newRotation);
}

void CTransform::TurnTo(CTransform * target)
{
	CMath::TVECTOR3 newForward = CMath::Vector3Normalize(target->GetPosition() - this->m_tPosition);
	this->TurnTo(newForward);
}

void CTransform::RenderTransform()
{
	CMath::TMATRIX tMatrix = GetMatrix();

	CMath::TVECTOR4 tColors[3]{ { 1, 0, 0, 1}, { 0, 1, 0, 1}, { 0, 0, 1, 1} };

	CMath::TVECTOR3 tPosA{ tMatrix.r[3].x, tMatrix.r[3].y, tMatrix.r[3].z };

	float fScaleFactor = 5;
	for(int i = 0; i < 3; i++)
	{
		TDebugLineMessage cVertex = TDebugLineMessage(tPosA, tColors[i]);
		CEventManager::SendDebugLineMessage(cVertex);

		CMath::TVECTOR3 tPosB{ tPosA.x + tMatrix.r[i].x * fScaleFactor,  tPosA.y + tMatrix.r[i].y * fScaleFactor,  tPosA.z + tMatrix.r[i].z * fScaleFactor };

		cVertex = TDebugLineMessage(tPosB, tColors[i]);
		CEventManager::SendDebugLineMessage(cVertex);

	}

}

CTransform & CTransform::operator=(CTransform & cCopy)
{
	m_tPosition = cCopy.GetPosition();
	m_tRotation = cCopy.GetRotation();
	m_tScale = cCopy.GetScale();

	return *this;
}

