#pragma once
#include "3DPCH.h"
#include "Entity.h"
#include "Engine.h"
#include "BaseAction.h"
#include "ShootProjectileAction.h"
#include "MoveToAction.h"
#include "BossSegment.h"
#include "MoveInGridAction.h"
#include "MoveToTargetInGridAction.h"


class Boss : public PhysicsObserver, public BossSegment
{
private:
public:
	BossStructures::PlatformArray platformArray = BossStructures::PlatformArray(10);

	Vector2 currentPlatformIndex = Vector2(0, 0);
	std::vector<BossSegment*> bossSegments;

	Boss() { };
	~Boss();
	void update(const float& dt);
	void initialize(Entity* entity, bool destroyActionOnComplete = true);
	void addSegment(BossSegment* segment);

	// Inherited via PhysicsObserver
	virtual void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity) override;
};





