#include "RenderManager.h"
#include "Entity.h"
#include "DebugManager.h"
#include "GUIManager.h"
#include "ComponentManager.h"
#include "MeshRenderer.h"
#include "EventManager.h"

#include "WICTextureLoader.h"
#include "CameraController.h"

#include "Transform.h"
#include "InputManager.h"
#include "AnimatorComponent.h"
//#include "EnumTypes.h"

#include "DefaultVS.csh"
#include "DefaultPS.csh"
#include "DebugVS.csh"
#include "DebugPS.csh"
#include "SkinnedVS.csh"

#define SAFE_RELEASE(ptr) { if(ptr) { ptr->Release(); ptr = nullptr; } }


XMMATRIX* CRenderManager::m_d3dProjMatrix;
D3D11_VIEWPORT CRenderManager::m_pd3dViewport[eViewport::COUNT];
CGUIManager* CRenderManager::m_pcGUIManager;
IDXGISwapChain*	CRenderManager::m_pd3dSwapChain;
TCameraConstantBuffer CRenderManager::m_tCameraBuffer;


CRenderManager::CRenderManager(HWND cWindow, CComponentManager * pcComponentManager)
{
	m_pcComponentManager = pcComponentManager;
	m_cWindow = cWindow;

	//FIX

	m_d3dViewMatrix = new XMMATRIX;
	*m_d3dViewMatrix = XMMatrixRotationX(XMConvertToRadians(45));
	m_d3dProjMatrix = new XMMATRIX;
	*m_d3dProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65), 1024 / 576.0f, 0.1f, 1000.0f);

	CInputManager::SetProjMat(*(CMath::TMATRIX*)m_d3dProjMatrix);


	InitializeDirectXComponents();
}

void CRenderManager::InitializeDirectXComponents()
{

	#pragma region Device and Swap Chain

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Width = 1024;
	sd.BufferDesc.Height = 576;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_cWindow;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;


	#if _DEBUG
		sd.Flags = D3D11_CREATE_DEVICE_DEBUG;


	#endif

	//Use IDXGISwapChain::SetFullscreenState to change to Fullscreen mode.

	D3D_FEATURE_LEVEL  featureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
	UINT               numLevelsRequested = 1;
	D3D_FEATURE_LEVEL  featureLevelsSupported;
	D3D11_CREATE_DEVICE_FLAG deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;


	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, &featureLevelsRequested,
									numLevelsRequested, D3D11_SDK_VERSION, &sd, &m_pd3dSwapChain, &m_pd3dDevice,
									&featureLevelsSupported, &m_pd3dDeviceContext);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"D3D11CreateDeviceAndSwapChain", L"Error", MB_OK);
		exit(0);
	}

#pragma endregion

	#pragma region Rasterizer State

	m_pd3dViewport[eViewport::DEFAULT].Width = 1024;
	m_pd3dViewport[eViewport::DEFAULT].Height = 576;
	m_pd3dViewport[eViewport::DEFAULT].MinDepth = 0.0f;
	m_pd3dViewport[eViewport::DEFAULT].MaxDepth = 1.0f;
	m_pd3dViewport[eViewport::DEFAULT].TopLeftX = 0;
	m_pd3dViewport[eViewport::DEFAULT].TopLeftY = 0;

	m_pd3dDeviceContext->RSSetViewports(1, &m_pd3dViewport[eViewport::DEFAULT]);

	D3D11_RASTERIZER_DESC rd;

	rd.FillMode = D3D11_FILL_SOLID;
	rd.CullMode = D3D11_CULL_FRONT;
	rd.FrontCounterClockwise = true;
	rd.DepthBias = false;
	rd.DepthBiasClamp = 0;
	rd.SlopeScaledDepthBias = 0;
	rd.DepthClipEnable = true;
	rd.ScissorEnable = false;
	rd.MultisampleEnable = false;
	rd.AntialiasedLineEnable = true;

	hr = m_pd3dDevice->CreateRasterizerState(&rd, &m_pd3dRasterizerState[eRasterizerState::DEFAULT]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"D3D11CreateRasterizerState", L"Error", MB_OK);
		exit(0);
	}

	m_pd3dDeviceContext->RSSetState(m_pd3dRasterizerState[eRasterizerState::DEFAULT]);

	#pragma endregion

	#pragma region Depth Stencil

	ID3D11Texture2D* pd3dDepthStencil = nullptr;
	D3D11_TEXTURE2D_DESC depthDesc;
	depthDesc.Width = 1024;
	depthDesc.Height = 576;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT; 
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	hr = m_pd3dDevice->CreateTexture2D(&depthDesc, nullptr, &pd3dDepthStencil);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"CreateTexture2D", L"Error", MB_OK);
		exit(0);
	}


	D3D11_DEPTH_STENCIL_DESC dsDesc = { 0 };
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = m_pd3dDevice->CreateDepthStencilState(&dsDesc, &m_pd3dDepthStencilState[eDepthStencilState::DEFAULT]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"CreateDepthStencilState", L"Error", MB_OK);
		exit(0);
	}

	m_pd3dDeviceContext->OMSetDepthStencilState(m_pd3dDepthStencilState[eDepthStencilState::DEFAULT], 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Flags = 0;
	descDSV.Texture2D.MipSlice = 0;

	hr = m_pd3dDevice->CreateDepthStencilView(pd3dDepthStencil, &descDSV, &m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"CreateDepthStencilView", L"Error", MB_OK);
		exit(0);
	}

	pd3dDepthStencil->Release();

	#pragma endregion

	#pragma region Render Target View

	ID3D11Texture2D* pd3dBackBuffer;
	hr = m_pd3dSwapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pd3dBackBuffer);

	hr = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTarget[eRenderTargetView::DEFAULT]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"D3D11CreateRenderTargetView", L"Error", MB_OK);
		exit(0);
	}

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTarget[eRenderTargetView::DEFAULT], m_pd3dDepthStencilView[eDepthStencilView::DEFAULT]);

	pd3dBackBuffer->Release();

	#pragma endregion

	#pragma region Samplers

	D3D11_SAMPLER_DESC samplerDesc{ };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::WRAP]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"CreateSampler", L"Error", MB_OK);
		exit(0);
	}

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	hr = m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::ANIMATION_SAMPLER]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"CreateSampler", L"Error", MB_OK);
		exit(0);
	}
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::CLAMP]);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	hr = m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::BORDER]);

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	hr = m_pd3dDevice->CreateSamplerState(&samplerDesc, &m_pd3dSamplers[eSampler::MIRROR]);

	#pragma endregion

	#pragma region Buffers

	LoadMeshFromFile("..//Assets//Meshes//Player//Minotaur.bin", eVertexBuffer::MINOTAUR);
	LoadMeshFromFile("..//Assets//Meshes//Player//FireGolem.bin", eVertexBuffer::FIRE_DEMON);
	LoadMeshFromFile("..//Assets//Meshes//Misc//Projectile.bin", eVertexBuffer::SPHERE);
	LoadMeshFromFile("..//Assets//Meshes//Enemies//BattleMage.bin", eVertexBuffer::ENEMY_MAGE);
	LoadMeshFromFile("..//Assets//Meshes//Environment//Level.bin", eVertexBuffer::ROOM);
	LoadMeshFromFile("..//Assets//Meshes//Environment//Level1.bin", eVertexBuffer::ROOM1);
	LoadMeshFromFile("..//Assets//Meshes//Environment//Level2.bin", eVertexBuffer::ROOM2);
	LoadMeshFromFile("..//Assets//Meshes//Environment//Level3.bin", eVertexBuffer::ROOM3);
	LoadMeshFromFile("..//Assets//Meshes//Environment//Door.bin", eVertexBuffer::DOOR);
	LoadMeshFromFile("..//Assets//Meshes//Environment//Wall.bin", eVertexBuffer::ENVIRONMENT);
	LoadMeshFromFile("..//Assets//Meshes//Environment//Pillar.bin", eVertexBuffer::PILLAR);



	D3D11_BUFFER_DESC bDesc = { 0 };
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.ByteWidth = sizeof(TDebugVertex) * 100024;	

	hr = m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES]);

	bDesc.ByteWidth = sizeof(TModelConstantBuffer);
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::MODEL]);

	bDesc.ByteWidth = sizeof(TCameraConstantBuffer);
	hr = m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::CAMERA]);

	bDesc.ByteWidth = sizeof(TAnimationBuffer);
	hr = m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES]);

	bDesc.ByteWidth = sizeof(TLightsConstantBuffer);
	hr = m_pd3dDevice->CreateBuffer(&bDesc, nullptr, &m_pd3dConstantBuffer[eConstantBuffer::LIGHTS]);

	#pragma endregion

	#pragma region SRV

	LoadTextureFromFile("..//Assets//Textures//Enemies//PPG_3D_Player_D.png", eTexture::ENEMY_MAGE_DIFFUSE);
	LoadTextureFromFile("..//Assets//Textures//Player//MinotaurDiffuse.png", eTexture::MINOTAUR_DIFFUSE); 
	LoadTextureFromFile("..//Assets//Textures//Player//FireGolemDiffuse.png", eTexture::FIRE_DEMON_DIFFUSE);
	LoadTextureFromFile("..//Assets//Textures//Environment//GreayBox.png", eTexture::ROOM_DIFFUSE);
	LoadTextureFromFile("..//Assets//Textures//Environment//GrayBox1.png", eTexture::ROOM1);
	LoadTextureFromFile("..//Assets//Textures//Environment//GrayBox2.png", eTexture::ROOM2);
	LoadTextureFromFile("..//Assets//Textures//Environment//GrayBox3.png", eTexture::ROOM3);
	LoadTextureFromFile("..//Assets//Textures//Utility//NoiseTexture.jpg", eTexture::NOISE);
	LoadTextureFromFile("..//Assets//Textures//Environment//LR_Diff_Door.png", eTexture::DOOR);
	LoadTextureFromFile("..//Assets//Textures//Environment//Wall_Center_X.png", eTexture::ENVIRONMENT);
	LoadTextureFromFile("..//Assets//Textures//Environment//pillar_col.tga.png", eTexture::PILLAR);


	#pragma endregion

	#pragma region Shaders
	
	hr = m_pd3dDevice->CreateVertexShader(DefaultVS, sizeof(DefaultVS), nullptr, &m_pd3dVertexShader[eVertexShader::DEFAULT]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingVS", L"Error", MB_OK);
		exit(0);
	}


	hr = m_pd3dDevice->CreatePixelShader(DefaultPS, sizeof(DefaultPS), nullptr, &m_pd3dPixelShader[ePixelShader::DEFAULT]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingPS", L"Error", MB_OK);
		exit(0);
	}

	hr = m_pd3dDevice->CreateVertexShader(DebugVS, sizeof(DebugVS), nullptr, &m_pd3dVertexShader[eVertexShader::DEBUG_SHADER]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingVS", L"Error", MB_OK);
		exit(0);
	}

	hr = m_pd3dDevice->CreatePixelShader(DebugPS, sizeof(DebugPS), nullptr, &m_pd3dPixelShader[ePixelShader::DEBUG_SHADER]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingPS", L"Error", MB_OK);
		exit(0);
	}

	hr = m_pd3dDevice->CreateVertexShader(SkinnedVS, sizeof(SkinnedVS), nullptr, &m_pd3dVertexShader[eVertexShader::SKINNED]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingVS", L"Error", MB_OK);
		exit(0);
	}

	#pragma endregion

	#pragma region Input Layout
	
	D3D11_INPUT_ELEMENT_DESC inputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = m_pd3dDevice->CreateInputLayout(inputDesc, 3, DefaultVS, sizeof(DefaultVS), &m_pd3dInputLayout[eInputLayout::DEFAULT]);
	if(FAILED(hr))
	{
		MessageBox(nullptr, L"CreateInputLayout", L"Error", MB_OK);
		exit(0);
	}

	D3D11_INPUT_ELEMENT_DESC debugInputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	
	hr = m_pd3dDevice->CreateInputLayout(debugInputDesc, 2, DebugVS, sizeof(DebugVS), &m_pd3dInputLayout[eInputLayout::DEBUG]);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"CreateInputLayout", L"Error", MB_OK);
		exit(0);
	}

	D3D11_INPUT_ELEMENT_DESC AnimationinputDesc[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXTURECOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "JOINT_INDEX" , 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0 , D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	hr = m_pd3dDevice->CreateInputLayout(AnimationinputDesc, 5, SkinnedVS, sizeof(SkinnedVS), &m_pd3dInputLayout[eInputLayout::SKINNED]);
	if (FAILED(hr))
	{
		MessageBox(nullptr, L"CreateAnimationInputLayout", L"Error", MB_OK);
		exit(0);
	}


	#pragma endregion
	
	#pragma region Subsystems

	m_pcGUIManager = new CGUIManager(m_pd3dDevice, m_pd3dSwapChain, m_cWindow);
	m_pcAnimatorManager = new CAnimatorManager(m_pd3dDevice);

	#pragma endregion
}

int CRenderManager::LoadMeshFromFile(const char * pchFilepath, int nType)
{
	std::fstream input;

	input.open(pchFilepath, std::ios_base::in | std::ios_base::binary);

	int nVertCount = 0;
	int nIndexCount = 0;;
	int* pnIndices;

	switch (nType)
	{
		//Animated
		case eVertexBuffer::ENEMY_MAGE:
		case eVertexBuffer::FIRE_DEMON:
		{
			
			TSkinnedVertex* ptVertices;

			if (input.is_open())
			{
				input.read((char*)&nVertCount, sizeof(int));

				
				ptVertices = new TSkinnedVertex[nVertCount];
				input.read((char*)ptVertices, sizeof(TSkinnedVertex) * nVertCount);

				input.read((char*)&nIndexCount, sizeof(int));

				pnIndices = new int[nIndexCount];
				input.read((char*)pnIndices, sizeof(int) * nIndexCount);

				input.close();

				HRESULT hr;

				D3D11_BUFFER_DESC bDesc = { 0 };
				bDesc.Usage = D3D11_USAGE_DYNAMIC;
				bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				
				bDesc.ByteWidth = sizeof(TSkinnedVertex) * nVertCount;
				bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = ptVertices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dVertexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateVertexBuffer", L"Error", MB_OK);
					exit(0);
				}

				bDesc.ByteWidth = sizeof(int) * nIndexCount;
				bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				data.pSysMem = pnIndices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dIndexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateIndexBuffer", L"Error", MB_OK);
					exit(0);
				}

				delete[] ptVertices;
				delete[] pnIndices;  


				m_pnVertexCount[nType] = nVertCount;
				m_pnIndexCount[nType] = nIndexCount;

				return 0;
			}
			else
			{
				return -1;
			}
		}

		//Static Meshes
		default:
		{
			if (input.is_open())
			{
				
				TVertex* ptVertices;

				input.read((char*)&nVertCount, sizeof(int));

				
				ptVertices = new TVertex[nVertCount];
				input.read((char*)ptVertices, sizeof(TVertex) * nVertCount);

				input.read((char*)&nIndexCount, sizeof(int));
				pnIndices = new int[nIndexCount];
				input.read((char*)pnIndices, sizeof(int) * nIndexCount);

				input.close();

				HRESULT hr;

				D3D11_BUFFER_DESC bDesc = { 0 };
				bDesc.Usage = D3D11_USAGE_DYNAMIC;
				bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				
				bDesc.ByteWidth = sizeof(TVertex) * nVertCount;
				bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				D3D11_SUBRESOURCE_DATA data = { 0 };
				data.pSysMem = ptVertices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dVertexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateVertexBuffer", L"Error", MB_OK);
					exit(0);
				}

				bDesc.ByteWidth = sizeof(int) * nIndexCount;
				bDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
				data.pSysMem = pnIndices;

				hr = m_pd3dDevice->CreateBuffer(&bDesc, &data, &m_pd3dIndexBuffer[nType]);
				if (FAILED(hr))
				{
					MessageBox(nullptr, L"CreateIndexBuffer", L"Error", MB_OK);
					exit(0);
				}

				delete[] ptVertices;
				delete[] pnIndices;

				m_pnVertexCount[nType] = nVertCount;
				m_pnIndexCount[nType] = nIndexCount;

				return 0;
			}
			else
			{
				return -1;
			}

		}
	}

	
}

void CRenderManager::LoadTextureFromFile(const char * pchFilepath, int ntType)
{
	std::wstring temp(&pchFilepath[0], &pchFilepath[strlen(pchFilepath)]);

	HRESULT hr = CreateWICTextureFromFile(m_pd3dDevice, temp.c_str(), nullptr, &m_pd3dSRV[ntType]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"TextureLoadingError", L"Error", MB_OK);
		exit(0);
	}
}

void CRenderManager::RenderMeshType(std::vector<CMeshRenderer*>& cMeshList)
{
	UINT strides;
	UINT offsets = 0;
	
	D3D11_MAPPED_SUBRESOURCE data = { 0 };

	#pragma region	Animations

	CAnimatorComponent* pcEntityAnimator = cMeshList[0]->m_pcOwner->GetComponent<CAnimatorComponent>();
	if(pcEntityAnimator != nullptr)
	{
		strides = sizeof(TSkinnedVertex);

		m_pcAnimatorManager->SetBindposeMatrices(pcEntityAnimator);

		for(CMeshRenderer* cCurrMesh : cMeshList)
		{
			m_pcAnimatorManager->ProcessAnimatorComponent(cCurrMesh->m_pcOwner->GetComponent<CAnimatorComponent>());
		}
		
		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(TAnimationBuffer), m_pcAnimatorManager->GetAnimationData(), sizeof(TAnimationBuffer));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES], 0);

		m_pd3dDeviceContext->VSSetConstantBuffers(2, 1, &m_pd3dConstantBuffer[eConstantBuffer::JOINT_MATRICES]);
		m_pd3dDeviceContext->VSSetSamplers(0, 1, &m_pd3dSamplers[eSampler::ANIMATION_SAMPLER]);
		m_pd3dDeviceContext->VSSetShaderResources(0, 2, m_pcAnimatorManager->GetAnimationTexture(pcEntityAnimator->GetAnimator()));
	}
	else
	{
		strides = sizeof(TVertex);
	}

	#pragma endregion

	#pragma region MVP

	TModelConstantBuffer TModelBuff;

	for(int i = 0; i < cMeshList.size(); i++)
	{
		CTransform* pcEntityTransform = cMeshList[i]->m_pcOwner->GetComponent<CTransform>();
		TModelBuff.mWorldMatrix[i] = XMMATRIX(pcEntityTransform->GetMatrix().mData);
	}
	
	m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	memcpy_s(data.pData, sizeof(TModelConstantBuffer), &TModelBuff, sizeof(TModelConstantBuffer));
	m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0);
	m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::MODEL]);

	#pragma endregion

	//#pragma region Lights

	//std::vector<CLightComponent*> ptRoomLights;	
	//TLightsConstantBuffer tLightsBuffer;

	//for(int i = 0; i < cMeshList.size(); i++)
	//{
	//	if(cMeshList[i]->m_pcOwner->m_nEntityType == eEntity::ROOM)
	//	{
	//		ptRoomLights = ((CRoomEntity*)cMeshList[i]->m_pcOwner)->GetLights();
	//	}
	//	else if(cMeshList[i]->m_pcOwner->m_pcRoom)
	//	{
	//		ptRoomLights = cMeshList[i]->m_pcOwner->m_pcRoom->GetLights();
	//	}

	//	tLightsBuffer.tObjectLights[i].vBufferData[0] = ptRoomLights.size();	//Light count
	//	tLightsBuffer.tObjectLights[i].vBufferData[1] = 0.15f;					//Ambient Light
	//	tLightsBuffer.tObjectLights[i].vBufferData[2] = cMeshList[i]->m_pcOwner->m_nEntityId != 0 ? 0.0f : 1.0f;

	//	//REMOVE THIS
	//	if(cMeshList[i]->m_pcOwner->m_nEntityType == eEntity::ROOM)
	//		tLightsBuffer.tObjectLights[i].vPadding = bIsDissolveShaderEnabled ? 1.0f : 0.0f;
	//	else
	//		tLightsBuffer.tObjectLights[i].vPadding = 0;

	//	if(tLightsBuffer.tObjectLights[i].vBufferData[0] > 0)
	//	{
	//		TPointLight tLight;

	//		for(int j = 0; j < MAX_LIGHTS - ( MAX_LIGHTS - tLightsBuffer.tObjectLights[i].vBufferData[0] ); j++)
	//		{
	//			if(ptRoomLights[j]->IsActive())
	//			{
	//				tLight.vColor = *(XMFLOAT4*)( &ptRoomLights[j]->GetColor() );
	//				tLight.vLightDesc = *(XMFLOAT4*)( &ptRoomLights[j]->GetWorldPosition() );
	//				tLight.vLightDesc.w = ptRoomLights[j]->GetRadius();
	//				tLightsBuffer.tObjectLights[i].tLights[j] = tLight;
	//			}
	//		}

	//		//REMOVE THIS
	//		if(cMeshList[i]->m_pcOwner->m_nEntityType == eEntity::ROOM)
	//			tLightsBuffer.tObjectLights[i].vPadding = bIsDissolveShaderEnabled ? 1 : 0;
	//		else
	//			tLightsBuffer.tObjectLights[i].vPadding = 0;
	//	}
	//}


	//m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::LIGHTS], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
	//memcpy_s(data.pData, sizeof(TLightsConstantBuffer), &tLightsBuffer, sizeof(TLightsConstantBuffer));
	//m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::LIGHTS], 0);
	//m_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::LIGHTS]);

	//#pragma endregion

	m_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer[cMeshList[0]->GetVertexBuffer()], &strides, &offsets);
	m_pd3dDeviceContext->IASetIndexBuffer(m_pd3dIndexBuffer[cMeshList[0]->GetIndexBuffer()], DXGI_FORMAT_R32_UINT, 0);
	m_pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout[cMeshList[0]->GetInputLayout()]);
	m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[cMeshList[0]->GetVertexShader()], nullptr, 0);

	m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[cMeshList[0]->GetPixelShader()], nullptr, 0);
	m_pd3dDeviceContext->PSSetShaderResources(0, 1, &m_pd3dSRV[cMeshList[0]->GetTextureAt(0)]);
	m_pd3dDeviceContext->PSSetSamplers(0, 1, &m_pd3dSamplers[cMeshList[0]->GetSampler()]);

	m_pd3dDeviceContext->DrawIndexedInstanced(m_pnIndexCount[cMeshList[0]->GetIndexBuffer()], (UINT)cMeshList.size(), 0, 0, 0);
}

void CRenderManager::Draw()
{
	if(GetAsyncKeyState('B') & 0x01)
	{
		bIsDissolveShaderEnabled = !bIsDissolveShaderEnabled;
	}

	std::vector<CCameraController*> pcCameras = m_pcComponentManager->GetCameraController();

	const float color[] = { 0.03f, 0.03f, 0.04f, 1.0f };
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTarget[eRenderTargetView::DEFAULT], color);
	m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView[eDepthStencilView::DEFAULT], D3D11_CLEAR_DEPTH, 1.0f, 0);
	if (pcCameras.size() > 0)
	{
		*m_d3dViewMatrix = XMMatrixInverse(nullptr, XMMATRIX(pcCameras[0]->m_pcOwner->GetComponent<CTransform>()->GetMatrix().mData));

		m_tCameraBuffer.mViewMatrix = *m_d3dViewMatrix;
		m_tCameraBuffer.mProjMatrix = *m_d3dProjMatrix;

		D3D11_MAPPED_SUBRESOURCE data = { 0 };
		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::CAMERA], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(m_tCameraBuffer), &m_tCameraBuffer, sizeof(m_tCameraBuffer));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::CAMERA], 0);
		m_pd3dDeviceContext->VSSetConstantBuffers(1, 1, &m_pd3dConstantBuffer[eConstantBuffer::CAMERA]);

		std::vector<CMeshRenderer*> meshes = m_pcComponentManager->GetMeshes();

		std::vector<CMeshRenderer*> meshesByType[eVertexBuffer::COUNT];

		for (CMeshRenderer* mesh : meshes)
		{
			if (mesh->IsActive())
			{
				meshesByType[mesh->GetVertexBuffer()].push_back(mesh);
			}
		}

		for(int i = 0; i < eVertexBuffer::COUNT; i++)
		{
			if(meshesByType[i].size() > 0)
			{
				RenderMeshType(meshesByType[i]);
			}
		}


		//Debug lines
		m_pd3dDeviceContext->Map(m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);

		memcpy(data.pData, CDebugManager::GetDebugLineVertices(), sizeof(CDebugManager::TDebugVertex) * CDebugManager::GetDebugVertexCount());
		m_pd3dDeviceContext->Unmap(m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES], 0);

		TModelConstantBuffer buff;
		buff.mWorldMatrix[0] = XMMatrixIdentity();

		m_pd3dDeviceContext->Map(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0, D3D11_MAP_WRITE_DISCARD, 0, &data);
		memcpy_s(data.pData, sizeof(buff), &buff, sizeof(buff));
		m_pd3dDeviceContext->Unmap(m_pd3dConstantBuffer[eConstantBuffer::MODEL], 0);


		UINT strides = sizeof(CDebugManager::TDebugVertex);
		UINT offsets = 0;
		m_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_pd3dVertexBuffer[eVertexBuffer::DEBUG_LINES], &strides, &offsets);
		m_pd3dDeviceContext->IASetInputLayout(m_pd3dInputLayout[eInputLayout::DEBUG]);
		m_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &m_pd3dConstantBuffer[eConstantBuffer::MODEL]);

		m_pd3dDeviceContext->VSSetShader(m_pd3dVertexShader[eVertexShader::DEBUG_SHADER], 0, 0);
		m_pd3dDeviceContext->PSSetShader(m_pd3dPixelShader[ePixelShader::DEBUG_SHADER], 0, 0);
		m_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		m_pd3dDeviceContext->Draw(CDebugManager::GetDebugVertexCount(), 0);
	}

	if (m_pcGUIManager)	m_pcGUIManager->Render();

	m_pd3dSwapChain->Present(0, 0);
}

void CRenderManager::ResizeEvent(float fWidth, float fHeight)
{
	m_pd3dViewport[eViewport::DEFAULT].Width = fWidth;
	m_pd3dViewport[eViewport::DEFAULT].Height = fHeight;

#if _DEBUG
	m_pd3dSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, D3D11_CREATE_DEVICE_DEBUG);
#else
	m_pd3dSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
#endif

	*m_d3dProjMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(65), fWidth / fHeight , 0.1f, 1000.0f);
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	
	m_pcGUIManager->WindowResizeEvent((float)sd.BufferDesc.Width, (float)sd.BufferDesc.Height, fWidth, fHeight);
}

CRenderManager::~CRenderManager()
{
	SAFE_RELEASE(m_pd3dDevice);

	SAFE_RELEASE(m_pd3dDeviceContext);

	SAFE_RELEASE(m_pd3dSwapChain);

	for(int i = 0; i < eRenderTargetView::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dRenderTarget[i]);
	}

	for(int i = 0; i < eDepthStencilView::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dDepthStencilView[i]);
	}

	for(int i = 0; i < eDepthStencilState::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dDepthStencilState[i]);
	}

	for(int i = 0; i < eRasterizerState::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dRasterizerState[i]);
	}

	for(int i = 0; i < eVertexBuffer::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dVertexBuffer[i]);
	}

	for(int i = 0; i < eIndexBuffer::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dIndexBuffer[i]);
	}

	for(int i = 0; i < eConstantBuffer::COUNT; i++)
	{
		//SAFE_RELEASE(m_pd3dConstantBuffer[i]);
	}

	for(int i = 0; i < eVertexShader::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dVertexShader[i]);
	}

	for(int i = 0; i < ePixelShader::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dPixelShader[i]);
	}

	for(int i = 0; i < eTexture::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dSRV[i]);
	}

	for(int i = 0; i < eInputLayout::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dInputLayout[i]);
	}

	for(int i = 0; i < eSampler::COUNT; i++)
	{
		SAFE_RELEASE(m_pd3dSamplers[i]);
	}

	delete m_d3dViewMatrix;
	delete m_d3dProjMatrix;
	delete m_pcGUIManager;
	delete m_pcAnimatorManager;
}




void CRenderManager::AddButtonToMenu(int menu, void(*pFunction)(), const WCHAR* buttonText, int width, int height, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	
	m_pcGUIManager->AddButtonToMenu(menu, pFunction, buttonText, sd.BufferDesc.Width, sd.BufferDesc.Height, (float)width, (float)height, left, right, top, bottom);
}

void CRenderManager::AddSliderToMenu(int menu, int nType, int width, int height, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	m_pcGUIManager->AddSliderToMenu(menu, nType, sd.BufferDesc.Width, sd.BufferDesc.Height, (float)width, (float)height, left, right, top, bottom);
}

void CRenderManager::SetTitleToMenu(int menu, const WCHAR* title_text, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	m_pcGUIManager->SetTitleToMenu(menu, title_text, sd.BufferDesc.Width, sd.BufferDesc.Height, left, right, top, bottom);
}

void CRenderManager::AddDescriptionToMenu(int menu, const WCHAR * text, float left, float right, float top, float bottom)
{
	DXGI_SWAP_CHAIN_DESC sd;
	m_pd3dSwapChain->GetDesc(&sd);
	m_pcGUIManager->AddDescriptionToMenu(menu, text, sd.BufferDesc.Width, sd.BufferDesc.Height, left, right, top, bottom);
}

void CRenderManager::ChangeCurrentMenu(int menu)
{
	m_pcGUIManager->ChangeCurrentMenu(menu);
}

int CRenderManager::GetMenuState()
{
	return m_pcGUIManager->GetMenuState();
}
