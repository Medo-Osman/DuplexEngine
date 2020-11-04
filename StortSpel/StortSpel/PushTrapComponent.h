#pragma once
#include "Component.h"
#include "Transform.h"
#include "PhysicsComponent.h"
#include"TriggerComponent.h"

class PushTrapComponent : public TriggerComponent
{
private:
	PhysicsComponent* m_controlledPhysicsComponent;
	bool shouldPush = false;
public:
	PushTrapComponent(Transform* transform, Vector3 startPos, Vector3 endPos, float travelTime, TrapType trapType, Entity* entity)
		:TriggerComponent()
	{
		m_controlledPhysicsComponent = static_cast<PhysicsComponent*>(entity->getComponent("physics"));
		//m_physicsData.associatedTriggerEnum = 2;
		//m_physicsData.triggerType = TriggerType::TRAP;
		m_physicsData.pointer = this;
		//m_type = ComponentType::TRAP;
	}

	void push()
	{
		shouldPush = true;
	}

	void update(float dt)
	{
		TriggerComponent::update(dt);


	}
};