#pragma once
#include "3DPCH.h"
#include "Entity.h"
#include "Engine.h"
#include "BaseAction.h"
#include "ShootProjectileAction.h"

/*
Attack queue / movement queue, baseclass action tht all possible actions inherit from 

E.g. spawn a meteor

Drop points, but only up to a max, or maybe mark a move as point drop move
*/

class Boss : public BossSubject, public PhysicsObserver
{
private:
	std::map<int, BossObserver*> m_observers;
	std::vector<BossStructures::BaseAction*> m_actionQueue;

	Entity* m_bossEntity = nullptr;
	BossStructures::BaseAction* m_currentAction = nullptr;

	UINT m_uniqueActionID = 0;
public:
	Boss() { };
	~Boss();
	void update();
	void initialize(Entity* entity);
	UINT addAction(BossStructures::BaseAction* action);
	void nextAction();
	BossStructures::BaseAction* getCurrentAction();
	std::vector<BossStructures::BaseAction*>* getActionQueue();
	

	// Inherited via BossSubject
	virtual void Attach(BossObserver* observer) override;
	virtual void Detach(BossObserver* observer) override;
	virtual void Notify(BossMovementType type, BossStructures::BossActionData data) override;

	// Inherited via PhysicsObserver
	virtual void sendPhysicsMessage(PhysicsData& physicsData, bool& destroyEntity) override;
};





