#pragma once
#include "Component.h"
#include"TriggerComponent.h"
#include "Traps.h"
class BarrelComponent : public TriggerComponent
{
private:

public:
	BarrelComponent()
		: TriggerComponent()
	{
		m_physicsData.triggerType = TriggerType::BARREL;
		m_physicsData.associatedTriggerEnum = (int)TrapType::BARREL;
		m_physicsData.pointer = this;
	}

	void update(float dt)
	{
  		TriggerComponent::update(dt);
	}
};