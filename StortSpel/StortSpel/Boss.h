#pragma once
#include "3DPCH.h"
#include "Entity.h"
#include "Engine.h"
#include "BaseAction.h"
#include "ShootProjectileAction.h"
#include "MoveToAction.h"
#include "BossSegment.h"
#include "MoveOneInGridAction.h"
#include "MoveToTargetInGridAction.h"
#include "ShootLaserXZAction.h"
#include "WaitAction.h"

#include "ShrinkingComponent.h"
#include "GrowingComponent.h"


class Boss : public PhysicsObserver, public BossSegment
{
private:
public:


	BossStructures::PlatformArray platformArray = BossStructures::PlatformArray(10);
	Vector2 currentPlatformIndex = Vector2(0, 0);
	std::vector<BossSegment*> m_bossSegments;

	Boss() { };
	~Boss();
	void update(const float& dt);
	void initialize(Entity* entity, bool destroyActionOnComplete = true);
	void addSegment(BossSegment* segment);
	BossStructures::IntVec getNewPlatformTarget();

	// Inherited via PhysicsObserver
	virtual void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity) override;
};





