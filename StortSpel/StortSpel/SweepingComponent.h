#pragma once
#include "Component.h"
#include "Transform.h"

class SweepingComponent : public Component
{
private:
	Transform* m_transform;
	Vector3 m_startPos;
	Vector3 m_endPos;
	float m_travelTime;
	float m_time = 0;
	int swap = false;

	float ParametricBlend(float t)
	{
		float sqt = t * t;
		return sqt / (2.0f * (sqt - t) + 1.0f);
	}

public:
	SweepingComponent(Transform* transform, Vector3 startPos, Vector3 endPos, float travelTime)
	{
		m_type = ComponentType::SWEEPING;
		this->m_transform = transform;
		this->m_startPos = startPos;
		this->m_endPos = endPos;
		this->m_travelTime = travelTime;

		m_transform->setPosition(m_startPos);
	}

	~SweepingComponent() {}
	virtual void update(float dt) override
	{
		m_time += dt;
		
		if (swap == false) // Move from start to end pos
			m_transform->setPosition(Vector3(XMVectorLerp(m_startPos, m_endPos, ParametricBlend(m_time / m_travelTime))));

		else // Move from end to start pos
			m_transform->setPosition(Vector3(XMVectorLerp(m_endPos, m_startPos, ParametricBlend(m_time / m_travelTime))));


		if (m_time >= m_travelTime)
		{
			m_time = 0;
			
			if (swap == false)
				swap = true;
			else
				swap = false;
		}
	}
};