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
	float m_startTime = m_upTime;
	float m_time = 0;
	float m_alpha = 0;
	Quaternion m_startRot;
	Quaternion m_endRot;
	bool m_doOnce = true;

	PhysicsComponent* m_physicsComponent;
	Quaternion slerpedRotation;
	
	float ParametricBlend(float t)
	{
		float sqt = t * t;
		return sqt / (2.0f * (sqt - t) + 1.0f);
	}

	void init(Transform* transform, float upTime, float downTime, float flipSpeed)
	{
		m_type = ComponentType::FLIPPING;
		this->m_transform = transform;
		this->m_upTime = upTime;
		this->m_downTime = downTime;
		this->m_flipSpeed = flipSpeed;
		m_physicsComponent = nullptr;
	}

public:
	FlippingComponent(Transform* transform, float upTime, float downTime, float flipSpeed = 0.5f)
	{
		init(transform, upTime, downTime, flipSpeed);
	}

	FlippingComponent(char* paramData, Transform* transform)
	{
		// Read data from package
		int offset = 0;

		float upTime = readDataFromChar<float>(paramData, offset);
		float downTime = readDataFromChar<float>(paramData, offset);
		float flipSpeed = readDataFromChar<float>(paramData, offset);

		init(transform, upTime, downTime, flipSpeed);
	}

	virtual void onSceneLoad() override
	{
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));
		if (m_physicsComponent)
			m_physicsComponent->setSlide(true);
	}

	~FlippingComponent() {}
	virtual void update(float dt) override
	{
		m_time += dt;

		if (m_time >= m_startTime && m_alpha <= 1)
		{
			if (m_doOnce == true)
			{
				this->m_startRot = Quaternion::CreateFromRotationMatrix(m_transform->getRotationMatrix());
				this->m_endRot = Quaternion::CreateFromYawPitchRoll(0, XMConvertToRadians(180), 0) * m_startRot;
				m_doOnce = false;
			}

			m_alpha += m_flipSpeed * dt; //                    0          180
			
			

			if (m_alpha >= 1)
			{
				m_physicsComponent ? m_physicsComponent->kinematicMove(m_transform->getTranslation(), m_endRot) :
					m_transform->setRotationQuat(m_endRot);
				
				m_time = 0;
				m_alpha = 0;

				if (m_startTime == m_upTime)
					m_startTime = m_downTime;
				else
					m_startTime = m_upTime;

				m_doOnce = true;
			}
			else
			{
				slerpedRotation = Quaternion::Slerp(m_startRot, m_endRot, ParametricBlend(m_alpha));
				m_physicsComponent ? m_physicsComponent->kinematicMove(m_transform->getTranslation(), slerpedRotation) :
					m_transform->setRotationQuat(slerpedRotation);
				//m_transform->setRotationQuat(Quaternion::Slerp(m_startRot, m_endRot, ParametricBlend(m_alpha)));
			}
		}
	}
};
