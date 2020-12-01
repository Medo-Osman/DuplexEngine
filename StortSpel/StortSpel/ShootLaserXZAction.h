#pragma once
#include "BaseAction.h"
#include "Entity.h"
#include "PhysicsComponent.h"
#include <random>

class ShootLaserAction : public BossStructures::BaseAction
{
private:
	BossStructures::BossActionData m_data;
	Timer m_timer;
	float m_shootInterval = 0.f; //Seconds
	bool m_hasFired = false;
public:
	ShootLaserAction(Entity* bossEntity, BossSubject* bossSubject, int coolDownAfterAction = 5)
		:BaseAction(bossEntity, bossSubject)
	{
		m_movementActionType = BossMovementType::ShootLaser;

		m_timeData.cooldownAfterAction = coolDownAfterAction;
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

		m_hasFired = false;

	}

	virtual void update(const float& dt) override
	{
		if (!m_hasFired)
		{
			//Fire in x, z
			for (int i = 0; i < 360; i += 90)
			{
				Component* componentPtr = m_bossEntity->getComponent("physics");
				if (componentPtr)
				{
					PhysicsComponent* physPtr = static_cast<PhysicsComponent*>(componentPtr);
					m_data.origin = physPtr->getActorPosition();
				}
				else
					m_data.origin = m_bossEntity->getTranslation();
				
				m_data.rotation = Vector3(0, XMConvertToRadians(i), 0);

				//Send event to scene triggering the shoot projectile with the parameters in m_data.
				m_subjectPtr->Notify(BossMovementType::ShootLaser, m_data);
			}
			m_timeData.timer.restart();
			m_hasFired = true;

			m_subjectPtr->Notify(BossMovementType::ClearColumn, m_data); //Data is not neccessary here.
			m_subjectPtr->Notify(BossMovementType::ClearRow, m_data);
		}

	}
	virtual bool isDone() override
	{
		bool isDone = (m_hasFired && m_timeData.timer.timeElapsed() > m_timeData.cooldownAfterAction);

		return isDone;
	}

};