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
	float m_lifeTime = 0;


public:
	float m_removalDebounce = 0.5f; //seconds before it can be removed when hitting something
	Timer m_timer;
	UINT m_id = 0;

	ProjectileComponent(Entity* entity, Transform* transform, Vector3 origin, Vector3 direction, float speed, float lifeTime)
		:m_origin(origin), m_direction(direction), m_speed(speed), m_lifeTime(lifeTime)
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

	float getLifeTime()
	{
		return m_lifeTime;
	}

	~ProjectileComponent() {}
	virtual void update(float dt) override
	{
		Vector3 somePos = m_physicsComponent->getActorPosition() + m_direction * m_speed * dt;

		m_transform->setPosition(m_physicsComponent->getActorPosition());
		m_physicsPtr->setPosition(m_actor, m_transform->getTranslation());

		m_physicsComponent ? m_physicsComponent->kinematicMove(somePos) : m_transform->setPosition(somePos);

	}
};