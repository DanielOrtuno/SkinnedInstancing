#pragma once
#include "System.h"
#include <vector>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <dxgi1_2.h>
#include <dwrite.h>
#include <Windows.h>
#include "HUD.h"

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class CGUIManager : public ISystem
{
	ID2D1Factory4*				m_pd2dFactory;
	ID2D1Device3*				m_pd2dDevice;
	ID2D1DeviceContext3*		m_pd2dContext;
	ID2D1HwndRenderTarget*		m_pd2dHRT;
	ID2D1SolidColorBrush*		m_pd2dColorBrush; //Make an array of brushes
	ID2D1SolidColorBrush*		m_pd2dColorBrush2;
	ID2D1SolidColorBrush*		m_pd2dColorBrush3;
	IDXGIDevice*				m_pdxgDevice;
	IDXGISurface*				m_pdxgSurface;
	ID2D1RenderTarget*			m_pd2dRT;
	ID2D1Bitmap1*				m_pd2dBitmap;
	ID3D11Device*				m_pd3dDevice;
	static CHUD*				m_pcHud;
	RECT m_rc;

	// Writing text stuff
	IDWriteFactory*				m_pdwWriteFactory;
	IDWriteTextFormat*			m_pdwTextFormat;
	IDWriteTextFormat*			m_pdwTitleFormat;
	IDWriteTextFormat*			m_pdwHUDFormat;
	IDWriteTextFormat*			m_pdwInventoryFormat;

	// Menu stuff
	std::vector<CMenu*>			m_menus;
	unsigned int				m_nCurrent;
	float						m_fWidth, m_fHeight;
	bool						m_bShowInventory;

	// Only called in the constructor, but it sets up stuff for the inventory menu
	// pd3dDevice: Direct3D device to create textures
	// pd2dContext: Direct2D context to create sprites and spritebatches
	// fWidth: Swapchain width
	// fHeight: Swapchain Height
	// fwWidth: Window Width
	// fwHight: Window Height
	void SetHudMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight, float fwWidth, float fwHeight);

public:
	CGUIManager(ID3D11Device* m_pd3dDevice, IDXGISwapChain* _m_pdxgSwapChain, HWND window_handle = nullptr);

	// Draws everything necessary
	void Render();

	// Add a button to the specified menu
	// If the menu doesn't exist, it will create the menu
	// menu: Menu to add to
	// pFunction: Function that is called when a button is clicked
	// buttonText: Text displayed on the button
	// width: Swapchain width
	// height: Swapchain Height
	// wWidth: Window Width
	// wHeight: Window Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively 
	void AddButtonToMenu(int menu, void(*pFunction)(), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, float left = 0.0f, float right = 0.0f, float top = 0.0f, float bottom = 0.0f);

	// Add a slider to the specified menu
	// If the menu doesn't exist, it will create the menu
	// NOTE: Will be called when creating the options menus ONLY
	// menu: Menu to add to
	// nType: What volume will be affected in which menu
	// width: Swapchain width
	// height: Swapchain Height
	// wWidth: Window Width
	// wHeight: Window Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void AddSliderToMenu(int menu, int nType, int width, int height, float wWidth, float wHeight, 
		float left, float right, float top, float bottom);

	// Give a title to a menu
	// If the menu doesn't exist, it will create the menu
	// menu: Menu to add to
	// text: Text to show
	// width: Swapchain width
	// height: Swapchain Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void SetTitleToMenu(int menu, const WCHAR* title_text, int width, int height, 
		float left, float right, float top, float bottom);

	// Adds text to a menu, meant for describing sliders
	// If the menu doesn't exist, it will create the menu
	// NOTE: Will be called when creating volume sliders
	// menu: Menu to add to
	// text: Text to show
	// width: Swapchain width
	// height: Swapchain Height
	// left: ratio between 0-1 to define the left side of the text box
	// right: ratio between 0-1 to define the right side of the text box
	// top: ratio between 0-1 to define the top side of the text box
	// bottom: ratio between 0-1 to define the bottom side of the text box
	// Note: ratios use the passed in width and height respectively
	void AddDescriptionToMenu(int menu, const WCHAR* text, int width, int height,
		float left, float right, float top, float bottom);

	// Call this function when the window has been resized
	// fWidth: Swapchain width
	// fHeight: Swapchain height
	// fwWidth: Window width
	// fwHeight: Window Height
	void WindowResizeEvent(float fWidth, float fHeight, float fwWidth, float fwHeight);

	// Called to change current menu rendered
	// Only works if there can be that window
	// menu: Menu to change to
	void ChangeCurrentMenu(int menu);

	// This is probably still being called in the Event Manager
	// It doesn't need to be, but let me know if you want it gone
	// fCurrHealth: Player's current health
	// fMaxHealth: Player's max health
	// fCurrMana: Player's current mana
	// fMaxMana: Player's max mana
	static void SetPlayerValues(float fCurrHealth = 0.0f, float fMaxHealth = 0.0f, float fCurrMana = 0.0f, float fMaxMana = 0.0f);

	// Allows the HUD to create the Minimap of the current level
	// cMap: Level layout defined in the level constructor
	static void SetMiniMap(std::vector<std::vector<Spot>> cMap);

	// Called so we don't get out of the Options Menu prematurely
	int GetMenuState();

	// Updates all the UI items
	void Update();

	~CGUIManager();
};

