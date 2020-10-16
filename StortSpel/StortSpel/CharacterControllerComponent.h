#pragma once
#include"3DPCH.h"
#include"Component.h"
#include"Entity.h"
#include"Physics.h"


class CharacterControllerComponent : public Component, public PxControllerBehaviorCallback
{
private:
	Physics* m_physicsPtr;
	Transform* m_transform;
	XMFLOAT3 m_transformOffset;

	PxController* m_controller;
	bool canUseController()
	{
		bool canUse = true;
		if (!m_physicsPtr || !m_transform || !m_controller)
		{
			canUse = false;
			ErrorLogger::get().logError("Trying to use CharacterController without initalizing it beforehand.");
		}
		return canUse;
	}

public:
	CharacterControllerComponent(Physics* physics)
	{
		m_type = ComponentType::PHYSICS;
		m_physicsPtr = physics;
	}

	void initController(Transform* transform, float height, float radius, XMFLOAT3 transformOffset, std::string material = "default")
	{
		m_transform = transform;
		m_controller = m_physicsPtr->addCapsuleController(m_transform->getTranslation(), height, radius, material, this);
		m_transformOffset = transformOffset;
	}

	void move(XMFLOAT3 moveTowards, const float &dt)
	{
		if (!canUseController())
			return;

	//	moveTowards.y -= 9.0f * dt;
		m_controller->move({moveTowards.x, moveTowards.y, moveTowards.z}, 0.001f, dt, NULL, NULL);

	}

	void setPosition(XMFLOAT3 position)
	{
		if (!canUseController())
			return;
		m_controller->setPosition(PxExtendedVec3(position.x, position.y, position.z));
	}

	XMFLOAT3 getFootPosition()
	{
		if (!canUseController())
			return {0.f, 0.f, 0.f};
		PxExtendedVec3 footPos = m_controller->getFootPosition();
		return XMFLOAT3(footPos.x, footPos.y, footPos.z);
	}

	void setControllerSize(float height)
	{
		if (!canUseController())
			return;
		m_physicsPtr->setCapsuleSize(m_controller, height);
	}

	void setControllerRadius(float radius)
	{
		if (!canUseController())
			return;
		m_physicsPtr->setCapsuleRadius(m_controller, radius);
	}

	void update(float dt)
	{
		if (!canUseController())
			return;
		PxExtendedVec3 pos = m_controller->getPosition();
		XMFLOAT3 position = XMFLOAT3(pos.x + m_transformOffset.x, pos.y + m_transformOffset.y, pos.z + m_transformOffset.z);

		m_transform->translation(XMFLOAT3(position.x, position.y, position.z ));
	}
	
	bool checkGround(Vector3 origin, Vector3 unitDirection, float distance)
	{
		return m_physicsPtr->castRay(origin, unitDirection, distance);
	}

	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor)
	{
		return PxControllerBehaviorFlags();
	}
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxController& controller)
	{
		return PxControllerBehaviorFlags();

	}
	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxObstacle& obstacle)
	{
		return PxControllerBehaviorFlags();
	}
};