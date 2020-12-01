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
#include "ThunderAction.h"

#include "ShrinkingComponent.h"
#include "GrowingComponent.h"


class Boss : public PhysicsObserver, public BossSegment
{
private:
	int m_maxStarCount = 0;
	int m_currentStarCount = 0;
	const int NR_OF_PLATFORMS = 5;
public:


	BossStructures::PlatformArray platformArray = BossStructures::PlatformArray(NR_OF_PLATFORMS);
	Vector2 currentPlatformIndex = Vector2(0, 0);
	std::vector<BossSegment*> m_bossSegments;

	Boss() { };
	~Boss();
	void update(const float& dt);
	void initialize(Entity* entity, bool destroyActionOnComplete = true);
	void addSegment(BossSegment* segment);
	BossStructures::IntVec getNewPlatformTarget();

	void dropStar(int dropAmount);
	int getCurrnentNrOfStars();
	void setNrOfMaxStars(int maxValue);
	int getNrOfMaxStars();
	int getNrOfPlatforms();

	// Inherited via PhysicsObserver
	virtual void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity) override;
};





