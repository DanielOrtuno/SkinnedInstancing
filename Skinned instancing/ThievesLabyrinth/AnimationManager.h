#pragma once
#include "System.h"
#include <vector>
#include <fstream>
#include <DirectXMath.h>
#include "EnumTypes.h"

#define MAX_JOINTS 32
#define MAX_INSTANCES 1000

using namespace DirectX;

class CAnimatorComponent;
struct ID3D11Device;
struct ID3D11ShaderResourceView;

class CAnimator
{
public:
	
	struct TAnimationClip
	{
		double m_dDuration;
		int	   m_nFrameCount;
	};

	int					m_nAnimationCount;
	TAnimationClip*		m_ptAnimationClips;

	int					m_nJointCount;
	XMMATRIX*			m_ptBindPoseMatrices;

	int					m_nMaxFrameCount;

	ID3D11ShaderResourceView* m_pd3dSRV[2];

	CAnimator();

	~CAnimator();

	void LoadAnimator(const char* pchFilepath);

	void LoadAnimationTexture(ID3D11Device* pcDevice, const wchar_t * pcFilepathA, const wchar_t * pcFilepathB);
};

struct AnimationData
{
	float	 fCurrentTime;
	float	 fTotalTime;
	int		 nCurrentAnimation;
	int		 nFrameCount;
};

struct TAnimationBuffer
{
	/*XMMATRIX m_tBindpose[MAX_JOINTS];
	float	 fCurrentTime;
	float	 fTotalTime;

	int		 nJointCount;
	int		 nCurrentAnimation;
	int		 nFrameCount;*/

	XMMATRIX		mBindpose[MAX_JOINTS];
	AnimationData	tAnimationData[MAX_INSTANCES];
	int				nJointCount;
	XMFLOAT3		nPadding;
};

class CAnimatorManager
{
private:
	int	m_nAnimationBufferCount;
	TAnimationBuffer	m_tMatrixBuffer;

	CAnimator**	m_pcAnimators;

public:
	CAnimatorManager(ID3D11Device* device);

	int SetBindposeMatrices(CAnimatorComponent* pcAnimComponent);

	int ProcessAnimatorComponent(CAnimatorComponent* pcAnimComponent);

	TAnimationBuffer* GetAnimationData();

	ID3D11ShaderResourceView*const* GetAnimationTexture(int nAnimator);

	~CAnimatorManager();
};

