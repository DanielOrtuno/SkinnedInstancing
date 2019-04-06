#include "CameraController.h"
#include "InputManager.h"
#include "Rigidbody.h"
#include "EventManager.h"
#include "Entity.h"
#include "Transform.h"

#include "EnumTypes.h"

#include <DirectXMath.h>
using namespace DirectX;

CCameraController::CCameraController(IEntity* pcOwner) : IComponent(pcOwner)
{
	//m_pPlayerPos = CEventManager::SendEntityMessage(TEntityMessage(0))->GetComponent<CTransform>();
	m_pCameraTransform = m_pcOwner->GetComponent<CTransform>();

	CInputManager::SetViewMat(*m_pCameraTransform);

	m_pcRigidBody = m_pcOwner->GetComponent<CRigidbody>();

	m_nComponentType = eComponent::CAMERA_CONTROLLER;
	m_fSpeed = 15.0f;
	m_fSpeedDebug = 100.0f;

	m_pCameraTransform->SetPosition({ 0, 0, 0 });
}

void CCameraController::Update()
{
	if(!m_pcRigidBody)
	{
		// TODO: Add debug error message
		return;
	}

	float fForwardMovement = 0;
	float fSideMovement = 0;
	float fUpwardMovement = 0;


	if(CInputManager::GetKeyDown(VK_UP))
	{
		fForwardMovement = 1;
	}
	else if(CInputManager::GetKeyDown(VK_DOWN))
	{
		fForwardMovement = -1;
	}

	if(CInputManager::GetKeyDown(VK_RIGHT))
	{
		fSideMovement = 1;
	}
	else if(CInputManager::GetKeyDown(VK_LEFT))
	{
		fSideMovement = -1;
	}
		
	if (GetAsyncKeyState(VK_RSHIFT))
	{
		fUpwardMovement = 1;
	}
	if (GetAsyncKeyState(VK_OEM_2))
	{
		fUpwardMovement = -1;
	}

	m_pcRigidBody->SetVelocity(CMath::Vector3Normalize(CMath::TVECTOR3{ fSideMovement, fUpwardMovement, fForwardMovement }) * 10.0f);
}

CCameraController::~CCameraController()
{
}
