#pragma once
#pragma once
#include "Component.h"
#include"TriggerComponent.h"
#include "Traps.h"
#include "Timer.h"
class BarrelTriggerComponent : public TriggerComponent
{
private:
public:
	Timer m_triggerTimer;
	BarrelTriggerComponent()
		: TriggerComponent()
	{
		m_triggerTimer.restart();
		m_physicsData.triggerType = TriggerType::TRAP;
		m_physicsData.associatedTriggerEnum = (int)TrapType::BARRELTRIGGER;
		m_physicsData.pointer = this;
	}
	


	void update(float dt)
	{
		TriggerComponent::update(dt);
	}
};