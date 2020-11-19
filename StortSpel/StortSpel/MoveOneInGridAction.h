#pragma once
#include "BaseAction.h"
#include "Entity.h"
#include "AnimatedMeshComponent.h"
#include <random>

class MoveOneInDirGridAction : public BossStructures::BaseAction
{
private:
	BossStructures::BossActionData m_data;
	Timer m_timer;
	Vector2 m_moveHowMuch;
	float m_speed = 0;

	Vector2* m_currentPlatformIndex;
	BossStructures::PlatformArray* m_platformArray;

	Vector3 dir;
	Entity* targetEntity = nullptr;

	void movePlatform(Entity* entity)
	{
		m_data.pointer0 = entity;
		m_data.origin = Vector3(entity->getTranslation().x, 4, entity->getTranslation().z);
		m_subjectPtr->Notify(BossMovementType::MovePlatform, m_data);
		m_subjectPtr->Notify(BossMovementType::DropPoints, m_data);
	}
public:
	MoveOneInDirGridAction(Entity* bossEntity, BossSubject* bossSubject, BossStructures::PlatformArray* platformArray, Vector2 moveHowMany, float speed, Vector2* currentPlatformIndex)
		:BaseAction(bossEntity, bossSubject)
	{
		m_currentPlatformIndex = currentPlatformIndex;
		m_movementActionType = BossMovementType::MoveTo;
		m_timer.start();

		m_moveHowMuch = moveHowMany;
		m_speed = speed;
		m_platformArray = platformArray;
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

		//Handle x-movement
		if (m_moveHowMuch.x > 0)
		{
			//In case move in positive x
			for (int x = m_currentPlatformIndex->x; x < m_currentPlatformIndex->x + m_moveHowMuch.x; x++)
			{
				/*PhysicsComponent* comp = static_cast<PhysicsComponent*>((*m_platformArray)[x][m_currentPlatformIndex->y]->getComponent("physics"));
				Vector3 currPos = comp->getActorPosition();
				comp->setPosition(currPos + Vector3(0, 300.f, 0));

				MeshComponent* meshComp = static_cast<MeshComponent*>((*m_platformArray)[x][m_currentPlatformIndex->y]->getComponent("mesh"));
				meshComp->setVisible(false);*/
				Entity* platform = (*m_platformArray)[x][m_currentPlatformIndex->y];
				movePlatform(platform);
			}
		}
		else
		{
			//In case move in negative x
			for (int x = m_currentPlatformIndex->x; x > m_currentPlatformIndex->x + m_moveHowMuch.x; x--)
			{
				/*PhysicsComponent* comp = static_cast<PhysicsComponent*>((*m_platformArray)[x][m_currentPlatformIndex->y]->getComponent("physics"));
				Vector3 currPos = comp->getActorPosition();
				comp->setPosition(currPos + Vector3(0, 300.f, 0));

				MeshComponent* meshComp = static_cast<MeshComponent*>((*m_platformArray)[x][m_currentPlatformIndex->y]->getComponent("mesh"));
				meshComp->setVisible(false);*/
				Entity* platform = (*m_platformArray)[x][m_currentPlatformIndex->y];
				movePlatform(platform);
			}
		}

		//Handle y-movement
		if (m_moveHowMuch.y > 0)
		{	//In case move in positive y
			for (int y = m_currentPlatformIndex->y; y < m_currentPlatformIndex->y + m_moveHowMuch.y; y++)
			{
				/*PhysicsComponent* comp = static_cast<PhysicsComponent*>((*m_platformArray)[m_currentPlatformIndex->x][y]->getComponent("physics"));
				Vector3 currPos = comp->getActorPosition();
				comp->setPosition(currPos + Vector3(0, 300.f, 0));

				MeshComponent* meshComp = static_cast<MeshComponent*>((*m_platformArray)[m_currentPlatformIndex->x][y]->getComponent("mesh"));
				meshComp->setVisible(false);*/
				Entity* platform = (*m_platformArray)[m_currentPlatformIndex->x][y];
				movePlatform(platform);
			}
		}
		else
		{	//In case move in negative y
			for (int y = m_currentPlatformIndex->y; y > m_currentPlatformIndex->y + m_moveHowMuch.y; y--)
			{
				/*PhysicsComponent* comp = static_cast<PhysicsComponent*>((*m_platformArray)[m_currentPlatformIndex->x][y]->getComponent("physics"));
				Vector3 currPos = comp->getActorPosition();
				comp->setPosition(currPos + Vector3(0, 300.f, 0));

				MeshComponent* meshComp = static_cast<MeshComponent*>((*m_platformArray)[m_currentPlatformIndex->x][y]->getComponent("mesh"));
				meshComp->setVisible(false);*/
				Entity* platform = (*m_platformArray)[m_currentPlatformIndex->x][y];
				movePlatform(platform);
			}
		}
		
		targetEntity = (*m_platformArray)[m_currentPlatformIndex->x + m_moveHowMuch.x][m_currentPlatformIndex->y + m_moveHowMuch.y];
		*m_currentPlatformIndex = *(m_currentPlatformIndex)+m_moveHowMuch;
		Vector3 targetPosition = targetEntity->getTranslation();
		dir = XMVector3Normalize(Vector3(targetPosition.x, m_bossEntity->getTranslation().y, targetPosition.z) - m_bossEntity->getTranslation());
	}

	virtual void update(const float& dt) override
	{
		//Vector3 dir = XMVector3Normalize(((*m_platformArray)[m_currentPlatformIndex->x + m_moveHowMuch.x][m_currentPlatformIndex->y + m_moveHowMuch.y]->getTranslation() - m_bossEntity->getTranslation()));
		

		m_bossEntity->setPosition(m_bossEntity->getTranslation() + dir * m_speed * dt);
	}

	virtual bool isDone() override
	{
		Vector3 targetPos = targetEntity->getTranslation();
		return (m_bossEntity->getTranslation() - Vector3(targetPos.x, m_bossEntity->getTranslation().y, targetPos.z)).Length() < 0.25f;
	}

};