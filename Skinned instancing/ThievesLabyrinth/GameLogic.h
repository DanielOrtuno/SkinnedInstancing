#pragma once
#include "InputManager.h"
#include <bitset>
#include <Windows.h>
#include <windowsx.h>
#include <tchar.h>
#include "Time.h"

#define CHEATCODES

class CComponentManager;
class CEntityManager;
class CEventManager;
class CAudioManager;
class CRenderManager;
class CPhysicsManager;
class CLevelManager;
class CAIManager;
class CTime;

class CGameLogic : public ISystem
{
private:
	CComponentManager* m_pcComponentManager;
	static CEntityManager* m_pcEntityManager;
	CEventManager* m_pcEventManager;
	static CRenderManager* m_pcRenderManager;
	CPhysicsManager* m_pcPhysicsManager;
	CTime m_cTime;
	static int m_nCurrentState;
	static bool m_bStateInitialized;

#ifdef CHEATCODES
	bool bGodMode = false;
	bool bInfiniteMana = false;
	bool bRefillHealth = false;;
#endif // CHEATCODES

public:
	CGameLogic(HWND cWindow);
	~CGameLogic();

	void Update();
	void BuildOptionsMenus();

	static void StartGame();
	static void ExitGame();
	static void TogglePause();
	static void GoBackToMain();
	static void StartToOptions();
	static void PauseToOptions();
	static void ToPause();
	static void ToStart();

};
