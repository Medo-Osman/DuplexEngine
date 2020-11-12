#pragma once
#include "3DPCH.h"
#include "Entity.h"
#include "Engine.h"
#include "BaseAction.h"
#include "ShootProjectileAction.h"
#include "MoveToAction.h"


class Boss : public BossSubject, public PhysicsObserver
{
private:
	std::map<int, BossObserver*> m_observers;
	std::vector<BossStructures::BaseAction*> m_actionQueue;

	Entity* m_bossEntity = nullptr;
	BossStructures::BaseAction* m_currentAction = nullptr;

	UINT m_uniqueActionID = 0;

	//Determines if the action queue will loop, if true the action queue will only be executed once
	//and actions will be deleted once completed.
	bool m_destroyActionOnComplete = true;
public:
	Boss() { };
	~Boss();
	void update(const float& dt);
	void initialize(Entity* entity, bool destroyActionOnComplete = true);
	UINT addAction(BossStructures::BaseAction* action);
	void nextAction();
	BossStructures::BaseAction* getCurrentAction();
	std::vector<BossStructures::BaseAction*>* getActionQueue();

	void setDestroyActionOnComplete(bool value);
	void startActionWithID(UINT id);
	

	// Inherited via BossSubject
	virtual void Attach(BossObserver* observer) override;
	virtual void Detach(BossObserver* observer) override;
	virtual void Notify(BossMovementType type, BossStructures::BossActionData data) override;

	// Inherited via PhysicsObserver
	virtual void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity) override;
};





