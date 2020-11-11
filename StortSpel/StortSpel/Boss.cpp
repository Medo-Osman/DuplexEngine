#include "3DPCH.h"
#include "Boss.h"
int BossObserver::nr = 0;

Boss::~Boss()
{
	Physics::get().Detach(this, true, false);
}

void Boss::update()
{
	if (m_currentAction && !m_currentAction->isDone())
	{

		if (!m_currentAction->isDone())
			m_currentAction->update();
		else
			nextAction();

	}
	
}

void Boss::initialize(Entity* entity)
{
	m_bossEntity = entity;
	//Physics::get().Attach(this, true, false);
}

UINT Boss::addAction(BossStructures::BaseAction* action)
{
	//Put at the back of the queue, FIFO applies. Back is actually front in this case.
	m_actionQueue.push_back(action);
	action->m_actionID = m_uniqueActionID++;

	if (m_currentAction == nullptr)
		m_currentAction = action;

	return action->m_actionID;
}

void Boss::nextAction()
{
	if (m_actionQueue.size() > 0)
	{
		m_currentAction = m_actionQueue.back();
		m_actionQueue.pop_back();
		m_currentAction->beginAction();
	}
	else
	{
		m_currentAction = nullptr;
		ErrorLogger::get().logError(L"Boss error - No other action to switch to!");
	}
}

BossStructures::BaseAction* Boss::getCurrentAction()
{
	return m_currentAction;
}

std::vector<BossStructures::BaseAction*>* Boss::getActionQueue()
{
	return &m_actionQueue;
}

void Boss::Attach(BossObserver* observer)
{
	m_observers[BossObserver::nr++] = observer;
}

void Boss::Detach(BossObserver* observer)
{
	m_observers.erase(observer->index);
}

void Boss::Notify(BossMovementType type, BossStructures::BossActionData data)
{
	for (auto observer : m_observers)
	{
		observer.second->bossEventUpdate(type, data);
	}
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
			data.origin = ptr->getTranslation();//Vector3(0, 1, 0);//static_cast<PhysicsComponent*>(ptr->getComponent("physics"))->getActorPosition();//->getTranslation();
			data.pointer0 = ptr;

			//If no longer debouncing, drop points
			if (component->m_timer.timeElapsed() > component->m_removalDebounce)
				Notify(BossMovementType::DropPoints, data);
		}
	}
}
