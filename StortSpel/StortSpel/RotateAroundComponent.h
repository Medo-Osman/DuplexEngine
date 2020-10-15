#pragma once
#include "Component.h"
#include "Transform.h"

class RotateAroundComponent : public Component
{
private:
	// Input stuff:
	Transform* m_originTransform;

	Vector3 m_origin;
	XMMATRIX m_rotation;
	Transform* m_transform;
	float m_angle = 0.f;
	float m_radius = 0.f;
	float m_rotationSpeed = 20.f;

	// Other:
	Vector3 m_pos;
	Vector3 m_offset;
	XMMATRIX m_originTransformMatrix;
	XMMATRIX m_rotationMatrix;

public:
	RotateAroundComponent(Transform* origin, XMMATRIX originRotationAxis, Transform* transform, float radius, float rotationSpeed = 20.f, float startAngle = 0.f)
	{
		m_type = ComponentType::ROTATEAROUND;
		this->m_originTransform = origin;

		//this->m_origin		  = origin;
		this->m_rotation	  = originRotationAxis;
		this->m_transform	  = transform;
		this->m_angle		  = startAngle;
		this->m_radius		  = radius;
		this->m_rotationSpeed = rotationSpeed;
	}

	~RotateAroundComponent() {}
	virtual void update(float dt) override 
	{
		// rotate the angle, reste when it hits 0
		m_angle -= m_rotationSpeed * dt;
		if (m_angle <= 0)
		{
			m_angle = 360.f;
		}

		m_offset		  = Vector3(m_radius, 0, 0);
		m_originTransformMatrix = XMMatrixTranslationFromVector(m_originTransform->getTranslation());
		m_rotationMatrix  = XMMatrixRotationY(XMConvertToRadians(m_angle));

		m_pos = XMVector3TransformCoord(m_offset,  m_rotationMatrix * m_rotation * m_originTransformMatrix);

		//Set rotation of the mesh that rotates, so that it matches the parent
		m_transform->setQuaternion(Quaternion::CreateFromRotationMatrix(m_rotation));

		//Set the position around the parent position, according to proper rotation and radius
		m_transform->translation(m_pos);


	}
};