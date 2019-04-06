#pragma once
#include "System.h"

#include "Math.h"

#include <vector>
#include <map>
#include <array>


#define ROOM_HEIGHT 100
#define ROOM_WIDTH 100
#define CELL_SIZE 10


class IEntity;
class CTransform;
class CRigidbody;
class ICollider;
class CBoxCollider;
class CCapsuleCollider;
class CComponentManager;
class CTransform;
class CRoomEntity;
class CPlayerEntity;

struct TRayHit
{
	IEntity* tEntity;
	CMath::TVECTOR3 tCollisionPoint;
};

class CEntityMask
{
private:
	int mask;

public:
	CEntityMask() : mask(0) {}
	CEntityMask(const int nEntityType);
	CEntityMask(const int* nEntityTypes, int nCount);

	CEntityMask operator +(const int nEntityType) const;
	CEntityMask operator -(const int nEntityType) const;
	void operator +=(const int nEntityType);
	void operator -=(const int nEntityType);
	int operator &(const int nMaskCheck) const;
};

class CPhysicsManager : public ISystem
{
private:

	struct TCollision
	{
		CMath::TVECTOR3 tAxis;
		float fOffset;
	};

	struct TCoordinate
	{
		int x, y;
		TCoordinate(int _x, int _y) : x(_x), y(_y) {}
	};

	struct TPlane
	{
		CMath::TVECTOR3 tNormal;
		float fDistance;
	};


	std::vector<CTransform*>*										m_pcTransforms;
	std::vector<CRigidbody*>*										m_pcRigidbodies;
	std::vector<ICollider*>*										m_pcColliders;
	static std::vector<ICollider*>									m_cStaticColliders;
	static std::vector<ICollider*>									m_cDynamicColliders;
	std::map<ICollider*, std::vector<ICollider*>>					m_cCollisions;

	std::map<ICollider*, std::vector<TCoordinate>>					m_StaticGridPositions;
	std::map<ICollider*, std::vector<TCoordinate>>					m_DynamicGridPositions;

	CComponentManager*												m_pcComponentManager;

	CRoomEntity*													m_pcCurrentRoom;
	CPlayerEntity*													m_pcPlayerEntity;

	CMath::TVECTOR3													m_tRoomPosition;

	std::vector<ICollider*>											m_Grid [ROOM_WIDTH / CELL_SIZE][ROOM_HEIGHT / CELL_SIZE];

	static TPlane															m_FloorPlane;

public:
#ifdef MULTI_THREADING

	std::mutex										m_cMainMutex;
	std::mutex										m_cMutexLock;

	std::condition_variable							m_cGunCondition;
	std::condition_variable							m_cMainWait;

	std::vector<std::thread>						threads;

	bool											m_bBitMask[1024]{ false };
	bool											m_bShutDownFlag;
	double											m_dDelta;
	size_t											m_iFinishedThreads;
	size_t											m_iNumThreads;
#endif // MUTLI_THREADING	

	CPhysicsManager(CComponentManager* pcComponentManager);
	~CPhysicsManager();

	void UpdateComponentLists();
	void UpdateRigidBodies(double dDeltaTime);
	void UpdateCollisions();

	static bool Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, float fMaxDistance = std::numeric_limits<float>::infinity());
	static bool Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit& tRayHit, CEntityMask tMask = CEntityMask());
	static bool Raycast(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, TRayHit& tRayHit, float fMaxDistance = std::numeric_limits<float>::infinity(), CEntityMask tMask = CEntityMask());
	static bool RaycastMouseToFloor(CMath::TVECTOR3 tOrigin, CMath::TVECTOR3 tDirection, CMath::TVECTOR3& tFloorPosition);

private:

	void PairwiseCollisionCheck(ICollider* pcA, ICollider* pcB, int nCollisionType, bool bIsTriggerCollision);
	int GetCollisionType(ICollider* pcA, ICollider* pcB);

	void UpdateBoxBoxCollision(CBoxCollider* pcA, CBoxCollider* pcB, bool bIsTriggerCollision, bool& bIsColliding);
	void UpdateCapsuleCapsuleCollision(CCapsuleCollider* pcA, CCapsuleCollider* pcB, bool bIsTriggerCollision, bool& bIsColliding);
	void UpdateBoxCapsuleCollision(CBoxCollider* pcA, CCapsuleCollider* pcB, bool bIsTriggerCollision, bool bReverseOrder, bool& bIsColliding);

	bool IsCollidingBoxBox(CBoxCollider* pcA, CBoxCollider* pcB, TCollision& tCollision);
	bool IsCollidingCapsuleCapsule(CCapsuleCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollision);
	bool IsCollidingBoxCapsule(CBoxCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollision);

	bool IsIntersecting(CMath::TVECTOR3 tAxis, CBoxCollider* pA, CBoxCollider* pB, float& fMinOverlap, CMath::TVECTOR3& tMinAxis);

	void ResolveCollision(CBoxCollider* pcA, CBoxCollider* pcB, TCollision& tCollision);
	void ResolveCollision(CCapsuleCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollision);
	void ResolveCollision(CBoxCollider* pcA, CCapsuleCollider* pcB, TCollision& tCollsion);

	bool IsNewCollision(ICollider* pcA, ICollider* pcB);

	bool IsCollisionExit(ICollider* pcA, ICollider* pcB, int& nIndexA, int& nIndexB);

	static float Clamp(float fValue, float fMin, float fMax);
	static float GetRatio(float fValue, float fMin, float fMax);

	static void ClosestPtSegmentSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3& tPs, CMath::TVECTOR3& tQt);
	static void ClosestPtLineSegment(CMath::TVECTOR3 tP0, CMath::TVECTOR3 tDirection, CMath::TVECTOR3 tQ0, CMath::TVECTOR3 tQ1, CMath::TVECTOR3& tPs, CMath::TVECTOR3& tQt);
	static void ClosestPtPointOBB(CMath::TVECTOR3 tPoint, CBoxCollider* pcB, CMath::TVECTOR3& tResult);
	static void ClosestPtPointSegment(CMath::TVECTOR3 tPoint, CMath::TVECTOR3 tP0, CMath::TVECTOR3 tP1, CMath::TVECTOR3& tResult);

	int FindGridPositionX(CMath::TVECTOR3 tPosition, int min, int max);
	int FindGridPositionZ(CMath::TVECTOR3 tPosition, int min, int max);

	static bool IsCollidingRayBox(CBoxCollider* pcBoxCollider, CMath::TVECTOR3 tRayOrigin, CMath::TVECTOR3 tRayDirection, float fMaxDistance, CMath::TVECTOR3& tCollisionPoint);
	static bool IsCollidingRayCapsule(CCapsuleCollider* pCapsuleCollider, CMath::TVECTOR3 tRayOrigin, CMath::TVECTOR3 tRayDirection, float fMaxDistance, CMath::TVECTOR3& tCollisionPoint);
};

