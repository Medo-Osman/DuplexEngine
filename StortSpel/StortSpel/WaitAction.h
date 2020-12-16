#pragma once
#include"3DPCH.h"
#include "BaseAction.h"
#include "Entity.h"
#include "PhysicsComponent.h"


class WaitAction : public BossStructures::BaseAction
{
private:
public:
	WaitAction(Entity* bossEntity, BossSubject* bossSubject, int waitTime)
		:BaseAction(bossEntity, bossSubject)
	{
		m_movementActionType = BossMovementType::Wait;

		m_timeData.cooldownAfterAction = waitTime;
	}


	// Inherited via BaseAction
	virtual void beginAction() override
	{
		//Do initialization stuff for action
		m_timeData.timer.start();

		//Have to do this, isDone() is time controlled, otherwise it will skip the action if looping is enabled.
		m_timeData.timer.restart();

	}

	virtual void update(const float& dt) override
	{
		//Intentionally left empty
	}
	virtual bool isDone() override
	{
		bool isDone = (m_timeData.timer.timeElapsed() > m_timeData.cooldownAfterAction);

		return isDone;
	}

};