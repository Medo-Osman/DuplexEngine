#pragma once
#include "BaseAction.h"
#include <random>

class ShootProjectileAction : public BossStructures::BaseAction
{
private:
	BossStructures::BossActionData m_data;
	Timer m_timer;
	float m_shootInterval = 0.5f; //Seconds
	Vector3 m_target;

	bool m_targetSet = false;
public:
	ShootProjectileAction(Entity* bossEntity, BossSubject* bossSubject, float howLongShouldActionLast = 10, float coolDownAfterAction = 5)
		:BaseAction(bossEntity, bossSubject)
	{
		m_movementActionType = BossMovementType::ShootProjectile;

		m_timeData.maxDuration = howLongShouldActionLast;
		m_timeData.cooldownAfterAction = coolDownAfterAction;
	}

	void setTarget(Vector3 target)
	{
		//m_data.origin = static_cast<PhysicsComponent*>(m_bossEntity->getComponent("physics"))->getActorPosition();//m_bossEntity->getTranslation() + Vector3(0, m_bossEntity->getScaling().y / 2, 0);

		Component* componentPtr = m_bossEntity->getComponent("physics");
		if (componentPtr)
		{
			PhysicsComponent* physPtr = static_cast<PhysicsComponent*>(componentPtr);
			m_data.origin = physPtr->getActorPosition();
		}
		else
			m_data.origin = m_bossEntity->getTranslation();

		m_target = target;
		Vector3 unnormalizedDir = (target - m_data.origin);
		m_data.direction = XMVector3Normalize(unnormalizedDir);
		m_targetSet = true;
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
		//Do initialization stuff for action
		m_timer.start();
		m_timeData.timer.start();

		//Have to do this, isDone() is time controlled, otherwise it will skip the action if looping is enabled.
		m_timer.restart();
		m_timeData.timer.restart();

	}

	virtual void update(const float& dt) override
	{
		//std::cout << m_timer.timeElapsed() << std::endl;
		if (m_timer.timeElapsed() >= m_shootInterval && m_targetSet)
		{
			//m_bossEntity->lookAt(m_target);
			m_data.speed = 10.f;

			//Send event to scene triggering the shoot projectile with the parameters in m_data.
			m_subjectPtr->Notify(BossMovementType::ShootProjectile, m_data);

			//Reset clock to count from 0
			m_timer.restart();
		}
			
	}
	virtual bool isDone() override
	{
		bool isDone = false;

		auto timeData = &m_timeData;
		//If we're after the cooldown period, switch actions
		float time = (float)timeData->timer.timeElapsed();
		if (time >= timeData->maxDuration + timeData->cooldownAfterAction)
		{
			isDone = true;
		}

		return isDone;
	}

};