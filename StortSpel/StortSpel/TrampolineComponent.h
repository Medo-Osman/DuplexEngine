#pragma once
#include "Component.h"
#include "Entity.h"
#include"MeshComponent.h"

class TrampolineComponent : public Component
{
private:
	MeshComponent* m_springMesh;
	MeshComponent* m_movingPart;

	const float WAIT_TIME_AT_EXTENDED = 0.13f;
	const float HALF_TIME = 0.25f;
	Vector3 SCALE_MOD = {0.f, -2.3f, 0.f};

	bool m_active;
	bool m_switchedDirection;
	float m_timer;
	Vector3 m_direction;
	Vector3 m_scale;
	Vector3 m_originalPos;

public:
	TrampolineComponent(Entity* entity)
	{
		m_springMesh = (MeshComponent*)entity->getComponent("mesh2");
		m_movingPart = (MeshComponent*)entity->getComponent("mesh3");
		m_direction = { 0, 8, 0 };
		m_switchedDirection = false;
		m_scale = { 1, 1, 1 };
		m_originalPos = m_movingPart->getTranslation();
		m_timer = 0;

	}
	~TrampolineComponent() {}

	// Update
	void moveMeshComponents(const float &dt)
	{
		m_scale += (m_direction + SCALE_MOD) * dt;
		m_movingPart->translate(m_direction * dt);
		m_springMesh->setScale(m_scale);
	}

	virtual void update(float dt) override
	{
		if (m_active)
		{
			m_timer += dt;


				if (m_timer < HALF_TIME)
				{
					moveMeshComponents(dt);
				}
				else if (m_timer < HALF_TIME + WAIT_TIME_AT_EXTENDED)
				{
					if (!m_switchedDirection)
					{
						m_direction *= -1;
						SCALE_MOD *= -1;
						m_switchedDirection = true;
					}

				}
				else if (m_timer < HALF_TIME * 2 + WAIT_TIME_AT_EXTENDED)
				{
					moveMeshComponents(dt);
				}
				else
				{
					m_active = false;
					m_timer = 0.f;
					m_direction *= -1;
					m_scale = { 1.f, 1.f, 1.f };
					SCALE_MOD *= -1;
					m_switchedDirection = false;
					m_springMesh->setScale(1, 1, 1);
					m_movingPart->setPosition(m_originalPos);

				}
		}
	}

	void activate()
	{
		m_active = true;
	}
};