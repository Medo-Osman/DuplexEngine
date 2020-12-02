#pragma once
#include"3DPCH.h"
#include "Component.h"
#include "Transform.h"

class RotateComponent : public Component
{
private:
	Transform* m_transform;
	Quaternion m_originalRotation; // Could be replaced with just a starting value on m_angle

	float m_angle = 0.f;
	float m_rotationSpeed = 20.f;
	PhysicsComponent* m_physicsComponent;

	Vector3 m_rotateVector;

	void init(Transform* origin, Vector3 rotateVector, float rotationSpeed = 5.f)
	{
		m_type = ComponentType::ROTATE;
		this->m_transform = origin;
		m_originalRotation = origin->getRotation();

		this->m_angle = 0.f;
		this->m_rotationSpeed = rotationSpeed;
		m_rotateVector = rotateVector;
	}

public:
	RotateComponent(Transform* origin, Vector3 rotateVector, float rotationSpeed = 5.f)
	{
		init(origin, rotateVector, rotationSpeed);
	}

	RotateComponent(Transform* origin, char* paramData)
	{
		int offset = 0;
		Vector3 rotateVector = readDataFromChar<Vector3>(paramData, offset);
		float rotationSpeed = readDataFromChar<float>(paramData, offset);

		init(origin, rotateVector, rotationSpeed);
	}

	virtual void onSceneLoad() override
	{
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));
		if (m_physicsComponent)
			m_physicsComponent->setSlide(false);
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
		Quaternion quat = XMQuaternionRotationAxis(m_rotateVector, m_angle);
		quat = XMQuaternionMultiply(quat, m_originalRotation);
		//m_transform->setRotation(m_rotateVector, m_angle);

		//m_physicsComponent ? m_physicsComponent->kinematicMove(m_transform->getTranslation(), quat) :
		//	m_transform->setRotationQuat(quat);

		
		
		

		if (m_physicsComponent)
		{
			//Vector3 co = m_physicsComponent->getCenterOffset();
			////Vector3 p = m_physicsComponent->getActorPosition() - co;
			//Vector3 p = m_transform->getTranslation() - co;
			//
			//co = XMVector3Rotate(co, quat);
			//Vector3 o = p + co;

			//m_physicsComponent->kinematicMove(o, quat);
			//
			//m_transform->setPosition(o);
			//m_transform->setRotationQuat(quat);
			m_physicsComponent->kinematicMove(m_physicsComponent->getActorPosition(), quat);
		}
		else
		//m_transform->setPosition(quat);
			m_transform->setRotationQuat(quat);
	}
};