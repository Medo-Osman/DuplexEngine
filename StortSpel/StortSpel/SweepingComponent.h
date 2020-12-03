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

	//SingleSweep update
	bool m_isMoving;
	bool m_singleSweeps;

	//TimedInterval update - used together with singleSweep
	bool m_timedInterval;
	bool m_startsAtEnd;
	float m_waitTime;
	bool m_stopAtSweep;


	float ParametricBlend(float t)
	{
		float sqt = t * t;
		return sqt / (2.0f * (sqt - t) + 1.0f);
	}

	void init(Transform* transform, Vector3 startPos, Vector3 endPos, float travelTime, bool singleSweeps, bool timedInterval = false, float waitTime = 2.f, bool startsAtEndPos = false)
	{
		m_type = ComponentType::SWEEPING;
		this->m_transform = transform;
		this->m_startPos = startPos;
		this->m_endPos = endPos;
		this->m_travelTime = travelTime;
		this->m_singleSweeps = singleSweeps;
		this->m_timedInterval = timedInterval;
		this->m_waitTime = waitTime;
		this->m_startsAtEnd = startsAtEndPos;
		this->m_stopAtSweep = (singleSweeps && timedInterval);
	}

public:
	SweepingComponent(Transform* transform, Vector3 startPos, Vector3 endPos, float travelTime, bool singleSweeps = false, bool timedInterval = false, float waitTime = 2.f, bool startsAtEndPos = false)
	{
		init(transform, startPos, endPos, travelTime, singleSweeps, timedInterval, waitTime, startsAtEndPos);
	}

	SweepingComponent(char* paramData, Transform* transform)
	{
		// Read data from package
		int offset = 0;

		Vector3 startPos = readDataFromChar<Vector3>(paramData, offset);
		Vector3 endPos = readDataFromChar<Vector3>(paramData, offset);
		flipX(endPos);
		float travelTime = readDataFromChar<float>(paramData, offset);
		bool singleSweeps = readDataFromChar<bool>(paramData, offset);
		bool timeInterval = readDataFromChar<bool>(paramData, offset);
		float waitTime = readDataFromChar<float>(paramData, offset);
		bool startAtEndPos = readDataFromChar<bool>(paramData, offset);

		init(transform, startPos, endPos, travelTime, singleSweeps, timeInterval, waitTime, startAtEndPos);
	}

	void activate()
	{
		m_isMoving = true;
	}

	virtual void onSceneLoad() override
	{
		Vector3 startPos;
		bool usesInterValAndStartAtEnd = (m_timedInterval && m_startsAtEnd);
		m_physicsComponent = dynamic_cast<PhysicsComponent*>(this->findSiblingComponentOfType(ComponentType::PHYSICS));

		usesInterValAndStartAtEnd ? startPos = m_endPos : startPos = m_startPos;
		swap = usesInterValAndStartAtEnd;
		
		if (m_physicsComponent)
		{
			m_physicsComponent->setSlide(false);
			m_physicsComponent->makeKinematic();
		}

		m_physicsComponent ? m_physicsComponent->kinematicMove(startPos, m_transform->getRotation()) :
			m_transform->setPosition(startPos);
		
	}

	~SweepingComponent() {}
	virtual void update(float dt) override
	{
		if (m_singleSweeps && !m_isMoving) 
		{
			if (m_timedInterval)
			{
				m_time += dt;
				if (m_time >= m_waitTime)
				{
					m_isMoving = true;
					m_time = 0;
				}
			}
			return;
		}
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
			{
				if (m_stopAtSweep)
				{
					m_isMoving = false;
				}
				swap = true;
			}
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