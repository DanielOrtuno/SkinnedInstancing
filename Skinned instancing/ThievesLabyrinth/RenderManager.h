#pragma once
#include "System.h"
#include "EnumTypes.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <fstream>
#include "AnimationManager.h"
#pragma comment(lib, "d3d11.lib")

#define MAX_LIGHTS 16

using namespace DirectX;

class CGUIManager;
class CTransform;
class CAnimatorManager;
class CComponentManager;
class CMeshRenderer;


struct TVertex
{
	XMFLOAT4 vPos;
	XMFLOAT3 vNormal;
	XMFLOAT2 vUV;
};

struct TDebugVertex
{
	XMFLOAT3 vPos;
	XMFLOAT4 vColor;
};

struct TSkinnedVertex
{
	XMFLOAT4 vPos;
	XMFLOAT3 vNormal;
	XMFLOAT2 vUV;
	XMFLOAT4 vJointIndex;
	XMFLOAT4 vWeights;
};

struct TModelConstantBuffer
{
	XMMATRIX mWorldMatrix[MAX_INSTANCES];
};

struct TCameraConstantBuffer
{
	XMMATRIX mViewMatrix;
	XMMATRIX mProjMatrix;
};

struct TPointLight
{
	XMFLOAT4 vLightDesc;
	XMFLOAT4 vColor;
};

struct TLightData
{
	float	vBufferData[3];
	float	vPadding;
	TPointLight tLights[MAX_LIGHTS];
};

struct TLightsConstantBuffer
{
	TLightData tObjectLights[MAX_INSTANCES];
};

class CRenderManager : public ISystem
{
private:
	ID3D11Device*				m_pd3dDevice;
	ID3D11DeviceContext*		m_pd3dDeviceContext;
	static IDXGISwapChain*		m_pd3dSwapChain;
	ID3D11RenderTargetView*		m_pd3dRenderTarget[eRenderTargetView::COUNT];
	ID3D11DepthStencilView*		m_pd3dDepthStencilView[eDepthStencilView::COUNT];
	ID3D11DepthStencilState*	m_pd3dDepthStencilState[eDepthStencilState::COUNT];
	ID3D11RasterizerState*		m_pd3dRasterizerState[eRasterizerState::COUNT];

	int							m_pnVertexCount[eVertexBuffer::COUNT];
	int							m_pnIndexCount[eIndexBuffer::COUNT];

	ID3D11Buffer*				m_pd3dVertexBuffer[eVertexBuffer::COUNT];
	ID3D11Buffer*				m_pd3dIndexBuffer[eIndexBuffer::COUNT];
	ID3D11Buffer*				m_pd3dConstantBuffer[eConstantBuffer::COUNT];

	ID3D11VertexShader*			m_pd3dVertexShader[eVertexShader::COUNT];
	ID3D11PixelShader*			m_pd3dPixelShader[ePixelShader::COUNT];

	ID3D11ShaderResourceView*	m_pd3dSRV[eTexture::COUNT];

	ID3D11InputLayout*			m_pd3dInputLayout[eInputLayout::COUNT];

	ID3D11SamplerState*			m_pd3dSamplers[eSampler::COUNT];

	static D3D11_VIEWPORT		m_pd3dViewport[eViewport::COUNT];

	CComponentManager*			m_pcComponentManager;

	HWND						m_cWindow;

	static CGUIManager*			m_pcGUIManager;
	CAnimatorManager*			m_pcAnimatorManager;


	static TCameraConstantBuffer m_tCameraBuffer;

	XMMATRIX*					m_d3dViewMatrix;
	static XMMATRIX	*			m_d3dProjMatrix;

	void InitializeDirectXComponents();

	int LoadMeshFromFile(const char* pchFilepath, int nType);

	void LoadTextureFromFile(const char* pchFilepath, int ntType);

	void RenderMeshType(std::vector<CMeshRenderer*>& pcMesh);

		//DELETE THIS	
		bool	bIsDissolveShaderEnabled = false;

public:
	CRenderManager(HWND cWindow, CComponentManager* pcComponentManager);

	static void ResizeEvent(float nWidth, float nHeight);

	void Draw();

	void AddButtonToMenu(int menu, void(*pFunction)(), const WCHAR* buttonText, int width, int height, float left = 0.0f, float right = 0.1f, float top = 0.0f, float bottom = 0.1f);
	void AddSliderToMenu(int menu, int nType, int width, int height,
		float left, float right, float top, float bottom);
	void SetTitleToMenu(int menu, const WCHAR* title_text, float left, float right, float top, float bottom);
	void AddDescriptionToMenu(int menu, const WCHAR* text, float left, float right, float top, float bottom);
	void ChangeCurrentMenu(int menu);
	int GetMenuState();

	~CRenderManager();
};

