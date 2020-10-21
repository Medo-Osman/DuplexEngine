#pragma once
#include"3DPCH.h"
#include"Component.h"
#include"Physics.h"
#include"Transform.h"

enum class TriggerType
{
	PICKUP,
};

class TriggerComponent : public Component
{
private:
	Physics* m_physicsPtr;
	Transform* m_transform;
	PxRigidActor* m_actor;
	PxGeometryHolder m_geometryHolder;
	TriggerType m_triggerType;

	PhysicsData m_physicsData;

public:
	TriggerComponent()
	{
		m_type = ComponentType::TRIGGER;
		m_transform = nullptr;
		m_physicsPtr = &Physics::get();

		m_physicsData.message = "pickup";
		m_physicsData.stringData = "speed";
		m_physicsData.floatData = 10;
		m_physicsData.intData = 10;
	}

	void initTrigger(Transform* transform, XMFLOAT3 boxExtends)
	{
		m_geometryHolder = PxBoxGeometry(1.f, 1.f, 1.f);
		m_transform = transform;
		m_actor = m_physicsPtr->createRigidActor(transform->getTranslation(), transform->getRotation(), true, &m_physicsData);
		m_physicsPtr->createAndSetShapeForActor(m_actor, m_geometryHolder, "default", false, {1, 1, 1}, true);
	}

	void update(float dt)
	{
		m_physicsPtr->setPosition(m_actor, m_transform->getTranslation());
	}

	const TriggerType& getTriggerType()
	{
		return m_triggerType;
	}

};