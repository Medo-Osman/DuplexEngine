#pragma once
#include "Component.h"
#include "Transform.h"
#include"PhysicsComponent.h"

class FlippingComponent : public Component
{
private:
	// Input stuff:
	Transform* m_transform;
	float m_upTime;
	float m_downTime;
	float m_flipSpeed;

	// Other:
	float m_time = 0;
	float m_alpha = 0;
	Quaternion m_startRot;
	Quaternion m_endRot;
	bool m_doOnce = true;
	XMMATRIX temp;

	PhysicsComponent* m_physicsComponent;
	

	float ParametricBlend(float t)
	{
		float sqt = t * t;
		return sqt / (2.0f * (sqt - t) + 1.0f);
	}

public:
	FlippingComponent(Transform* transform, float upTime, float downTime, float flipSpeed = 0.5f)
	{
		m_type = ComponentType::FLIPPING;
		this->m_transform = transform;
		this->m_upTime = upTime;
		this->m_downTime = downTime;
		this->m_flipSpeed = flipSpeed;
		m_physicsComponent = nullptr;

	}

	void setComponentMapPointer(std::unordered_map<std::string, Component*>* componentMap)
	{
		Component::setComponentMapPointer(componentMap);
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));
	}

	~FlippingComponent() {}
	virtual void update(float dt) override
	{
		Quaternion slerpedRotation;
		m_time += dt;

		if (m_time >= m_upTime && m_alpha <= 1) // Rotate down:
		{
			if (m_doOnce == true)
			{
				this->m_startRot = Quaternion::CreateFromRotationMatrix(m_transform->getRotationMatrix());
				temp = XMMatrixRotationRollPitchYaw(XMConvertToRadians(180), XMConvertToRadians(0), XMConvertToRadians(0));
				this->m_endRot = Quaternion::CreateFromRotationMatrix(m_transform->getRotationMatrix() * temp);
				m_doOnce = false;
			}

			m_alpha += m_flipSpeed * dt; //               0          180
			slerpedRotation = Quaternion::Slerp(m_startRot, m_endRot, ParametricBlend(m_alpha));
			m_physicsComponent ? m_physicsComponent->kinematicMove({ m_transform->getTranslation() }, slerpedRotation) : m_transform->setRotationQuat(slerpedRotation);
			if (m_alpha >= 1)
			{
				m_time = 0;
				m_doOnce = true;
			}
		}
		if (m_time >= m_downTime && m_alpha >= 1) // Rotate back up
		{
			if (m_doOnce == true)
			{
				this->m_startRot = Quaternion::CreateFromRotationMatrix(m_transform->getRotationMatrix());
				temp = XMMatrixRotationRollPitchYaw(XMConvertToRadians(180), XMConvertToRadians(0), XMConvertToRadians(0));
				this->m_endRot   = Quaternion::CreateFromRotationMatrix(m_transform->getRotationMatrix() * temp);
				m_doOnce = false;
			}

			m_alpha += m_flipSpeed * dt; //               180        361
			slerpedRotation = Quaternion::Slerp(m_startRot, m_endRot, ParametricBlend(m_alpha - 1));
			m_physicsComponent ? m_physicsComponent->kinematicMove({ m_transform->getTranslation() }, slerpedRotation) : m_transform->setRotationQuat(slerpedRotation);

			if (m_alpha >= 2)
			{
				m_time = 0;
				m_alpha = 0;
				m_doOnce = true;
			}
		}

	}
};
