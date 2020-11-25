#include "3DPCH.h"
#include "Boss.h"
int BossObserver::nr = 0;

Boss::~Boss()
{
	Physics::get().Detach(this, true, false);

	for (int i = 0; i < m_bossSegments.size(); i++)
	{
		delete m_bossSegments.at(i);
	}

	m_bossSegments.clear();
}

void Boss::update(const float& dt)
{
	if (m_currentAction && !m_currentAction->isDone())
	{

		if (!m_currentAction->isDone())
			m_currentAction->update(dt);
	}
	else if (m_actionQueue.size() > 0)
		nextAction();

	for (int i = 0; i < m_bossSegments.size(); i++)
	{
		m_bossSegments.at(i)->update(dt);

		Component* compPtr = m_bossSegments.at(i)->m_bossEntity->getComponent("physics");
		if (compPtr)
		{
			PhysicsComponent* physComp = static_cast<PhysicsComponent*>(m_bossSegments.at(i)->m_bossEntity->getComponent("physics"));
			physComp->setPosition(m_bossEntity->getTranslation() + m_bossSegments.at(i)->m_entityOffset);
		}

		m_bossSegments.at(i)->m_bossEntity->setPosition(m_bossEntity->getTranslation() + (m_bossSegments.at(i)->m_bossEntity->getTranslation() - m_bossEntity->getTranslation()));
	}


}

void Boss::initialize(Entity* entity, bool destroyActionOnComplete)
{
	m_bossEntity = entity;
	m_destroyActionOnComplete = destroyActionOnComplete;
}

void Boss::addSegment(BossSegment* segment)
{
	m_bossSegments.push_back(segment);
}

BossStructures::IntVec Boss::getNewPlatformTarget() //Generate new random target 
{
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distribution(0, platformArray.columns.size() - 1);
	int numberX = (int)distribution(generator);
	int numberY = (int)distribution(generator);

	return BossStructures::IntVec{ numberX, numberY };
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
