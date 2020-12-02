#pragma once
#include "BaseAction.h"
#include "Entity.h"
#include <random>

class MoveToAction : public BossStructures::BaseAction
{
private:
	BossStructures::BossActionData m_data;
	Timer m_timer;
	Vector3 m_target;
	float m_speed = 0;


public:
	MoveToAction(Entity* bossEntity, BossSubject* bossSubject, Vector3 target, float speed)
		:BaseAction(bossEntity, bossSubject)
	{
		m_movementActionType = BossMovementType::MoveTo;
		m_timer.start();

		m_target = target;
		m_speed = speed;
	}

	void setTarget(Vector3 target)
	{
		m_target = target;
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
		//AnimatedMeshComponent* animComp = static_cast<AnimatedMeshComponent*>(m_bossEntity->getComponent("mesh"));
		//animComp->playSingleAnimation("Running4.1", 0.1f, false, true);
		//animComp->setCurrentBlend(0.5f);
		//animComp->setAnimationSpeed(0.5f);
		//animComp->pla
	}

	virtual void update(const float& dt) override
	{
		Vector3 dir = XMVector3Normalize((m_target - m_bossEntity->getTranslation()));
		m_bossEntity->setPosition(m_bossEntity->getTranslation() + dir * m_speed * dt);
	}

	virtual bool isDone() override
	{
		return (m_bossEntity->getTranslation() - m_target).LengthSquared() < 0.25f;
	}

};