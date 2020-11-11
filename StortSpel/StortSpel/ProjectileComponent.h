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
	float m_removalDebounce = 0.5f; //seconds before it can be removed when hitting something
	Timer m_timer;

	ProjectileComponent(Entity* entity, Transform* transform, Vector3 origin, Vector3 direction, float speed)
		:m_origin(origin), m_direction(direction), m_speed(speed)
	{
		m_timer.start();
		m_timer.restart();
		m_type = ComponentType::PROJECTILE;
		this->m_transform = transform;
		m_physicsData.triggerType = TriggerType::PROJECTILE;
		m_physicsData.associatedTriggerEnum = (int)EventType::BOSS_PROJECTILE_HIT;
		m_physicsData.pointer = entity;
		m_physicsData.entityIdentifier = entity->getIdentifier();

		m_transform->setPosition(origin);

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

		//m_physicsComponent->setVelocity(m_direction*m_speed);
		m_transform->setPosition(m_physicsComponent->getActorPosition());
		m_physicsPtr->setPosition(m_actor, m_transform->getTranslation());

		m_physicsComponent ? m_physicsComponent->kinematicMove(somePos) : m_transform->setPosition(somePos);

		//m_timer.addTime(dt);
	}
};