#pragma once
#include "System.h"
#include <Windows.h>
#include <bitset>
#include "Math.h"

class CTransform;

#define NBITSIZE 50

class CInputManager : public ISystem
{
	static std::bitset<NBITSIZE> m_bKeyPresses;
	static std::bitset<NBITSIZE> m_bKeysDown;
	static int	nMouseXPos, nMouseYPos, nScreenWidth, nScreenHeight, nXDiff, nYDiff;
	static CMath::TMATRIX* m_pProjMat;
	static CTransform* m_pCameraTransform;

	// This function is utilized to allow keys to
	// return true in GetKeyPress again
	static void UpdatePresses();

public:
	// Checks to see if a key is pressed once
	// Works once until the key is released
	// chKey: Key to get
	// NOTE: Alphabet letters should be capitalized
	static bool GetKeyPress(char chKey);

	// Checks to see if a key is down
	// Works constantly until the key is released
	// chKey: Key to get
	// NOTE: Alphabet letters should be capitalized
	static bool GetKeyDown(char chKey);

	// Get the mouse positions to be calculated to world space
	// nMouseX: integer to take the mouse's x position
	// nMouseY: integer to take the mouse's y position
	static void GetMousePos(int& nMouseX, int& nMouseY);

	// Get the mouse position converted to NDC space
	// _nX: integer to take the mouse's x position in NDC space
	// _nY: integer to take the mouse's y position in NDC space
	static void GetMousePosNDC(float& _nX, float& _nY);

	// Get the difference in mouse position
	// nDiffX: integer to take the mouse's x difference
	// nDiffY: integer to take the mouse's y difference
	static void GetMouseDelta(int& nDiffX, int& nDiffY);

	// Get the window width and height
	// nWidth: integer to take the window's width
	// nHeight: integer to take the window's height
	static void GetScreenWH(int& nWidth, int& nHeight);

	// Sets the view matrix that InputManager is pointing to
	// _mView: matrix to point to
	static void SetViewMat(CTransform& _mView);
	   	 
	// Sets the proj matrix that InputManager is pointing to
	// _mProj: matrix to point to
	static void SetProjMat(CMath::TMATRIX& _mProj);

	// Returns the mouse x y z in world space
	static void GetMouseWorldSpace(const CMath::TVECTOR2 &vScreenSpacePosition, CMath::TVECTOR3 &vWorldStart, CMath::TVECTOR3 &vWorldEnd);

	// WARNING: THIS FUNCTION SHOULD ONLY BE DECLARED IN MAIN
	// CALLING THIS ANYWHERE ELSE CAN BREAK THE INPUT MANAGER
	// Processes window inputs
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

