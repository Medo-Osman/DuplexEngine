#pragma once
#include "3DPCH.h"
#include "BaseAction.h"
#include "PhysicsComponent.h"
#include "Entity.h"
#include "AnimatedMeshComponent.h"
#include <random>
#include "MoveInGridAction.h"

class MoveToTargetInGridAction : public BossStructures::BaseAction
{
private:
	BossStructures::BossActionData m_data;
	Timer m_timer;
	Vector2 m_targetIndicies;
	float m_speed = 0;

	Vector2* m_currentPlatformIndex;
	BossStructures::PlatformArray* m_platformArray;
	std::vector<BossStructures::BaseAction*>* m_actionQueue;

	BossSubject* m_bossSubject = nullptr;

	Vector3 dir;
	Entity* targetEntity = nullptr;
public:
	MoveToTargetInGridAction(Entity* bossEntity, BossSubject* bossSubject, BossStructures::PlatformArray* platformArray, Vector2 targetIndicies, float speed, Vector2* currentPlatformIndex, std::vector<BossStructures::BaseAction*>* actionQueue)
		:BaseAction(bossEntity, bossSubject)
	{
		m_currentPlatformIndex = currentPlatformIndex;
		m_movementActionType = BossMovementType::MoveTo;
		m_actionQueue = actionQueue;
		m_timer.start();

		m_targetIndicies = targetIndicies;
		m_speed = speed;
		m_platformArray = platformArray;
		m_bossSubject = bossSubject;
	}

	void setSpeed(float speed)
	{
		m_data.speed = speed;
	}

	void setBossActionData(BossStructures::BossActionData data)
	{
		m_data = data;
	}

	// Inherited via BaseAction
	virtual void beginAction() override
	{
		m_timeData.timer.start();
		AnimatedMeshComponent* animComp = static_cast<AnimatedMeshComponent*>(m_bossEntity->getComponent("mesh"));


		//targetEntity = (*m_platformArray)[m_currentPlatformIndex->x + m_targetIndicies.x][m_currentPlatformIndex->y + m_targetIndicies.y];
		
		int deltaX = m_targetIndicies.x - (*m_currentPlatformIndex).x;
		int deltaY = m_targetIndicies.y - (*m_currentPlatformIndex).y;

		for (int x = 0; x < abs(deltaX); x++)
		{
			int multiplier = (deltaX > 1) ? 1 : -1;

			m_actionQueue->push_back(new MoveInGridAction(m_bossEntity, m_bossSubject, m_platformArray, Vector2(1*multiplier, 0), m_speed, m_currentPlatformIndex));
		}
		for (int y = 0; y < abs(deltaY); y++)
		{
			int multiplier = (deltaY > 1) ? 1 : -1;

			m_actionQueue->push_back(new MoveInGridAction(m_bossEntity, m_bossSubject, m_platformArray, Vector2(0, 1*multiplier), m_speed, m_currentPlatformIndex));
		}
	}

	virtual void update(const float& dt) override
	{
		//Intentionally left empty
	}

	virtual bool isDone() override
	{
		return true;
	}

};