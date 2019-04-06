#pragma once
#include "HUD.h"
#include <dxgi1_2.h>
#include <string>
#include "WICTextureLoader.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Component.h"
#include "FireDemonController.h"
#include "Stats.h"
#include "EnumTypes.h"

int CHUD::m_nCurrConsume;
bool CHUD::m_bChange;

void CHUD::UseHealth()
{
	m_nCurrConsume = eConsumableItems::HEALTH;
	m_bChange = true;
}

void CHUD::UseMana()
{
	m_nCurrConsume = eConsumableItems::MANA;
	m_bChange = true;
}

void CHUD::UseDamage()
{
	m_nCurrConsume = eConsumableItems::DAMAGE;
	m_bChange = true;
}

void CHUD::UseHaste()
{
	m_nCurrConsume = eConsumableItems::HASTE;
	m_bChange = true;
}

void CHUD::UseResist()
{
	m_nCurrConsume = eConsumableItems::RESIST;
	m_bChange = true;
}

void CHUD::Nothing()
{
	//Literally does nothing
	//I only added this to make a couple of things easier
}

CHUD::CHUD(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight)
{
	m_fCurrHealth = m_fCurrMana = 0.0f;
	// To avoid errors through dividing by 0
	m_fMaxHealth = m_fMaxMana = 1.0f;
	m_nCurrActive = m_nCurrPass = m_nCurrConsume = 0;
	m_pnConsume = nullptr;
	m_pd2dTempSprite = m_pd2dInventorySprite = nullptr;
	m_pd2dInventoryBatch = nullptr;
	m_pcMenu = nullptr;
	m_bChange = m_bRenderMenu = false;
	pd2dContext->CreateSpriteBatch(&m_pd2dSpriteBatch);
	ID3D11Texture2D* fromDisk = nullptr;
	ID3D11ShaderResourceView* ignored = nullptr;
	//Sprite sheet will use this in the future
	if (FAILED(DirectX::CreateWICTextureFromFile(pd3dDevice, L"../Assets/Textures/UI/Sprite.png", (ID3D11Resource**)&fromDisk, &ignored)))
	{
		MessageBox(nullptr, L"Failed to load texture for buttons", L"Error", MB_OK);
		if (fromDisk)	fromDisk->Release();
		if (ignored)	ignored->Release();
		exit(0);
	}
	IDXGISurface2* texSurface = nullptr;
	fromDisk->QueryInterface(__uuidof(IDXGISurface2), (void**)&texSurface);

	DXGI_SURFACE_DESC sDesc;
	D2D1_BITMAP_PROPERTIES1 bitProp;
	ZeroMemory(&sDesc, sizeof(DXGI_SURFACE_DESC));
	ZeroMemory(&bitProp, sizeof(D2D1_BITMAP_PROPERTIES1));
	texSurface->GetDesc(&sDesc);
	bitProp.pixelFormat = { sDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED };
	bitProp.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
	bitProp.dpiX = bitProp.dpiY = 96.0f; // THIS MUST NOT BE CHANGED!!
	// I don't know why, but not using this specific float can easily break EVERYTHING
	// If you put zero (which *some* functions seem to say is a-ok!) it will draw nothing!!!
	// You also get no errors, I guess it just assumes you are fine with drawing nothing at all.
	pd2dContext->CreateBitmapFromDxgiSurface(texSurface, bitProp, &m_pd2dTempSprite);
	texSurface->Release();
	if (fromDisk) { fromDisk->Release(); fromDisk = nullptr; }
	if (ignored) { ignored->Release();	 ignored = nullptr; }

	D2D1_RECT_F offscreen[6] = { 0, };
	m_pd2dSpriteBatch->AddSprites(6, offscreen);
	m_fCurrTime = 0.0f;
	m_fMaxTime = 1.0f;
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}

void CHUD::Update()
{
	for (size_t i = 0; i < m_pcMenu->m_pcButtons.size(); i++)
	{
		m_pcMenu->m_pcButtons[i]->Update();
	}
	SetPlayerValues();
}

void CHUD::SetPlayerValues(float fCurrHealth, float fMaxHealth, float fCurrMana, float fMaxMana)
{
	IEntity* player = nullptr;
	player = CEntityManager::GetEntity(0);
	bool defaulted = false;
	if (player)
	{
		CStats* stats = player->GetComponent<CStats>();
		m_fCurrHealth = stats->GetHP();
		m_fCurrMana = stats->GetMP();
		m_fMaxTime = stats->GetAttackSpeed();
		CInventory* inventory = player->GetComponent<CInventory>();
		m_nCurrActive = inventory->GetActive();
		m_nCurrPass = inventory->GetPassive();
		CFireDemonController* controller = player->GetComponent<CFireDemonController>();
		m_fCurrTime = controller->GetAttackTime();
		if (m_bChange)
		{
			inventory->SetCurrentConsumable(m_nCurrConsume);
			m_bChange = false;
		}
		// Value may not be what we set it to
		m_nCurrConsume = inventory->GetCurrentConsumable();

		// I'm aware that we don't need to reset this pointer
		// I'm doing this on the off-chance we implement another
		// controller or a new game is started after an old one ends
		inventory->GetConsumables(&m_pnConsume);
	}
}

void CHUD::WriteHealthAndMana(ID2D1RenderTarget* pd2dRT, IDWriteTextFormat* pdxwTextFormat, ID2D1SolidColorBrush* pd2dBrushie, float fWidth, float fHeight)
{
	pd2dRT->BeginDraw();
	std::wstring health, mana;
	health += L"Health: ";
	health += std::to_wstring((int)m_fCurrHealth);
	//health += L" / ";
	//health += std::to_wstring((int)m_fMaxHealth);
	mana += L"Mana: ";
	mana += std::to_wstring((int)m_fCurrMana);
	//mana += L" / ";
	//mana += std::to_wstring((int)m_fMaxMana);

	pd2dRT->DrawTextW(health.c_str(), (UINT32)health.size(), pdxwTextFormat, D2D1::RectF(0, 0, fWidth * 0.15f, fHeight * 0.05f), pd2dBrushie);
	pd2dRT->DrawTextW(mana.c_str(), (UINT32)mana.size(), pdxwTextFormat, D2D1::RectF(0, fHeight * 0.05f, fWidth * 0.15f, fHeight * 0.1f), pd2dBrushie);
	pd2dRT->EndDraw();
}

void CHUD::SetMiniMap(std::vector<std::vector<Spot>> cMap)
{
	m_cMap = cMap;
	ResizeMiniMap(m_fWidth, m_fHeight);
}

void CHUD::ResizeMiniMap(float fWidth, float fHeight)
{
	m_rMap.clear();
	m_rMap.resize(m_cMap.size());
	float Left = fWidth * 0.5f;
	float Right = fWidth * 0.85f;
	float Top = fHeight * 0.1f;
	float Bottom = fHeight * 0.65f;
	for (size_t i = 0; i < m_rMap.size(); i++)
	{
		m_rMap[i].resize(m_cMap[i].size());
	}
	size_t col = m_rMap.size();
	for (size_t j = 0; j < col; j++)
	{
		size_t row = m_rMap[j].size();
		for (size_t i = 0; i < row; i++)
		{
			if (m_cMap[i][j].bRoom)
			{
				m_rMap[i][j] = D2D1::RectF(Left + ((Right - Left) * (i /(float)row)), Top + ((Bottom - Top) * ((col - j) / (float)col)),
					Left + ((Right - Left) * ((i + 1) / (float)row)), Top + ((Bottom - Top) * ((row - (j + 1)) / (float)col)));
			}
			else
			{
				m_rMap[i][j] = D2D1::RectF(0, 0, 0, 0);
			}
		}
	}
}

CHUD::~CHUD()
{
	if (m_pd2dSpriteBatch)	{ m_pd2dSpriteBatch->Release();	m_pd2dSpriteBatch = nullptr; }
	if (m_pd2dTempSprite)	{ m_pd2dTempSprite->Release();	m_pd2dTempSprite = nullptr; }
	if (m_pd2dInventoryBatch) { m_pd2dInventoryBatch->Release();	m_pd2dInventoryBatch = nullptr; }
	if (m_pd2dInventorySprite) { m_pd2dInventorySprite->Release();	m_pd2dInventorySprite = nullptr; }
	if (m_pcMenu)	delete m_pcMenu;
}

void CHUD::DrawBaseHUD(ID2D1DeviceContext3* pd2dContext, ID2D1RenderTarget* pd2dRT, ID2D1SolidColorBrush* pd2dBrushie, float fWidth, float fHeight)
{
	// Set up base for the HUD, and draw it immediately
	std::vector<D2D1_RECT_F> rectangles;

	rectangles.push_back(D2D1::RectF(0, fHeight * 0.8f, fWidth * 0.1f, fHeight));
	rectangles.push_back(D2D1::RectF(fWidth * 0.15f, fHeight * 0.8f, fWidth * 0.25f, fHeight));
	rectangles.push_back(D2D1::RectF(fWidth * 0.3f, fHeight * 0.8f, fWidth * 0.4f, fHeight));
	if (m_nCurrConsume > eConsumableItems::NONE)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.9f, fHeight * 0.8f, fWidth, fHeight));
	}
	else
	{
		rectangles.push_back(D2D1::RectF(0, 0, 0, 0));
	}
	if (m_nCurrPass > eActiveItems::NONE)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.75f, fHeight * 0.8f, fWidth * 0.85f, fHeight));
	}
	else
	{
		rectangles.push_back(D2D1::RectF(0, 0, 0, 0));
	}
	if (m_nCurrConsume > ePassiveItems::NONE)
	{
		rectangles.push_back(D2D1::RectF(fWidth * 0.6f, fHeight * 0.8f, fWidth * 0.7f, fHeight));
	}
	else
	{
		rectangles.push_back(D2D1::RectF(0, 0, 0, 0));
	}
	pd2dContext->BeginDraw();
	m_pd2dSpriteBatch->SetSprites(0, 6, rectangles.data());
	pd2dContext->DrawSpriteBatch(m_pd2dSpriteBatch, m_pd2dTempSprite);
	pd2dContext->EndDraw();

	D2D1_RECT_F rectangle = rectangles[0];
	rectangle.top = rectangle.bottom - ((rectangle.bottom - rectangle.top) * (m_fCurrTime / m_fMaxTime));
	pd2dRT->BeginDraw();
	pd2dRT->FillRectangle(&rectangle, pd2dBrushie);
	pd2dRT->EndDraw();
}

void CHUD::SetMenu(ID3D11Device* pd3dDevice, ID2D1DeviceContext3* pd2dContext, float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	if (!m_pcMenu)
	{
		m_pcMenu = new CMenu(pd3dDevice, pd2dContext);
		m_pcMenu->AddButton(UseHealth, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.15f, 0.25f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseMana, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.3f, 0.4f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseDamage, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.45f, 0.55f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseHaste, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.6f, 0.7f, 0.7f, 0.9f);
		m_pcMenu->AddButton(UseResist, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.75f, 0.85f, 0.7f, 0.9f);
		m_pcMenu->AddButton(Nothing, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.15f, 0.25f, 0.4f, 0.6f);
		m_pcMenu->AddButton(Nothing, L"", (int)fWidth, (int)fHeight, fwWidth, fwHeight, 0.3f, 0.4f, 0.4f, 0.6f);

		pd2dContext->CreateSpriteBatch(&m_pd2dInventoryBatch);
		D2D1_RECT_F offscreen[7] = { 0, };
		m_pd2dInventoryBatch->AddSprites(7, offscreen);
	}
}

void CHUD::DrawInventory(ID2D1DeviceContext3* pd2dContext, ID2D1RenderTarget* pd2dRT, IDWriteTextFormat* pdxwTextFormat, IDWriteTextFormat* pdxwTitleFormat,
	ID2D1SolidColorBrush* pd2dBrushie, ID2D1SolidColorBrush* pd2dBrushie2, ID2D1SolidColorBrush* pd2dBrushie3, float fWidth, float fHeight)
{
	pd2dRT->BeginDraw();
	pd2dRT->FillRectangle(D2D1::RectF(fWidth * 0.1f, fHeight * 0.05f, fWidth * 0.9f, fHeight * 0.95f), pd2dBrushie);
	pd2dRT->DrawTextW(L"Inventory", 9, pdxwTitleFormat, D2D1::RectF(fWidth * 0.1f, fHeight * 0.05f, fWidth * 0.45f, fHeight * 0.35f), pd2dBrushie2);
	pd2dRT->EndDraw();
	pd2dContext->BeginDraw();
	m_pcMenu->SetSprites();
	pd2dContext->DrawSpriteBatch(m_pcMenu->GetSpriteBatch(), m_pcMenu->GetButtonSprite());
	std::vector<D2D1_RECT_F> rectangular;
	if (m_pnConsume)
	{
		if (m_pnConsume[1] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[0]->GetButtonBox());
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		}
		if (m_pnConsume[2] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[1]->GetButtonBox());
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		}
		if (m_pnConsume[3] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[2]->GetButtonBox());
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		}
		if (m_pnConsume[4] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[3]->GetButtonBox());
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		}
		if (m_pnConsume[5] > 0) {
			rectangular.push_back(m_pcMenu->m_pcButtons[4]->GetButtonBox());
		}
		else {
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		}
	}
	else
	{
		for (size_t i = 0; i < 5; i++)
		{
			rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
		}
	}
	if (m_nCurrActive > eActiveItems::NONE) {
		rectangular.push_back(m_pcMenu->m_pcButtons[5]->GetButtonBox());
	}
	else {
		rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
	}
	if (m_nCurrPass > ePassiveItems::NONE) {
		rectangular.push_back(m_pcMenu->m_pcButtons[6]->GetButtonBox());
	}
	else {
		rectangular.push_back(D2D1::RectF(0, 0, 0, 0));
	}

	// TODO: after the spritesheet is created
	// pull the locations for each item appropriately
	// Also set the colors for consumable items appropriately
	m_pd2dInventoryBatch->SetSprites(0, (UINT32)rectangular.size(), rectangular.data());
	pd2dContext->DrawSpriteBatch(m_pd2dInventoryBatch, m_pd2dTempSprite);
	pd2dContext->EndDraw();
	pd2dRT->BeginDraw();
	std::wstring number;
	if (m_pnConsume)
	{
		for (size_t i = 0; i < 5; i++)
		{
			number.clear();
			number += std::to_wstring(m_pnConsume[i + 1]);
			pd2dRT->DrawTextW(number.c_str(), (UINT32)number.length(), pdxwTextFormat, m_pcMenu->m_pcButtons[i]->GetButtonBox(), pd2dBrushie3);
		}
	}
	else
	{
		number += std::to_wstring(0);
		for (size_t i = 0; i < 5; i++)
		{
			pd2dRT->DrawTextW(number.c_str(), 1, pdxwTextFormat, m_pcMenu->m_pcButtons[i]->GetButtonBox(), pd2dBrushie3);
		}
	}
	size_t col = m_rMap.size();
	for (size_t j = 0; j < col; j++)
	{
		size_t row = m_rMap[j].size();
		for (size_t i = 0; i < row; i++)
		{
			if (CLevel::m_pcCurrentRoom == m_cMap[j][i].Room)
			{
				pd2dRT->FillRectangle(m_rMap[j][i], pd2dBrushie2);
			}
			else
			{
				pd2dRT->FillRectangle(m_rMap[j][i], pd2dBrushie3);
			}
		}
	}
	pd2dRT->EndDraw();
}

void CHUD::ResizeItems(float fWidth, float fHeight, float fwWidth, float fwHeight)
{
	m_pcMenu->ResizeItems(fWidth, fHeight, fwWidth, fwHeight);
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	ResizeMiniMap(fWidth, fHeight);
}
