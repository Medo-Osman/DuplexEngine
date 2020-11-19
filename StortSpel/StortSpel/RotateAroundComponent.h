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
	bool m_lockRotationToParent;

	// Other:
	Vector3 m_pos;
	Vector3 m_offset;
	XMMATRIX m_originTransformMatrix;
	XMMATRIX m_rotationMatrix;
	PhysicsComponent* m_physicsComponent;

	void init(Transform* origin, XMMATRIX originRotationAxis, Transform* transform, float radius, float rotationSpeed, float startAngle, bool lockRotationToParent)
	{
		m_type = ComponentType::ROTATEAROUND;
		this->m_originTransform = origin;

		//this->m_origin		  = origin;
		this->m_rotation = originRotationAxis;
		this->m_transform = transform;
		this->m_angle = startAngle;
		this->m_radius = radius;
		this->m_rotationSpeed = rotationSpeed;
		this->m_lockRotationToParent = lockRotationToParent;
	}

public:
	RotateAroundComponent(Transform* origin, XMMATRIX originRotationAxis, Transform* transform, float radius, float rotationSpeed = 20.f, float startAngle = 0.f, bool lockRotationToParent = false)
	{
		init(origin, originRotationAxis, transform, radius, rotationSpeed, startAngle, lockRotationToParent);
	}

	RotateAroundComponent(char* paramData, Transform* origin, Transform* transform)
	{
		// Read data from package
		int offset = 0;
		std::string tempString = readStringFromChar(paramData, offset);

		XMMATRIX originRotationAxis = readDataFromChar<XMMATRIX>(paramData, offset);
		float radius = readDataFromChar<float>(paramData, offset);
		float rotationSpeed = readDataFromChar<float>(paramData, offset);
		float startAngle = readDataFromChar<float>(paramData, offset);
		bool lockRotationToParent = readDataFromChar<bool>(paramData, offset);

		init(origin, originRotationAxis, transform, radius, rotationSpeed, startAngle, lockRotationToParent);
	}

	void setComponentMapPointer(std::unordered_map<std::string, Component*>* componentMap)
	{
		Component::setComponentMapPointer(componentMap);
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));
		if (m_physicsComponent)
			m_physicsComponent->setSlide(false);
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

		m_offset = Vector3(m_radius, 0, 0);
		m_originTransformMatrix = XMMatrixTranslationFromVector(m_originTransform->getTranslation());
		m_rotationMatrix = XMMatrixRotationY(XMConvertToRadians(m_angle));

		m_pos = XMVector3TransformCoord(m_offset, m_rotationMatrix * m_rotation * m_originTransformMatrix);

		//Set rotation of the mesh that rotates, so that it matches the parent
		if (m_lockRotationToParent == true)
		{
			m_transform->setRotationQuat(Quaternion::CreateFromRotationMatrix(m_rotation));
			m_physicsComponent ? m_physicsComponent->kinematicMove(m_pos, Quaternion::CreateFromRotationMatrix(m_rotation)) : m_transform->setPosition(m_pos);
		}
		else
			m_physicsComponent ? m_physicsComponent->kinematicMove(m_pos, m_transform->getRotation()) : m_transform->setPosition(m_pos);

	}
};