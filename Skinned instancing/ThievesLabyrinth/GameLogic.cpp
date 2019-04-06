#include "GameLogic.h"
#include "PhysicsManager.h"
#include "DebugManager.h"
#include "MeshRenderer.h"
#include "RenderManager.h"
#include "EventManager.h"

#include "AnimatorComponent.h"
#include "Entity.h"
#include "Transform.h"
#include <iostream>

int CGameLogic::m_nCurrentState;
bool CGameLogic::m_bStateInitialized;
CRenderManager* CGameLogic::m_pcRenderManager;
CEntityManager* CGameLogic::m_pcEntityManager;

CGameLogic::CGameLogic(HWND cWindow)
{
	m_pcComponentManager = new CComponentManager();

	m_pcEntityManager = new CEntityManager(m_pcComponentManager);

	m_pcEventManager = new CEventManager();

	m_pcPhysicsManager = new CPhysicsManager(m_pcComponentManager);

	m_pcRenderManager = new CRenderManager(cWindow, m_pcComponentManager);
	// Create Start Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::START, StartGame, L"Play", 1024, 576, 0.4f, 0.6f, 0.4f, 0.5f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START, StartToOptions, L"Options", 1024, 576, 0.4f, 0.6f, 0.6f, 0.7f);
	m_pcRenderManager->AddButtonToMenu(eMenu::START, ExitGame, L"Quit", 1024, 576, 0.4f, 0.6f, 0.8f, 0.9f);
	m_pcRenderManager->SetTitleToMenu(eMenu::START, L"Thieves Labyrinth", 0.3f, 0.7f, 0.1f, 0.3f);
	// Create Pause Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE, TogglePause, L"Resume", 1024, 576, 0.4f, 0.6f, 0.4f, 0.5f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE, PauseToOptions, L"Options", 1024, 576, 0.4f, 0.6f, 0.6f, 0.7f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE, GoBackToMain, L"Exit To Main Menu", 1024, 576, 0.4f, 0.6f, 0.8f, 0.9f);
	m_pcRenderManager->SetTitleToMenu(eMenu::PAUSE, L"Pause", 0.3f, 0.7f, 0.1f, 0.3f);
	// Create Win Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::WIN, GoBackToMain, L"Exit To Main Menu", 1024, 576, 0.4f, 0.6f, 0.4f, 0.6f);
	m_pcRenderManager->SetTitleToMenu(eMenu::WIN, L"You Win!!", 0.3f, 0.7f, 0.1f, 0.3f);
	// Create Lose Menu
	m_pcRenderManager->AddButtonToMenu(eMenu::LOSE, GoBackToMain, L"Exit To Main Menu", 1024, 576, 0.4f, 0.6f, 0.4f, 0.6f);
	m_pcRenderManager->SetTitleToMenu(eMenu::LOSE, L"You Lose!!", 0.3f, 0.7f, 0.1f, 0.3f);
	BuildOptionsMenus();

	m_bStateInitialized = false;

#pragma region Camera Creation

	CCameraEntity* pMainCamera = (CCameraEntity*)m_pcEntityManager->CreateEntity(eEntity::CAMERA);
	m_pcEntityManager->AddComponentToEntity(pMainCamera, eComponent::TRANSFORM);
	m_pcEntityManager->AddComponentToEntity(pMainCamera, eComponent::RIGIDBODY);
	m_pcEntityManager->AddComponentToEntity(pMainCamera, eComponent::CAMERA_CONTROLLER);

	CTransform* cameraTransform = pMainCamera->GetComponent<CTransform>();
	cameraTransform->SetPosition(CMath::TVECTOR3(0, 0, 0));

#pragma endregion

	m_nCurrentState = eGameState::PLAYING;
	m_pcRenderManager->ChangeCurrentMenu(eMenu::GAME);

	for(int y = 0; y < 31; y++)
	{
		for(int x = 0; x < 31; x++)
		{
			CEnemyEntity* pcEntity = (CEnemyEntity*)CEntityManager::CreateEntity(eEntity::ENEMY, eEnemyType::MAGE);

			CTransform* pcTransform = (CTransform*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::TRANSFORM);
			pcTransform->SetPosition({ x * 4.0f, 0.0f, y * 4.0f });
			pcTransform->SetRotation({ 0, 1, 0, 0});

			CMeshRenderer* pcMeshRenderer = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::MESH_RENDERER);
			pcMeshRenderer->SetVertexBuffer(eVertexBuffer::ENEMY_MAGE);
			pcMeshRenderer->SetIndexBuffer(eIndexBuffer::ENEMY_MAGE);
			pcMeshRenderer->SetTextureCount(1);
			pcMeshRenderer->SetTexture(eTexture::ENEMY_MAGE_DIFFUSE, 0);
			pcMeshRenderer->SetInputLayout(eInputLayout::SKINNED);
			pcMeshRenderer->SetVertexShader(eVertexShader::SKINNED);
			pcMeshRenderer->SetPixelShader(ePixelShader::DEFAULT);
			pcMeshRenderer->SetSampler(eSampler::CLAMP);

			CAnimatorComponent* pcAnimator = (CAnimatorComponent*)CEntityManager::AddComponentToEntity(pcEntity, eComponent::ANIMATOR);
			
			pcAnimator->SetAnimator(eAnimator::MAGE);

			int nAnimation = rand() % eEnemyAnimation::COUNT;
			pcAnimator->SetAnimation(nAnimation);

			double fRandNumber = 0;
			switch(nAnimation)
			{
				case eEnemyAnimation::IDLE:
				{
					fRandNumber = (double)( rand() ) / ( (double)( RAND_MAX / 2.49 ) );
					pcAnimator->SetAnimationTime(fRandNumber);
					break;
				}

				case eEnemyAnimation::WALK:
				{
					fRandNumber = (double)( rand() ) / ( (double)( RAND_MAX / 1.19 ) );
					pcAnimator->SetAnimationTime(fRandNumber);
					break;
				}
			}
		}
	}
}

CGameLogic::~CGameLogic()
{
	delete m_pcRenderManager;
	delete m_pcPhysicsManager;
	delete m_pcEventManager;
	delete m_pcEntityManager;
	delete m_pcComponentManager;
}

void CGameLogic::Update()
{
	switch (m_nCurrentState)
	{
	case eGameState::MAIN_MENU:
	{
		m_pcEventManager->Notify();
		m_pcRenderManager->Draw();
		break;
	}

	case eGameState::LOADING_LEVEL:
	{


		break;
	}

	case eGameState::PLAYING:
	{
		if (!m_bStateInitialized)
		{
			// TODO Set Music to normal volume
			// TODO Play FootSteps
			// TODO Close pause menu
			m_pcRenderManager->ChangeCurrentMenu(eMenu::GAME);
			m_bStateInitialized = true;
		}

		m_pcRenderManager->Draw();

		if (CInputManager::GetKeyPress(VK_ESCAPE))
		{
			TogglePause();
		}

		m_pcComponentManager->UpdateControllers();
		m_pcPhysicsManager->UpdateRigidBodies(CTime::GetDelta());

		break;
	}

	case eGameState::PAUSED:
	{
		if (!m_bStateInitialized)
		{
			ToPause();
			m_bStateInitialized = true;
		}
		m_pcRenderManager->Draw();
		m_pcEventManager->Notify();
		if (CInputManager::GetKeyPress(VK_ESCAPE))
		{
			if (m_pcRenderManager->GetMenuState() == eMenu::PAUSE)
			{
				TogglePause();
			}
			else
			{
				ToPause();
			}
		}

		break;
	}

	case eGameState::QUIT:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		break;
	}

	m_cTime.Update();
}

void CGameLogic::BuildOptionsMenus()
{
	// Hand the back buttons to the options menus
	m_pcRenderManager->AddButtonToMenu(eMenu::START_OPTIONS, ToStart, L"Back", 1024, 576, 0.4f, 0.6f, 0.8f, 0.9f);
	m_pcRenderManager->AddButtonToMenu(eMenu::PAUSE_OPTIONS, ToPause, L"Back", 1024, 576, 0.4f, 0.6f, 0.8f, 0.9f);
	// Set the slider stuff for the options menu linked to start
	// MASTER OF VOLUMES I'M PULLING YOUR STRINGS!!
	m_pcRenderManager->AddSliderToMenu(eMenu::START_OPTIONS, eAudio::COUNT, 1024, 576, 0.4f, 0.8f, 0.35f, 0.45f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::START_OPTIONS, L"Master Volume", 0.15f, 0.35f, 0.35f, 0.45f);
	// Please have this off, don't toggle it anymore
	m_pcRenderManager->AddSliderToMenu(eMenu::START_OPTIONS, eAudio::MUSIC, 1024, 576, 0.4f, 0.8f, 0.5f, 0.6f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::START_OPTIONS, L"Music Volume", 0.15f, 0.35f, 0.5f, 0.6f);
	// I honestly don't care what volume you have this at
	m_pcRenderManager->AddSliderToMenu(eMenu::START_OPTIONS, eAudio::SFX, 1024, 576, 0.4f, 0.8f, 0.65f, 0.75f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::START_OPTIONS, L"SFX Volume", 0.15f, 0.35f, 0.65f, 0.75f);
	// GIVE IT A NAME!!
	m_pcRenderManager->SetTitleToMenu(eMenu::START_OPTIONS, L"Options", 0.3f, 0.7f, 0.15f, 0.35f);
	// Second verse, same as the first.  This time for the menu linked to pause
	m_pcRenderManager->AddSliderToMenu(eMenu::PAUSE_OPTIONS, eAudio::COUNT, 1024, 576, 0.4f, 0.8f, 0.35f, 0.45f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::PAUSE_OPTIONS, L"Master Volume", 0.15f, 0.35f, 0.35f, 0.45f);
	m_pcRenderManager->AddSliderToMenu(eMenu::PAUSE_OPTIONS, eAudio::MUSIC, 1024, 576, 0.4f, 0.8f, 0.5f, 0.6f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::PAUSE_OPTIONS, L"Music Volume", 0.15f, 0.35f, 0.5f, 0.6f);
	m_pcRenderManager->AddSliderToMenu(eMenu::PAUSE_OPTIONS, eAudio::SFX, 1024, 576, 0.4f, 0.8f, 0.65f, 0.75f);
	m_pcRenderManager->AddDescriptionToMenu(eMenu::PAUSE_OPTIONS, L"SFX Volume", 0.15f, 0.35f, 0.65f, 0.75f);
	m_pcRenderManager->SetTitleToMenu(eMenu::PAUSE_OPTIONS, L"Options", 0.3f, 0.7f, 0.15f, 0.35f);
}

void CGameLogic::StartGame()
{
	m_nCurrentState = eGameState::LOADING_LEVEL;
}

void CGameLogic::ExitGame()
{
	m_nCurrentState = eGameState::QUIT;
}

void CGameLogic::TogglePause()
{
	if (m_nCurrentState == eGameState::PLAYING)
	{
		m_nCurrentState = eGameState::PAUSED;
		m_bStateInitialized = false;
	}

	else if (m_nCurrentState == eGameState::PAUSED)
	{
		m_nCurrentState = eGameState::PLAYING;
		m_bStateInitialized = false;
	}
}

void CGameLogic::GoBackToMain()
{
	m_nCurrentState = eGameState::MAIN_MENU;
	m_pcRenderManager->ChangeCurrentMenu(eMenu::START);
}

void CGameLogic::StartToOptions()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::START_OPTIONS);
}

void CGameLogic::PauseToOptions()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::PAUSE_OPTIONS);
}

void CGameLogic::ToPause()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::PAUSE);
}

void CGameLogic::ToStart()
{
	m_pcRenderManager->ChangeCurrentMenu(eMenu::START);
}
