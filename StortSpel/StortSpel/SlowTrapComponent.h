#pragma once
#include"3DPCH.h"
#include"TriggerComponent.h"

class SlowTrapComponent : public TriggerComponent
{
private:
	bool m_isOnTrap = true;
public:
	SlowTrapComponent(Entity* entity, TrapType trapType)
		:TriggerComponent()
	{
		m_physicsData.associatedTriggerEnum = (int)trapType;
		m_physicsData.triggerType = TriggerType::TRAP;
		m_physicsData.pointer = entity;
		m_type = ComponentType::TRAP;
	}


	void update(float dt)
	{
		TriggerComponent::update(dt);
	}
};