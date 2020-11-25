#pragma once
#include "Component.h"
#include "Transform.h"

class SwingComponent : public Component
{
private:
	Transform* m_transform;
	float m_swingSpeed;
	float m_time = 0;
	Vector3 m_initialRot;

	float m_rotationDirection = 1;
	float m_alpha = 0;
	bool m_doOnce = true;
	PhysicsComponent* m_physicsComponent;
	Quaternion start;
	Quaternion end;
	float ParametricBlend(float t)
	{
		float sqt = t * t;
		return sqt / (2.0f * (sqt - t) + 1.0f);
	}

public:
	SwingComponent(Transform* transform, Vector3 orginRotAsDeg, float swingSpeed)
	{
		m_type = ComponentType::SWING;
		m_transform = transform;
		m_swingSpeed = swingSpeed;
		m_initialRot = orginRotAsDeg;
	}

	void setComponentMapPointer(std::unordered_map<std::string, Component*>* componentMap)
	{
		Component::setComponentMapPointer(componentMap);
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));
		if (m_physicsComponent)
			m_physicsComponent->setSlide(false);
	}

	~SwingComponent() {}

	virtual void update(float dt) override
	{
		if (m_doOnce == true)
		{
			m_rotationDirection *= -1;

			start = Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(m_initialRot.y),
													   XMConvertToRadians(m_initialRot.x),
													   XMConvertToRadians(89 * m_rotationDirection));
																						   
			end   = Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(m_initialRot.y),
													   XMConvertToRadians(m_initialRot.x),
													   XMConvertToRadians(89 * m_rotationDirection * -1));

			m_doOnce = false;
		}
		m_alpha += dt * m_swingSpeed;
		Quaternion q = Quaternion::Slerp(start, end, ParametricBlend(m_alpha));
		
		m_physicsComponent ? m_physicsComponent->kinematicMove(m_transform->getTranslation(), q) :
			m_transform->setRotationQuat(q);

		if (m_alpha >= 1 || m_alpha <= -1)
		{
			m_alpha = 0;
			m_doOnce = true;
		}
	}
};