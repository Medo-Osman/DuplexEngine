#pragma once
#include "Component.h"
#include "Transform.h"
#include "PhysicsComponent.h"

class ProjectileComponent : public TriggerComponent
{
private:
	Transform* m_transform;
	PhysicsComponent* m_physicsComponent = nullptr;

	Vector3 m_origin;
	Vector3 m_direction;
	float m_speed;


public:
	Timer m_timer;
	float m_removalDebounce = 0.5f; //seconds before it can be removed
	ProjectileComponent(Entity* entity, Transform* transform, Vector3 origin, Vector3 direction, float speed)
		:m_origin(origin), m_direction(direction), m_speed(speed)
	{
		m_type = ComponentType::PROJECTILE;
		this->m_transform = transform;
		m_physicsData.triggerType = TriggerType::PROJECTILE;
		m_physicsData.associatedTriggerEnum = (int)EventType::BOSS_PROJECTILE_HIT;
		m_physicsData.pointer = entity;

		m_transform->setPosition(origin);
		/*m_physicsComponent ? m_physicsComponent->kinematicMove(m_startPos) :
									  m_transform->setPosition(m_startPos);*/

		m_timer.start();
	}

	void setComponentMapPointer(std::unordered_map<std::string, Component*>* componentMap)
	{
		Component::setComponentMapPointer(componentMap);
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));
		if (m_physicsComponent)
			m_physicsComponent->setSlide(true);
	}

	~ProjectileComponent() {}
	virtual void update(float dt) override
	{
		Vector3 somePos = m_physicsComponent->getActorPosition() + m_direction * m_speed * dt;

		m_physicsComponent ? m_physicsComponent->kinematicMove(somePos) : m_transform->setPosition(somePos);

		m_timer.addTime(dt);
	}
};