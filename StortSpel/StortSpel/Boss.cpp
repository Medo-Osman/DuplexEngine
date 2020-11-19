#include "3DPCH.h"
#include "Boss.h"
int BossObserver::nr = 0;

Boss::~Boss()
{
	Physics::get().Detach(this, true, false);
}

void Boss::update(const float& dt)
{
	std::cout << bossSegments.size() << std::endl;
	if (m_currentAction && !m_currentAction->isDone())
	{

		if (!m_currentAction->isDone())
			m_currentAction->update(dt);
	}
	else if (m_actionQueue.size() > 0)
		nextAction();

	for (int i = 0; i < bossSegments.size(); i++)
	{
		bossSegments.at(i)->update(dt);
		PhysicsComponent* comp = static_cast<PhysicsComponent*>(bossSegments.at(i)->m_bossEntity->getComponent("physics"));
		comp->setPosition(m_bossEntity->getTranslation() + bossSegments.at(i)->m_entityOffset);

		bossSegments.at(i)->m_bossEntity->setPosition(m_bossEntity->getTranslation() + bossSegments.at(i)->m_entityOffset);
	}


}

void Boss::initialize(Entity* entity, bool destroyActionOnComplete)
{
	m_bossEntity = entity;
	m_destroyActionOnComplete = destroyActionOnComplete;
}

void Boss::addSegment(BossSegment* segment)
{
	bossSegments.push_back(segment);
}


void Boss::sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity)
{
	if (physicsData.triggerType == TriggerType::PROJECTILE)
	{

		if ((EventType)physicsData.associatedTriggerEnum == EventType::BOSS_PROJECTILE_HIT)
		{
			Entity* ptr = static_cast<Entity*>(physicsData.pointer);
			auto component = static_cast<ProjectileComponent*>(ptr->getComponent("projectile"));
			BossStructures::BossActionData data;
			data.origin = ptr->getTranslation();
			data.pointer0 = ptr;

			//If no longer debouncing, drop points
			if (component->m_timer.timeElapsed() > component->m_removalDebounce)
				Notify(BossMovementType::DropPoints, data);
		}
	}
}
