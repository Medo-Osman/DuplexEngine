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
	int m_sceneID;

public:
	TriggerComponent()
	{
		m_type = ComponentType::TRIGGER;
		m_transform = nullptr;
		m_actor = nullptr;
		m_physicsPtr = &Physics::get();

		m_physicsData.triggerType = TriggerType::UNDEFINED;
		m_physicsData.associatedTriggerEnum = 1;
		m_physicsData.stringData = "";
		m_physicsData.floatData = 0;
		m_physicsData.intData = 0;
	}
	~TriggerComponent()
	{
		if(m_actor)
			m_physicsPtr->removeActor(m_actor);
	}

	void initTrigger(int sceneID, Transform* transform, XMFLOAT3 boxExtends)
	{
		m_sceneID = sceneID;
		m_physicsData.entityIdentifier = m_parentEntityIdentifier;
		m_geometryHolder = PxBoxGeometry(boxExtends.x, boxExtends.y, boxExtends.z);
		m_transform = transform;
		m_actor = m_physicsPtr->createRigidActor(transform->getTranslation(), transform->getRotation(), true, &m_physicsData, sceneID);
		m_physicsPtr->createAndSetShapeForActor(m_actor, m_geometryHolder, "default", false, {1, 1, 1}, true);
	}

	void setEventData(TriggerType triggerType, int associatedTriggerEnum = 0, void* pointer = nullptr, std::string stringData = "", float floatData = 0.f, int intData = 0)
	{
		m_physicsData.triggerType = triggerType;
		m_physicsData.associatedTriggerEnum = associatedTriggerEnum;
		m_physicsData.pointer = pointer;
		m_physicsData.stringData = stringData;
		m_physicsData.floatData = floatData;
		m_physicsData.intData = floatData;
	}

	void setEventPointer(void* ptr)
	{
		m_physicsData.pointer = ptr;
	}

	void setIntData(int intData)
	{
		m_physicsData.intData = intData;
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