#include "LevelManager.h"
#include "EventManager.h"

#include "AnimatorComponent.h"
#include "MeshRenderer.h"
#include "CapsuleCollider.h"
#include "BoxCollider.h"
#include "Entity.h"
#include "Stats.h"
#include "Transform.h"

#include <vector>

CLevel* CLevelManager::m_pcCurrentLevel;
std::vector<CRoomEntity*> CLevelManager::m_pRoomsType;

CLevelManager::CLevelManager()
{
	
}

void CLevelManager::CreateLevel()
{
	m_pRoomsType.clear();

#pragma region Walls

	CDoorEntity* Door = (CDoorEntity*)CEntityManager::CreateEntity(eEntity::DOOR);
	CEntityManager::AddComponentToEntity(Door, eComponent::TRANSFORM);
	
	CMeshRenderer* pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Door, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::DOOR);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::DOOR);
	pcMeshRender->SetTextureCount(1);
	pcMeshRender->SetTexture(eTexture::DOOR, 0);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);
	
	
	CEntityManager::AddComponentToEntity(Door, eComponent::BOX_COLLIDER);

	CEnvironmentEntity* wall = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(wall, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(wall, eComponent::BOX_COLLIDER);

	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(wall, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::ENVIRONMENT);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::ENVIRONMENT);
	pcMeshRender->SetTextureCount(1);
	pcMeshRender->SetTexture(eTexture::ENVIRONMENT, 0);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);

	// Pillar
	CEnvironmentEntity* Pillar = (CEnvironmentEntity*)CEntityManager::CreateEntity(eEntity::ENVIRONMENT);
	CEntityManager::AddComponentToEntity(Pillar, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(Pillar, eComponent::BOX_COLLIDER);

	pcMeshRender = (CMeshRenderer*)CEntityManager::AddComponentToEntity(Pillar, eComponent::MESH_RENDERER);
	pcMeshRender->SetVertexBuffer(eVertexBuffer::PILLAR);
	pcMeshRender->SetIndexBuffer(eIndexBuffer::PILLAR);
	pcMeshRender->SetTextureCount(1);
	pcMeshRender->SetTexture(eTexture::PILLAR, 0);
	pcMeshRender->SetInputLayout(eInputLayout::DEFAULT);
	pcMeshRender->SetVertexShader(eVertexShader::DEFAULT);
	pcMeshRender->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRender->SetSampler(eSampler::CLAMP);
	

	wall->SetActiveState(false);
	Door->SetActiveState(false);
	Pillar->SetActiveState(false);

#pragma endregion

#pragma region Enemys

	CEnemyEntity* pcEnemy = (CEnemyEntity*)CEntityManager::CreateEntity(eEntity::ENEMY);
	CEntityManager::AddComponentToEntity(pcEnemy, eComponent::TRANSFORM);
	CEntityManager::AddComponentToEntity(pcEnemy, eComponent::RIGIDBODY);


	CCapsuleCollider* pcPlayerCollider = (CCapsuleCollider*)CEntityManager::AddComponentToEntity(pcEnemy, eComponent::CAPSULE_COLLIDER);
	pcPlayerCollider->SetCenter(CMath::TVECTOR3(0, 3.5f, 0));
	pcPlayerCollider->SetHeight(7.0f);
	pcPlayerCollider->SetRadius(1.0f);

	CMeshRenderer* pcMeshRenderer = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcEnemy, eComponent::MESH_RENDERER);
	pcMeshRenderer->SetVertexBuffer(eVertexBuffer::ENEMY_MAGE);
	pcMeshRenderer->SetIndexBuffer(eIndexBuffer::ENEMY_MAGE);
	pcMeshRenderer->SetTextureCount(1);
	pcMeshRenderer->SetTexture(eTexture::ENEMY_MAGE_DIFFUSE, 0);
	pcMeshRenderer->SetInputLayout(eInputLayout::SKINNED);
	pcMeshRenderer->SetVertexShader(eVertexShader::SKINNED);
	pcMeshRenderer->SetPixelShader(ePixelShader::DEFAULT);
	pcMeshRenderer->SetSampler(eSampler::CLAMP);

	CAnimatorComponent* pcAnimator = (CAnimatorComponent*)CEntityManager::AddComponentToEntity(pcEnemy, eComponent::ANIMATOR);
	pcAnimator->SetAnimator(eAnimator::MAGE);
	pcAnimator->SetAnimation(eEnemyAnimation::IDLE);
	pcAnimator->SetAnimationTime(0.0);

	CStats*	pcStats = (CStats*)CEntityManager::AddComponentToEntity(pcEnemy, eComponent::STATS);
	pcStats->SetHP(2.0f);
	pcStats->SetMP(1000.0f);
	pcStats->SetMovementSpeed(8.0f);
	pcStats->SetBaseDamage(1.0f);

	CEntityManager::AddComponentToEntity(pcEnemy, eComponent::MAGE_CONTROLLER);

	pcEnemy->SetActiveState(false);

#pragma endregion

#pragma region Room 1 

	CRoomEntity* pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);

	CMeshRenderer* pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM1);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM1);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eTexture::ROOM1, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);

	//Pillars
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcPillars.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(Pillar));
		
		pcRoom->m_pcPillars[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_pcPillars[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(49.5, 0, 49.5));
	pcRoom->m_pcPillars[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(49.5, 0, -49.5));
	pcRoom->m_pcPillars[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-49.5, 0, 49));
	pcRoom->m_pcPillars[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-49.5, 0, -49));
	

	//Walls
	for (int i = 0; i < 16; i++)
	{
		pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));
		pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;
		
	}
	pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(39.5f, -3.3f, 47.5f));
	pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(22.5f, -3.3f, 47.5f));
	pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-18.0f, -3.3f, 47.5f));
	pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35.0f, -3.3f, 47.5f));
	pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(39.5f, -3.3f, -51.5f));
	pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(22.5f, -3.3f, -51.5f));
	pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-18.0f, -3.3f, -51.5f));
	pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-35.0f, -3.3f, -51.5f));

	pcRoom->m_pcWalls[8]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-50.5f, -3.3f, 38.0f));
	pcRoom->m_pcWalls[9]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-50.5f, -3.3f, 18.0f));
	pcRoom->m_pcWalls[10]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(47.5f, -3.3f, -36.0f));
	pcRoom->m_pcWalls[11]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(47.5f, -3.3f, -22.0f));
	pcRoom->m_pcWalls[12]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(47.5f, -3.3f, 36.0f));
	pcRoom->m_pcWalls[13]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(47.5f, -3.3f, 18.0f));
	pcRoom->m_pcWalls[14]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-50.5f, -3.3f, -22.0f));
	pcRoom->m_pcWalls[15]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-50.5f, -3.3f, -36.0f));

	//Rotate Walls
	pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));

	pcRoom->m_pcWalls[8]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcWalls[9]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcWalls[10]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcWalls[11]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcWalls[12]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcWalls[13]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcWalls[14]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcWalls[15]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));

	int wallsize = (int)pcRoom->m_pcWalls.size();

	for (int i = 0; i < wallsize; i++)
	{
		pcRoom->m_pcWalls[i]->GetComponent<CBoxCollider>()->SetCenter(CMath::TVECTOR3(0,9,0));
		pcRoom->m_pcWalls[i]->GetComponent<CBoxCollider>()->SetExtent(CMath::TVECTOR3(1, 6, 8));
	}



	//Doors
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));
		

		pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
		
	}
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(48.5, 0, 0));
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-48.5, 0, 0));
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 48.5));
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -48.5));

	//Rotate
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, -90));
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, -180));

	for (size_t i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors[i]->GetComponent<CBoxCollider>()->SetCenter({ 0, 9, 0 });
		pcRoom->m_pcDoors[i]->GetComponent<CBoxCollider>()->SetExtent(CMath::TVECTOR3(0.5f, 6, 10));

		
	}

	//Enemies

	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcEnemies.push_back((CEnemyEntity*)CEntityManager::CloneEntity(pcEnemy));

		pcRoom->m_pcEnemies[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_nEnemyCount = (int)pcRoom->m_pcEnemies.size();
	pcRoom->m_pcEnemies[0]->GetComponent<CTransform>()->SetPosition({ 20, 0, 10 });

	m_pRoomsType.push_back(pcRoom);
	pcRoom->SetActiveState(false);

	//Light
	//CLightEntity* pcLight = (CLightEntity*)CEntityManager::CreateEntity(eEntity::LIGHT);
	//pcLight->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	//pcLight->SetRadius(10.0f);

	//CEntityManager::AddComponentToEntity(pcLight, eComponent::TRANSFORM);

	#pragma endregion

#pragma region Room 2

	pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);
	pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM2);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM2);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eTexture::ROOM2, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);

	//Walls
	for (int i = 0; i < 16; i++)
	{
		pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));
		

		pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(19, 0, 29));
	pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-19, 0, 29));
	pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(19, 0, -29));
	pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-19, 0, -29));
	pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(29, 0, 19));
	pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-29, 0, 19));
	pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(29, 0, -19));
	pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-29, 0, -19));
	pcRoom->m_pcWalls[8]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, 40));
	pcRoom->m_pcWalls[9]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, 40));
	pcRoom->m_pcWalls[10]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(10, 0, -40));
	pcRoom->m_pcWalls[11]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-10, 0, -40));
	pcRoom->m_pcWalls[12]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, 10));
	pcRoom->m_pcWalls[13]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, 10));
	pcRoom->m_pcWalls[14]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(40, 0, -10));
	pcRoom->m_pcWalls[15]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-40, 0, -10));

	//Rotate Walls
	for (int i = 0; i <= 3; i++)
	{
		pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90.0f));
	}

	for (int i = 12; i <= 15; i++)
	{
		pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90.0f));
	}

	wallsize = (int)pcRoom->m_pcWalls.size();
	for (int i = 0; i < wallsize; i++)
	{
		pcRoom->m_pcWalls[i]->GetComponent<CBoxCollider>()->SetCenter({ 0, 0, 0 });
		pcRoom->m_pcWalls[i]->GetComponent<CBoxCollider>()->SetExtent(CMath::TVECTOR3(1, 12, 10));
	}

	//Doors
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));

		pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(49, 0, 0));
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-49, 0, 0));
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 49));
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -49));

	//Rotate
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, -90));
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 180));
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));

	for (size_t i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors[i]->GetComponent<CBoxCollider>()->SetCenter({ 0, 0, 0 });
		pcRoom->m_pcDoors[i]->GetComponent<CBoxCollider>()->SetExtent(CMath::TVECTOR3(0.5f, 12, 10));


	}

	//Enemies
	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcEnemies.push_back((CEnemyEntity*)CEntityManager::CloneEntity(pcEnemy));
		pcRoom->m_pcEnemies[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_nEnemyCount = (int)pcRoom->m_pcEnemies.size();
	pcRoom->m_pcEnemies[0]->GetComponent<CTransform>()->SetPosition({ 20, 0, 10 });

	m_pRoomsType.push_back(pcRoom);

	pcRoom->SetActiveState(false);

#pragma endregion

#pragma region Room 3

	pcRoom = (CRoomEntity*)CEntityManager::CreateEntity(eEntity::ROOM);
	CEntityManager::AddComponentToEntity(pcRoom, eComponent::TRANSFORM);
	pcMesh = (CMeshRenderer*)CEntityManager::AddComponentToEntity(pcRoom, eComponent::MESH_RENDERER);
	pcMesh->SetVertexBuffer(eVertexBuffer::ROOM3);
	pcMesh->SetIndexBuffer(eIndexBuffer::ROOM3);
	pcMesh->SetTextureCount(1);
	pcMesh->SetTexture(eTexture::ROOM3, 0);
	pcMesh->SetInputLayout(eInputLayout::DEFAULT);
	pcMesh->SetVertexShader(eVertexShader::DEFAULT);
	pcMesh->SetPixelShader(ePixelShader::DEFAULT);
	pcMesh->SetSampler(eSampler::WRAP);

	//Walls
	for (int i = 0; i < 9; i++)
	{
		pcRoom->m_pcWalls.push_back((CEnvironmentEntity*)CEntityManager::CloneEntity(wall));
		//CEntityManager::AddComponentToEntity(pcRoom->m_pcWalls[i], eComponent::TRANSFORM);
		//CEntityManager::AddComponentToEntity(pcRoom->m_pcWalls[i], eComponent::BOX_COLLIDER);

		pcRoom->m_pcWalls[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_pcWalls[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(49, 0, 29));
	pcRoom->m_pcWalls[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(29, 0, 49));
	pcRoom->m_pcWalls[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(49, 0, -29));
	pcRoom->m_pcWalls[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(29, 0, -49));
	pcRoom->m_pcWalls[4]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-49, 0, 29));
	pcRoom->m_pcWalls[5]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-29, 0, 49));
	pcRoom->m_pcWalls[6]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-49, 0, -29));
	pcRoom->m_pcWalls[7]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-29, 0, -49));
	pcRoom->m_pcWalls[8]->GetComponent<CTransform>()->SetPosition(pcRoom->GetComponent<CTransform>()->GetPosition());

	//Rotate Walls
	for (int i = 0; i < 8; i++)
	{
		if (i % 2 == 0)
		{
			pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));
		}
		else
		{
			pcRoom->m_pcWalls[i]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90.0f));
		}
	}

	wallsize = (int)pcRoom->m_pcWalls.size();
	for (size_t i = 0; i < wallsize - 1; i++)
	{
		pcRoom->m_pcWalls[i]->GetComponent<CBoxCollider>()->SetCenter({ 0, 0, 0 });
		pcRoom->m_pcWalls[i]->GetComponent<CBoxCollider>()->SetExtent(CMath::TVECTOR3(1, 12, 20));
	}
	pcRoom->m_pcWalls[8]->GetComponent<CBoxCollider>()->SetCenter({ 0, 0, 0 });
	pcRoom->m_pcWalls[8]->GetComponent<CBoxCollider>()->SetExtent(CMath::TVECTOR3(16, 6, 15.5));

	//Doors
	for (int i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors.push_back((CDoorEntity*)CEntityManager::CloneEntity(Door));

		pcRoom->m_pcDoors[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(49, 0, 0));
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(-49, 0, 0));
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, 49));
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetPosition(CMath::TVECTOR3(0, 0, -49));

	//Rotate
	pcRoom->m_pcDoors[2]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, -90));
	pcRoom->m_pcDoors[3]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 90));
	pcRoom->m_pcDoors[0]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 180));
	pcRoom->m_pcDoors[1]->GetComponent<CTransform>()->SetRotation(CMath::QuaternionFromAxisAngle({ 0, 1, 0 }, 0));

	for (size_t i = 0; i < 4; i++)
	{
		pcRoom->m_pcDoors[i]->GetComponent<CBoxCollider>()->SetCenter({ 0, 0, 0 });
		pcRoom->m_pcDoors[i]->GetComponent<CBoxCollider>()->SetExtent(CMath::TVECTOR3(0.5f, 12, 10));


	}

	//Enemies
	for (int i = 0; i < 1; i++)
	{
		pcRoom->m_pcEnemies.push_back((CEnemyEntity*)CEntityManager::CloneEntity(pcEnemy));
		pcRoom->m_pcEnemies[i]->m_pcRoom = pcRoom;
	}
	pcRoom->m_nEnemyCount = (int)pcRoom->m_pcEnemies.size();
	pcRoom->m_pcEnemies[0]->GetComponent<CTransform>()->SetPosition({ 20, 0, 10 });

	m_pRoomsType.push_back(pcRoom);

	pcRoom->SetActiveState(false);

#pragma endregion

	m_pcCurrentLevel = new CLevel();
}

CLevelManager::~CLevelManager()
{
	if (m_pcCurrentLevel)	delete m_pcCurrentLevel;
}
