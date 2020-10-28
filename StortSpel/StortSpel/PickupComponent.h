#pragma once
#include"3DPCH.h"
#include"TriggerComponent.h"
#include"Pickup.h"

class PickupComponent : public TriggerComponent
{
private:

public:
	PickupComponent(PickupType pickupType, float modifierValue, int duration)
		:TriggerComponent()
	{
		m_physicsData.triggerType = TriggerType::PICKUP;
		m_physicsData.associatedTriggerEnum = (int)pickupType;
		m_physicsData.floatData = modifierValue;
		m_physicsData.intData = duration;
	}

	void update(float dt)
	{
		TriggerComponent::update(dt);
	}
};