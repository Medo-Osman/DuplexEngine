#include "3DPCH.h"
#include "Boss.h"
int BossObserver::nr = 0;

void Boss::update()
{
	if (m_currentAction && !m_currentAction->isDone())
	{
		m_currentAction->update();
	}
	
}

void Boss::initialize(Entity* entity)
{
	m_bossEntity = entity;
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

void Boss::forceNextAction()
{
	m_actionQueue.pop_back();
	m_currentAction = m_actionQueue.back();
	m_currentAction->beginAction();
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
