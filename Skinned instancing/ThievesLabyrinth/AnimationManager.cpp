#include "AnimationManager.h"
#include <iostream>
//#include "EnumTypes.h"
#include "AnimatorComponent.h"
#include "Time.h"
#include "InputManager.h"
#include "DDSTextureLoader.h"
#include "iostream"

CAnimator::CAnimator()
{
	m_nAnimationCount = 0;
	m_nJointCount = 0;
	m_nMaxFrameCount = 0;
	m_ptAnimationClips = nullptr;
	m_ptBindPoseMatrices = nullptr;
}

CAnimator::~CAnimator()
{
	delete[] m_ptBindPoseMatrices;
	delete[] m_ptAnimationClips;

	m_pd3dSRV[0]->Release();
	m_pd3dSRV[1]->Release();

}

void CAnimator::LoadAnimator(const char * pchFilepath)
{
	std::fstream input;
	input.open(pchFilepath, std::ios_base::in | std::ios_base::binary);

	if(input.is_open())
	{
		input.read((char*)&m_nJointCount, sizeof(size_t));
		m_ptBindPoseMatrices = new XMMATRIX[m_nJointCount];
		input.read((char*)m_ptBindPoseMatrices, sizeof(XMMATRIX) * m_nJointCount);
		input.read((char*)&m_nAnimationCount, sizeof(int));

		m_ptAnimationClips = new TAnimationClip[m_nAnimationCount];
		for(int i = 0; i < m_nAnimationCount; i++)
		{
			input.read((char*)&m_ptAnimationClips[i], sizeof(double) + sizeof(int));
		}
	}


	input.close();

	for(int i = 0; i < m_nAnimationCount; i++)
	{
		if(m_nMaxFrameCount < m_ptAnimationClips[i].m_nFrameCount)
		{
			m_nMaxFrameCount = m_ptAnimationClips[i].m_nFrameCount;
		}
	}

}


CAnimatorManager::CAnimatorManager(ID3D11Device* pd3dDevice)
{
	m_nAnimationBufferCount = 0;

	m_pcAnimators = new CAnimator*[eAnimator::COUNT];

	m_pcAnimators[eAnimator::MAGE] = new CAnimator();

	m_pcAnimators[eAnimator::MAGE]->LoadAnimator("..//Assets//Animations//Enemies//Mage//MageData.bin");
	m_pcAnimators[eAnimator::MAGE]->LoadAnimationTexture(pd3dDevice, L"..//Assets//Animations//Enemies//Mage//MagePositionTexture.DDS", 
																	 L"..//Assets//Animations//Enemies//Mage//MageRotationTexture.DDS");

	m_pcAnimators[eAnimator::FIRE_DEMON] = new CAnimator();
	m_pcAnimators[eAnimator::FIRE_DEMON]->LoadAnimator("..//Assets//Animations//Player//FireGolemData.bin");
	m_pcAnimators[eAnimator::FIRE_DEMON]->LoadAnimationTexture(pd3dDevice, L"..//Assets//Animations//Player//FireGolemPositionTexture.DDS",
													   L"..//Assets//Animations//Player//FireGolemRotationTexture.DDS");

}

int CAnimatorManager::SetBindposeMatrices(CAnimatorComponent * pcAnimComponent)
{
	int nAnimator = pcAnimComponent->GetAnimator();

	for(int i = 0; i < m_pcAnimators[nAnimator]->m_nJointCount; i++)
	{
		m_tMatrixBuffer.mBindpose[i] = m_pcAnimators[nAnimator]->m_ptBindPoseMatrices[i];
	}

	return 0;
}

int CAnimatorManager::ProcessAnimatorComponent(CAnimatorComponent * pcAnimComponent)
{
	if(m_nAnimationBufferCount < MAX_INSTANCES)
	{
		int nAnimator = pcAnimComponent->GetAnimator();
		int nAnimation = pcAnimComponent->GetCurrentAnimation();
		double dCurrentTime = pcAnimComponent->GetCurrentAnimationTime();

		if(dCurrentTime < m_pcAnimators[nAnimator]->m_ptAnimationClips[nAnimation].m_dDuration)
		{
			dCurrentTime += CTime::GetDelta() * pcAnimComponent->GetSpeed();
		}
		else
		{
 			dCurrentTime = 0.0f;
		}

		pcAnimComponent->SetAnimationTime(dCurrentTime);

		m_tMatrixBuffer.tAnimationData[m_nAnimationBufferCount].fCurrentTime = (float)dCurrentTime;
		m_tMatrixBuffer.tAnimationData[m_nAnimationBufferCount].fTotalTime = (float)m_pcAnimators[nAnimator]->m_ptAnimationClips[nAnimation].m_dDuration;

		m_tMatrixBuffer.nJointCount = m_pcAnimators[nAnimator]->m_nJointCount;
		m_tMatrixBuffer.tAnimationData[m_nAnimationBufferCount].nCurrentAnimation = nAnimation;
		m_tMatrixBuffer.tAnimationData[m_nAnimationBufferCount].nFrameCount = m_pcAnimators[nAnimator]->m_ptAnimationClips[nAnimation].m_nFrameCount;

		m_nAnimationBufferCount++;

		return 0;
	}

	return 1;
}



void CAnimator::LoadAnimationTexture(ID3D11Device* pcDevice, const wchar_t * pcFilepathA, const wchar_t * pcFilepathB)
{
	HRESULT hr = CreateDDSTextureFromFile(pcDevice, pcFilepathA, ( ID3D11Resource** )nullptr, &m_pd3dSRV[0]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingAnimationTexture", L"Error", MB_OK);
		exit(0);
	}

	hr = CreateDDSTextureFromFile(pcDevice, pcFilepathB, ( ID3D11Resource** )nullptr, &m_pd3dSRV[1]);

	if(FAILED(hr))
	{
		MessageBox(nullptr, L"LoadingAnimationTexture", L"Error", MB_OK);
		exit(0);
	}
}


TAnimationBuffer* CAnimatorManager::GetAnimationData()
{
	m_nAnimationBufferCount = 0;
	return &m_tMatrixBuffer;
}

CAnimatorManager::~CAnimatorManager()
{
	for(int i = 0; i < eAnimator::COUNT; i++)
	{
		delete m_pcAnimators[i];
	}

	delete[] m_pcAnimators;
}
ID3D11ShaderResourceView*const* CAnimatorManager::GetAnimationTexture(int nAnimator)
{
	return m_pcAnimators[nAnimator]->m_pd3dSRV;
}

