#pragma once
#include "Component.h"
#include "Transform.h"

class SwingComponent : public Component
{
private:
	Transform* m_transform;
	float m_swingSpeed;
	float m_time = 0;

	float ParametricBlend(float t)
	{
		float sqt = t * t;
		return sqt / (2.0f * (sqt - t) + 1.0f);
	}

public:
	SwingComponent(Transform* transform, float swingSpeed)
	{
		m_type = ComponentType::SWING;
		m_transform = transform;
		m_swingSpeed = swingSpeed;
	}

	~SwingComponent() {}

	virtual void update(float dt) override
	{
		m_time += dt;
	}
};