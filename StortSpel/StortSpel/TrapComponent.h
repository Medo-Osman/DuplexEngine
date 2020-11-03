#pragma once
#include"3DPCH.h"
#include"TriggerComponent.h"

class TrapComponent : public TriggerComponent
{
private:
	bool m_isOnTrap = true;
public:
	TrapComponent(Entity* entity)
		:TriggerComponent()
	{
		m_physicsData.associatedTriggerEnum = 1;
		m_physicsData.triggerType = TriggerType::TRAP;
		m_physicsData.pointer = entity;
		m_type = ComponentType::TRAP;
	}

	bool trapActive()
	{
		return m_isOnTrap;
	}

	void setUsing(bool state)
	{
		m_isOnTrap = state;
	}

	void update(float dt)
	{
		TriggerComponent::update(dt);
	}
};