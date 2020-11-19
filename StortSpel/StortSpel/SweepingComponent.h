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
	PhysicsComponent* m_physicsComponent = nullptr;

	bool m_isMoving;
	bool m_singleSweeps;

	float ParametricBlend(float t)
	{
		float sqt = t * t;
		return sqt / (2.0f * (sqt - t) + 1.0f);
	}

	void init(Transform* transform, Vector3 startPos, Vector3 endPos, float travelTime, bool singleSweeps)
	{
		m_type = ComponentType::SWEEPING;
		this->m_transform = transform;
		this->m_startPos = startPos;
		this->m_endPos = endPos;
		this->m_travelTime = travelTime;
		this->m_singleSweeps = singleSweeps;
	}

public:
	SweepingComponent(Transform* transform, Vector3 startPos, Vector3 endPos, float travelTime, bool singleSweeps = false)
	{
		init(transform, startPos, endPos, travelTime, singleSweeps);
	}

	SweepingComponent(char* paramData, Transform* transform)
	{
		// Read data from package
		int offset = 0;
		std::string tempString = readStringFromChar(paramData, offset);

		Vector3 startPos = readDataFromChar<Vector3>(paramData, offset);
		Vector3 endPos = readDataFromChar<Vector3>(paramData, offset);
		float travelTime = readDataFromChar<float>(paramData, offset);
		bool singleSweeps = readDataFromChar<bool>(paramData, offset);

		init(transform, startPos, endPos, travelTime, singleSweeps);
	}

	void activate()
	{
		m_isMoving = true;
	}

	void setComponentMapPointer(std::unordered_map<std::string, Component*>* componentMap)
	{
		Component::setComponentMapPointer(componentMap);
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));
		if (m_physicsComponent)
		{
			m_physicsComponent->setSlide(false);
			m_transform->setPosition(m_startPos);
			m_physicsComponent ? m_physicsComponent->kinematicMove(m_startPos, m_transform->getRotation()) :
				m_transform->setPosition(m_startPos);
		}
	}

	~SweepingComponent() {}
	virtual void update(float dt) override
	{
		if (m_singleSweeps && !m_isMoving) return;
		m_time += dt;

		
		Vector3 somePos(0, 20, 0);
		if (swap == false) // Move from start to end pos
		{
			somePos = Vector3(XMVectorLerp(m_startPos, m_endPos, ParametricBlend(m_time / m_travelTime)));
			m_physicsComponent ? m_physicsComponent->kinematicMove(somePos, m_transform->getRotation()) :
										  m_transform->setPosition(somePos);
			//m_transform->setPosition(Vector3(XMVectorLerp(m_startPos, m_endPos, ParametricBlend(m_time / m_travelTime))));
		}

		else // Move from end to start pos
		{
			somePos = Vector3(XMVectorLerp(m_endPos, m_startPos, ParametricBlend(m_time / m_travelTime)));
			m_physicsComponent ? m_physicsComponent->kinematicMove(somePos, m_transform->getRotation()) :
										  m_transform->setPosition(somePos);
			//m_transform->setPosition(Vector3(XMVectorLerp(m_endPos, m_startPos, ParametricBlend(m_time / m_travelTime))));
		}


		if (m_time >= m_travelTime)
		{
			m_time = 0;

			if (swap == false)
				swap = true;
			else //Back at start
			{
				swap = false;
				if (m_singleSweeps)
				{
					m_isMoving = false;
				}
			}
		}
	}
};