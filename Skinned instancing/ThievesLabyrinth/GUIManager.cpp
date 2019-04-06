#pragma once
#include "GUIManager.h"
#include "EnumTypes.h"
#include "InputManager.h"
#include "Time.h"
#include "WICTextureLoader.h"
#include "InputManager.h"

CHUD* CGUIManager::m_pcHud;

CGUIManager::CGUIManager(ID3D11Device* pd3dDevice, IDXGISwapChain* m_pdxgSwapChain, HWND window_handle)
{
	m_nCurrent = eMenu::START;
	m_pd2dFactory = nullptr;
	m_pd2dDevice = nullptr;
	m_pd2dContext = nullptr;
	m_pd2dHRT = nullptr;
	m_pd2dColorBrush = nullptr;
	m_pd2dColorBrush2 = nullptr;
	m_pd2dColorBrush3 = nullptr;
	m_pdxgDevice = nullptr;
	m_pdxgSurface = nullptr;
	m_pd2dBitmap = nullptr;
	m_pd2dRT = nullptr;
	m_pdwWriteFactory = nullptr;
	m_pdwTextFormat = nullptr;
	m_pdwTitleFormat = nullptr;
	m_pcHud = nullptr;
	m_bShowInventory = false;
	m_pd3dDevice = pd3dDevice;

	// Initialize D2DFactory to create device and swapchain
	HRESULT hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)&m_pdxgDevice);
	D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_NONE };
#ifdef _DEBUG
	options.debugLevel = D2D1_DEBUG_LEVEL_ERROR;
#endif
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, options, &m_pd2dFactory);
	// Make D2D1Device3
	hr = m_pd2dFactory->CreateDevice(m_pdxgDevice, &m_pd2dDevice);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"D2D1CreateDevice", L"Error", MB_OK);
		exit(0);
	}
	// Make D2D1DeviceContext3
	// This is the only way to render sprites to the screen
	hr = m_pd2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pd2dContext);
	m_pd2dContext->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);

	// Create render target for text rendering
	FLOAT dpiX, dpiY;
	m_pd2dFactory->GetDesktopDpi(&dpiX, &dpiY);
	hr = m_pdxgSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&m_pdxgSurface));
	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			dpiX, dpiY
		);
	hr = m_pd2dFactory->CreateDxgiSurfaceRenderTarget(m_pdxgSurface, &props, &m_pd2dRT);

	D2D1_COLOR_F test;
	test.b = test.g = test.r = 0.0f;
	test.a = 0.5f;
	// Spell pig backwards, then say pretty colors
	hr = m_pd2dRT->CreateSolidColorBrush(test, &m_pd2dColorBrush);
	hr = m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Gold), &m_pd2dColorBrush2);
	hr = m_pd2dRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LimeGreen), &m_pd2dColorBrush3);

	// Pain in the butt text rendering
	hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown**)&m_pdwWriteFactory);
	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Arial",					// Font family name.
		NULL,                       // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,
		L"en-us",
		&m_pdwTextFormat
	);

	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Arial",					// Font family name.
		NULL,                       // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_ULTRA_BOLD,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		40.0f,
		L"en-us",
		&m_pdwTitleFormat
	);

	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Arial",					// Font family name.
		NULL,                       // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,
		L"en-us",
		&m_pdwHUDFormat
	);

	hr = m_pdwWriteFactory->CreateTextFormat(
		L"Arial",					// Font family name.
		NULL,                       // Font collection (NULL sets it to use the system font collection).
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f,
		L"en-us",
		&m_pdwInventoryFormat
	);

	// This centers all text to the center of their specific box
	m_pdwTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pdwTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	// It sucks that this has to happen for different Font sizes
	m_pdwTitleFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	m_pdwTitleFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	// And for different positioning of text
	m_pdwHUDFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	m_pdwHUDFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	// This is why I wasn't the biggest fan of English
	m_pdwInventoryFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	m_pdwInventoryFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);

	// Hold onto swapchain width and height for drawing the hud
	DXGI_SWAP_CHAIN_DESC sd;
	m_pdxgSwapChain->GetDesc(&sd);
	m_fWidth = (float)sd.BufferDesc.Width;
	m_fHeight = (float)sd.BufferDesc.Height;

	// So this is used to create the Bitmap to render sprites to
	DXGI_SURFACE_DESC sDesc;
	D2D1_BITMAP_PROPERTIES1 bitProp;
	IDXGISurface2* texSurface = nullptr;
	ZeroMemory(&sDesc, sizeof(DXGI_SURFACE_DESC));
	ZeroMemory(&bitProp, sizeof(D2D1_BITMAP_PROPERTIES1));
	m_pdxgSurface->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface); // should always succeed
	texSurface->GetDesc(&sDesc);
	bitProp.pixelFormat = { sDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED };
	bitProp.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	// create compatible bitmap to main DXGI surface (for high speed sprite rendering)
	if (FAILED(m_pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &m_pd2dBitmap)))
	{
		// This will severely upset me if it happens
		MessageBox(nullptr, L"Failed to make a bitmap to render menu stuff to", L"Error", MB_OK);
		texSurface->Release();
		exit(0);
	}
	texSurface->Release();
	// THAT WAS TOO LONG!!
	m_pcHud = new CHUD(pd3dDevice, m_pd2dContext, m_fWidth, m_fHeight);
	SetHudMenu(pd3dDevice, m_pd2dContext, m_fWidth, m_fHeight, m_fWidth, m_fHeight);
}


void CGUIManager::Update()
{
	if (CInputManager::GetKeyPress(VK_TAB))
	{
		//m_bShowInventory = !m_bShowInventory;
	}
	if (m_nCurrent == eMenu::GAME)
	{
		//m_pcHud->Update();
	}
	else
	{
		//m_menus[m_nCurrent]->Update();
	}
}

CGUIManager::~CGUIManager()
{
	// SAFE RELEASE EVERYTHING!!
	if (m_pdwTextFormat)	{ m_pdwTextFormat->Release();		m_pdwTextFormat = nullptr; }
	if (m_pdwTitleFormat)	{ m_pdwTitleFormat->Release();		m_pdwTitleFormat = nullptr; }
	if (m_pdwHUDFormat)		{ m_pdwHUDFormat->Release();		m_pdwHUDFormat = nullptr; }
	if (m_pdwWriteFactory)	{ m_pdwWriteFactory->Release();		m_pdwWriteFactory = nullptr; }
	if (m_pd2dBitmap)		{ m_pd2dBitmap->Release();			m_pd2dBitmap = nullptr; }
	if (m_pd2dHRT)			{ m_pd2dHRT->Release();				m_pd2dHRT = nullptr; }
	if (m_pd2dRT)			{ m_pd2dRT->Release();				m_pd2dRT = nullptr; }
	if (m_pd2dColorBrush)	{ m_pd2dColorBrush->Release();		m_pd2dColorBrush = nullptr; }
	if (m_pd2dColorBrush2)	{ m_pd2dColorBrush2->Release();		m_pd2dColorBrush2 = nullptr; }
	if (m_pd2dColorBrush3)	{ m_pd2dColorBrush3->Release();		m_pd2dColorBrush3 = nullptr; }
	if (m_pd2dContext)		{ m_pd2dContext->Release();			m_pd2dContext = nullptr; }
	if (m_pd2dFactory)		{ m_pd2dFactory->Release();			m_pd2dFactory = nullptr; }
	if (m_pd2dDevice)		{ m_pd2dDevice->Release();			m_pd2dDevice = nullptr; }
	if (m_pdxgDevice)		{ m_pdxgDevice->Release();			m_pdxgDevice = nullptr; }
	if (m_pdxgSurface)		{ m_pdxgSurface->Release();			m_pdxgSurface = nullptr; }
	if (m_pcHud)			  delete m_pcHud;

	for (size_t i = 0; i < m_menus.size(); i++)
	{
		if (m_menus[i])
			delete m_menus[i];
	}
}

void CGUIManager::Render()
{
	// initial setup for FPS
	static double delta = 0.0;
	static double hold = 0.0;
	delta += CTime::GetDelta();
	if (delta >= 1.0) // Change the double here if you want to make it update sooner or later in seconds
	{
		delta -= 1.0;
		// update FPS
		hold = 1.0 / CTime::GetDelta();
	}
	m_pd2dContext->SetTarget(m_pd2dBitmap);
	Update();//Have this for later
	if (m_nCurrent == eMenu::GAME) // Draws HUD in-game
	{
		/*m_pd2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pcHud->DrawBaseHUD(m_pd2dContext, m_pd2dRT, m_pd2dColorBrush, m_fWidth, m_fHeight);
		m_pcHud->WriteHealthAndMana(m_pd2dRT, m_pdwHUDFormat, m_pd2dColorBrush3, m_fWidth, m_fHeight);
		if (m_bShowInventory) 
		{ 
			m_pcHud->DrawInventory(m_pd2dContext, m_pd2dRT, m_pdwInventoryFormat, m_pdwTitleFormat, 
				m_pd2dColorBrush, m_pd2dColorBrush2, m_pd2dColorBrush3, m_fWidth, m_fHeight);
		}*/
	}
	else if (m_menus.size() > 0 && m_nCurrent < m_menus.size())
	{
		//// Draw a transparent box to hold the menu items in
		//m_pd2dRT->BeginDraw();
		//m_pd2dRT->FillRectangle(D2D1::RectF(m_fWidth * 0.1f, m_fHeight * 0.05f, m_fWidth * 0.9f, m_fHeight * 0.95f), m_pd2dColorBrush);
		//m_pd2dRT->EndDraw();
		//// Render the menu
		//m_pd2dContext->BeginDraw();
		//// Sets the sprites for all the buttons and sliders the menu contains
		//m_menus[m_nCurrent]->SetSprites();
		//// Draws the sprites
		//m_pd2dContext->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		//m_pd2dContext->DrawSpriteBatch(m_menus[m_nCurrent]->GetSpriteBatch(), m_menus[m_nCurrent]->GetButtonSprite());
		//m_pd2dContext->EndDraw();
		//// Draw Text
		//m_pd2dRT->BeginDraw();
		//// Render title text if it exists
		//const WCHAR* titleText = m_menus[m_nCurrent]->GetTitle();
		//if (titleText)
		//{
		//	m_pd2dRT->DrawTextW(titleText, (UINT32)m_menus[m_nCurrent]->GetTitleLength(),
		//		m_pdwTitleFormat, m_menus[m_nCurrent]->GetTitleBox(), m_pd2dColorBrush2);
		//}
		//size_t size = m_menus[m_nCurrent]->m_pcButtons.size();
		//// Render button text for all buttons
		//for (size_t i = 0; i < size; i++)
		//{
		//	m_pd2dRT->DrawTextW(m_menus[m_nCurrent]->m_pcButtons[i]->GetButtonText(),
		//		(UINT32)m_menus[m_nCurrent]->m_pcButtons[i]->GetTextLength(), m_pdwTextFormat,
		//		m_menus[m_nCurrent]->m_pcButtons[i]->GetButtonBox(), m_pd2dColorBrush2);
		//}
		//size = m_menus[m_nCurrent]->m_pDescriptionText.size();
		//// Render all description text
		//for (size_t i = 0; i < size; i++)
		//{
		//	m_pd2dRT->DrawTextW(m_menus[m_nCurrent]->m_pDescriptionText[i],
		//		(UINT32)m_menus[m_nCurrent]->m_DescriptionLength[i], m_pdwTextFormat,
		//		m_menus[m_nCurrent]->m_DescriptionBoxes[i], m_pd2dColorBrush2);
		//}
		//m_pd2dRT->EndDraw();
	}

	m_pd2dRT->BeginDraw();
	// FPS
	std::wstring framerate;
	framerate += L"FPS: ";
	framerate += std::to_wstring((int)hold);
	m_pd2dRT->DrawTextW(framerate.c_str(), (UINT32)framerate.size(),
		m_pdwHUDFormat, D2D1::RectF(m_fWidth * 0.9f, 0, m_fWidth, 100), m_pd2dColorBrush3);
	m_pd2dRT->EndDraw();

	m_pd2dContext->SetTarget(nullptr); // Allows for resizing if needed
	// I don't know if I need to do this, but it is better to be safe than sorry
}

void CGUIManager::AddButtonToMenu(int menu, void(*pFunction)(), const WCHAR* buttonText, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	if (menu >= m_menus.size())
	{
		while (menu >= m_menus.size())
		{
			CMenu *nMenu = new CMenu(m_pd3dDevice, m_pd2dContext);
			m_menus.push_back(nMenu);
		}
	}
	// Add that thing to the menu
	m_menus[menu]->AddButton(pFunction, buttonText, width, height, wWidth, wHeight, left, right, top, bottom);
}

void CGUIManager::AddSliderToMenu(int menu, int nType, int width, int height, float wWidth, float wHeight, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	if (menu >= m_menus.size())
	{
		while (menu >= m_menus.size())
		{
			CMenu *nMenu = new CMenu(m_pd3dDevice, m_pd2dContext);
			m_menus.push_back(nMenu);
		}
	}
	// Add that thing to the menu
	m_menus[menu]->AddSlider(menu, nType, width, height, wWidth, wHeight, left, right, top, bottom);
}

void CGUIManager::SetTitleToMenu(int menu, const WCHAR * title_text, int width, int height, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	if (menu >= m_menus.size())
	{
		while (menu >= m_menus.size())
		{
			CMenu *nMenu = new CMenu(m_pd3dDevice, m_pd2dContext);
			m_menus.push_back(nMenu);
		}
	}
	// Add that thing to the menu
	m_menus[menu]->MakeTitle(title_text, width, height, left, right, top, bottom);
}

void CGUIManager::AddDescriptionToMenu(int menu, const WCHAR * text, int width, int height, float left, float right, float top, float bottom)
{
	// Create new menu if needed
	if (menu >= m_menus.size())
	{
		while (menu >= m_menus.size())
		{
			CMenu *nMenu = new CMenu(m_pd3dDevice, m_pd2dContext);
			m_menus.push_back(nMenu);
		}
	}
	// Add that thing to the menu
	m_menus[menu]->AddDescriptionText(text, width, height, left, right, top, bottom);
}

void CGUIManager::WindowResizeEvent(float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	// Hold onto new swapchain width and height
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	// Resize all menu items
	for (size_t i = 0; i < m_menus.size(); i++)
	{
		m_menus[i]->ResizeItems(fWidth, fHeight, fwWidth, fwHeight);
	}
	m_pcHud->ResizeItems(fWidth, fHeight, fwWidth, fwHeight);
}

void CGUIManager::ChangeCurrentMenu(int menu)
{
	if (menu < eMenu::COUNT)
	{
		m_nCurrent = menu;
	}
}

void CGUIManager::SetHudMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	m_pcHud->SetMenu(pd3dDevice, pd2dContext, fWidth, fHeight, fwWidth, fwHeight);
}

void CGUIManager::SetPlayerValues(float fCurrHealth, float fMaxHealth, float fCurrMana, float fMaxMana)
{
	m_pcHud->SetPlayerValues(fCurrHealth, fMaxHealth, fCurrMana, fMaxMana);
}

void CGUIManager::SetMiniMap(std::vector<std::vector<Spot>> cMap)
{
	m_pcHud->SetMiniMap(cMap);
}

int CGUIManager::GetMenuState()
{
	return m_nCurrent;
}