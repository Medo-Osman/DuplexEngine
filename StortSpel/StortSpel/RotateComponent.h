#pragma once
#include"3DPCH.h"
#include "Component.h"
#include "Transform.h"

class RotateComponent : public Component
{
private:
	Transform* m_transform;

	float m_angle = 0.f;
	float m_rotationSpeed = 20.f;

	Vector3 m_rotateVector;
public:
	RotateComponent(Transform* origin, Vector3 rotateVector, float rotationSpeed = 5.f)
	{
		m_type = ComponentType::ROTATE;
		this->m_transform = origin;

		this->m_angle = 0.f;
		this->m_rotationSpeed = rotationSpeed;
		m_rotateVector = rotateVector;
	}

	~RotateComponent() {}
	virtual void update(float dt) override
	{
		// rotate the angle, reste when it hits 0
		m_angle -= m_rotationSpeed * dt;
		if (m_angle <= 0)
		{
			m_angle = 360.f;
		}
		//XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(m_rotateVector * m_angle);

		//m_transform->setRotation(rotMatrix);
		m_transform->setRotation(m_rotateVector, m_angle);
	}
};