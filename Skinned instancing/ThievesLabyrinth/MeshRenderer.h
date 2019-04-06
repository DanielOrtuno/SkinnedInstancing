#pragma once
#include "Component.h"

class CMeshRenderer : public IComponent
{
	int		m_nVertices;
	int		m_nIndices;

	int		m_nTextureCount;
	int*	m_pnTexture;

	int		m_nVS;
	int		m_nPS;

	int		m_nSampler;

	int		m_nInputLayout;

public:
	CMeshRenderer(IEntity* pcOwner);

	/***************
	* Accessors
	***************/

	int GetVertexBuffer();
	int GetIndexBuffer();
	int GetTextureCount();
	int GetTextureAt(int nSlot);
	int GetVertexShader();
	int GetPixelShader();
	int GetSampler();
	int GetInputLayout();

	/***************
	* Mutators
	***************/

	void SetVertexBuffer(int nBuffer);
	void SetIndexBuffer(int nBuffer);
	void SetTextureCount(int nTextureCount);
	void SetTexture(int nTexture, int nSlot);
	void SetVertexShader(int nVS);
	void SetPixelShader(int nPS);
	void SetSampler(int nSampler);
	void SetInputLayout(int nInputLayout);

	CMeshRenderer& operator=(CMeshRenderer& cCopy);

	~CMeshRenderer();
};


