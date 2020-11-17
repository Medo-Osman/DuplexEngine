#pragma once
#include "3DPCH.h"
#include "Entity.h"
#include "Engine.h"
#include "BaseAction.h"
#include "ShootProjectileAction.h"
#include "MoveToAction.h"
#include "BossSegment.h"


class Boss : public PhysicsObserver, public BossSegment
{
private:
public:
	BossStructures::PlatformArray platformArray = BossStructures::PlatformArray(10);

	Boss() { };
	~Boss();
	void update(const float& dt);
	void initialize(Entity* entity, bool destroyActionOnComplete = true);

	// Inherited via PhysicsObserver
	virtual void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity) override;
};





