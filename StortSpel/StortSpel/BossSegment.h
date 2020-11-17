#pragma once
#include "3DPCH.h"
#include "BaseAction.h"
#include "Entity.h"

class BossSegment : public BossSubject
{
protected:
	std::map<int, BossObserver*> m_observers;
	std::vector<BossStructures::BaseAction*> m_actionQueue;
	std::vector<BossSegment*> m_segments;

	Entity* m_bossEntity = nullptr;
	BossStructures::BaseAction* m_currentAction = nullptr;

	UINT m_uniqueActionID = 0;

	//Determines if the action queue will loop, if true the action queue will only be executed once
	//and actions will be deleted once completed.
	bool m_destroyActionOnComplete = true;

	void initializeSegment(Entity* entity, bool destroyActionOnComplete)
	{
		m_bossEntity = entity;
		m_destroyActionOnComplete = destroyActionOnComplete;
	}
public:
	UINT addAction(BossStructures::BaseAction* action)
	{
		//Put at the back of the queue, FIFO applies. Back is actually front in this case as we always take 
		//from the back.
		m_actionQueue.insert(m_actionQueue.begin(), action);
		action->m_actionID = m_uniqueActionID++;

		if (m_currentAction == nullptr)
			nextAction();//m_currentAction = action;

		return action->m_actionID;
	}

	BossStructures::BaseAction* getCurrentAction()
	{
		return m_currentAction;
	}

	void nextAction()
	{
		if (m_actionQueue.size() > 0)
		{
			if (m_destroyActionOnComplete)
			{
				if (m_currentAction)
					delete m_currentAction;

				m_currentAction = m_actionQueue.back();
				m_actionQueue.pop_back();
				m_currentAction->beginAction();
			}
			else
			{
				//m_actionQueue.push_back(m_actionQueue.back());
				if (m_currentAction)
					m_actionQueue.insert(m_actionQueue.begin(), m_currentAction);

				m_currentAction = m_actionQueue.back();
				m_actionQueue.pop_back();
				m_currentAction->beginAction();
			}
		}
		else
		{
			m_currentAction = nullptr;
			ErrorLogger::get().logError(L"Boss segment error - No other action to switch to!");
		}
	}

	virtual void update(const float& dt)
	{
		if (m_currentAction && !m_currentAction->isDone())
		{

			if (!m_currentAction->isDone())
				m_currentAction->update(dt);
		}
		else if (m_actionQueue.size() > 0)
			nextAction();
	}

	// Inherited via BossSubject
	virtual void Attach(BossObserver* observer) override
	{
		m_observers[BossObserver::nr++] = observer;
	}
	
	virtual void Detach(BossObserver* observer) override
	{
		m_observers.erase(observer->index);
	}

	virtual void Notify(BossMovementType type, BossStructures::BossActionData data) override
	{
		for (auto observer : m_observers)
		{
			observer.second->bossEventUpdate(type, data);
		}
	}

	void setDestroyActionOnComplete(bool value)
	{
		m_destroyActionOnComplete = value;
	}

	void startActionWithID(UINT id)
	{
		//Linear search unfortunately - dumb but practical
		for (int i = 0; i < m_actionQueue.size(); i++)
		{
			if (m_actionQueue.at(i)->m_actionID == id)
			{
				m_currentAction = m_actionQueue.at(i);
				m_currentAction->beginAction();
			}
		}
	}
};