#pragma once
#include "BaseAction.h"

class ShootProjectileAction : public BossStructures::BaseAction
{
private:
	BossStructures::BossActionData m_data;
	BossStructures::actionTimeData m_timeData;
	Timer m_timer;
	float m_shootInterval = 15.f; //Seconds

public:
	ShootProjectileAction(Entity* bossEntity, BossSubject* bossSubject)
		:BaseAction(bossEntity, bossSubject)
	{
		m_movementActionType = BossMovementType::ShootProjectile;
		m_timer.start();
	}

	void setTarget(Vector3 target)
	{
		Vector3 unnormalizedDir = (target - m_bossEntity->getTranslation());
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
		std::cout << "Began action!" << std::endl;
	}
	virtual void update() override
	{
		if (m_timer.timeElapsed() > m_shootInterval)
		{
			m_data.origin = m_bossEntity->getTranslation();
			m_data.direction = m_data.direction;//Vector3(0, 0, 1);
			m_data.speed = 1.f;

			m_subjectPtr->Notify(BossMovementType::ShootProjectile, m_data);
			m_timer.restart();
		}
			
	}
	virtual bool isDone() override
	{
		return false;
	}

};