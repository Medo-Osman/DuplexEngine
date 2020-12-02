#pragma once
#include "Component.h"
#include "Entity.h"

class GrowingComponent : public Component
{
private:
	Vector3 m_originalSize;
	Vector3 m_targetSize;
	bool m_doneTweening = false;

	Entity* m_entity = nullptr;
	float m_speed = 8.f;

public:
	GrowingComponent(Entity* entity, Vector3 sizeTarget, float speed)
	{
		m_type = ComponentType::GROW;
		m_speed = speed;
		m_entity = entity;
		m_targetSize = sizeTarget;

		m_originalSize = entity->getScaling();
	}
	~GrowingComponent() {}

	// Update
	virtual void update(float dt) override
	{
		if (!m_doneTweening)
		{
			m_entity->setScale(m_entity->getScaling().x + 8.f * dt, m_entity->getScaling().y, m_entity->getScaling().z + 8.f * dt);
		}

		if (m_entity->getScaling().x >= m_targetSize.x)
		{
			m_entity->setScale(m_targetSize);
			m_doneTweening = true;
		}
	}

	void setDone(bool value)
	{
		m_doneTweening = value;
	}

	bool doneTweening()
	{
		return m_doneTweening;
	}
};