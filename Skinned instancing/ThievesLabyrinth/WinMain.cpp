#include <windows.h>
#include <DbgHelp.h>
//#include <windowsx.h>
//#include <tchar.h>
#include <iostream>
//#include "RenderManager.h"
//#include <bitset>
//#include "InputManager.h"
//#include "PhysicsManager.h"
//#include "LevelManager.h"
//#include "AudioManager.h"
#include "DebugManager.h"
//#include "Transform.h"
#include "GameLogic.h"
#include <random>
#include <time.h>

#pragma comment (lib,"Dbghelp.lib")



static TCHAR szWindowClass[] = _T("Thieves Labyrinth");
static TCHAR szTitle[] = _T("Thieves Labyrinth");

HINSTANCE hInst;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand((unsigned int)time(0));

	SetUnhandledExceptionFilter(&CDebugManager::errorFunc);

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(26453);
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	WNDCLASSEX windowDesc;
	windowDesc.cbSize = sizeof(WNDCLASSEX);
	windowDesc.style = CS_HREDRAW | CS_VREDRAW;
	windowDesc.lpfnWndProc = CInputManager::WndProc;
	windowDesc.cbClsExtra = 0;
	windowDesc.cbWndExtra = 0;
	windowDesc.hInstance = hInstance;
	windowDesc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	windowDesc.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowDesc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowDesc.lpszMenuName = NULL;
	windowDesc.lpszClassName = szWindowClass;
	windowDesc.hIconSm = LoadIcon(windowDesc.hInstance, IDI_APPLICATION);


	if (!RegisterClassEx(&windowDesc))
	{
		MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}

	hInst = hInstance;

	HWND windowHandle = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 576, NULL, NULL, hInstance, NULL);

	if (!windowHandle)
	{
		MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Windows Desktop Guided Tour"), NULL);
		return 1;
	}

	ShowWindow(windowHandle, nCmdShow);
	UpdateWindow(windowHandle);

	AllocConsole();
	FILE* new_std_in_out;
	freopen_s(&new_std_in_out, "CONOUT$", "w", stdout);
	freopen_s(&new_std_in_out, "CONIN$", "r", stdin);
	std::cout << "I can smell the debug:" << std::endl;

	CGameLogic* pcGameLogic = new CGameLogic(windowHandle);


	MSG msg;
	while (true)
	{
		//GetMessage(&msg, NULL, 0, 0);
		// Process all messages, stop on WM_QUIT 
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}
		else
		{
			//std::cout << rand() << std::endl;
			pcGameLogic->Update();
		}
	}

	delete pcGameLogic;
	//_CrtDumpMemoryLeaks();
	return (int)msg.wParam;
}