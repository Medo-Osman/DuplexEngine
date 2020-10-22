#pragma once
#include"3DPCH.h"
#include"TriggerComponent.h"
#include"Pickup.h"

class PickupComponent : public TriggerComponent
{
private:

public:
	PickupComponent(Transform* transform, PickupType pickupType, float modifierValue, int duration)
		:TriggerComponent()
	{
		TriggerComponent::initTrigger(transform, { 1.f, 1.f, 1.f });

		m_physicsData.triggerType = TriggerType::PICKUP;
		m_physicsData.assosiatedTriggerEnum = (int)pickupType;
		m_physicsData.floatData = modifierValue;
		m_physicsData.intData = duration;
	}

	void update(float dt)
	{
		TriggerComponent::update(dt);
	}
};