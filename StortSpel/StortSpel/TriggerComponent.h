#pragma once
#include"3DPCH.h"
#include"Component.h"
#include"Physics.h"
#include"Transform.h"



class TriggerComponent : public Component
{
protected:
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
		m_actor = nullptr;
		m_physicsPtr = &Physics::get();

		m_physicsData.triggerType = TriggerType::UNDEFINED;
		m_physicsData.assosiatedTriggerEnum = 1;
		m_physicsData.stringData = "";
		m_physicsData.floatData = 0;
		m_physicsData.intData = 0;
	}
	~TriggerComponent()
	{
		if(m_actor)
			m_physicsPtr->removeActor(m_actor);
	}

	void initTrigger(Transform* transform, XMFLOAT3 boxExtends)
	{
		m_physicsData.entityIdentifier = m_parentEntityIdentifier;
		m_geometryHolder = PxBoxGeometry(boxExtends.x, boxExtends.y, boxExtends.z);
		m_transform = transform;
		m_actor = m_physicsPtr->createRigidActor(transform->getTranslation(), transform->getRotation(), true, &m_physicsData);
		m_physicsPtr->createAndSetShapeForActor(m_actor, m_geometryHolder, "default", false, {1, 1, 1}, true);
	}

	virtual void update(float dt)
	{
		m_physicsPtr->setPosition(m_actor, m_transform->getTranslation());
	}

	const TriggerType& getTriggerType()
	{
		return m_triggerType;
	}

};