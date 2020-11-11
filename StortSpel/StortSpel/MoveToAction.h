#pragma once
#include "BaseAction.h"
#include "Entity.h"
#include <random>

class MoveToAction : public BossStructures::BaseAction
{
private:
	BossStructures::BossActionData m_data;
	Timer m_timer;
	float m_shootInterval = 1.f; //Seconds
	float m_maxDirectionOffset = 4.f;


public:
	MoveToAction(Entity* bossEntity, BossSubject* bossSubject, int howLongShouldActionLast = 10, int coolDownAfterAction = 5)
		:BaseAction(bossEntity, bossSubject)
	{
		m_movementActionType = BossMovementType::ShootProjectile;
		m_timer.start();

		m_timeData.maxDuration = howLongShouldActionLast;
		m_timeData.cooldownAfterAction = coolDownAfterAction;
	}

	void setTarget(Vector3 target)
	{
		Vector3 unnormalizedDir = (target - m_data.origin);
		//unnormalizedDir.Normalize(m_data.direction);
		m_data.direction = unnormalizedDir;
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
		//std::cout << "Began action!" << std::endl;
	}
	virtual void update() override
	{
		if (m_timer.timeElapsed() > m_shootInterval)
		{
			m_data.origin = m_bossEntity->getTranslation() + Vector3(0, m_bossEntity->getScaling().y / 2, 0);
			m_data.direction = m_data.direction;
			m_data.speed = 0.5f;

			//Send event to scene triggering the shoot projectile with the parameters in m_data.

			std::default_random_engine generator;
			std::normal_distribution<float> distribution(0.f, m_maxDirectionOffset * 2); //Only generates unsigned ints

			Vector3 originalDir = m_data.direction;

			//Generate random offsets
			float offY = distribution(generator);
			float offZ = distribution(generator);
			float offX = distribution(generator);

			//Make negative numbers possible
			offY -= offY / 2;
			offZ -= offZ / 2;
			offX -= offX / 2;

			m_data.direction = originalDir + Vector3(offY, offZ, offX);
			m_subjectPtr->Notify(BossMovementType::ShootProjectile, m_data);

			//Reset clock to count from 0
			m_timer.restart();
		}

	}
	virtual bool isDone() override
	{
		return false;
	}

};