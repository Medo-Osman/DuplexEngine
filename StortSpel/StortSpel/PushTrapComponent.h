#pragma once
#include "Component.h"
#include "Transform.h"
#include "SweepingComponent.h"
#include"TriggerComponent.h"



class PushTrapComponent : public TriggerComponent
{
private:
	SweepingComponent* m_controlledSweepingComponent;
	bool shouldPush = false;
public:
	PushTrapComponent(Entity* entity)
		:TriggerComponent()
	{
		m_controlledSweepingComponent = static_cast<SweepingComponent*>(entity->getComponent("physics"));
		//m_physicsData.associatedTriggerEnum = 2;
		//m_physicsData.triggerType = TriggerType::TRAP;
		m_physicsData.pointer = this;
		//m_type = ComponentType::TRAP;
	}

	void push()
	{
		shouldPush = true;
		m_controlledSweepingComponent->activate();
	}

	void update(float dt)
	{
		TriggerComponent::update(dt);


	}
};