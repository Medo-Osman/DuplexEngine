#pragma once
#include"3DPCH.h"
#include"Component.h"
#include"Physics.h"
#include"PhysicsComponent.h"


class CharacterControllerComponent : public Component, public PxControllerBehaviorCallback
{
private:
	Physics* m_physicsPtr;
	Transform* m_transform;
	XMFLOAT3 m_transformOffset;

	PxController* m_controller;
	bool canUseController() const
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
	CharacterControllerComponent()
	{
		m_type = ComponentType::PHYSICS;
		m_transform = nullptr;
		m_controller = nullptr;
		m_physicsPtr = &Physics::get();
		m_transformOffset = { 0.f, 0.f, 0.f };
	}

	void initController(Transform* transform, const float& height, const float &radius, const XMFLOAT3 &transformOffset, std::string material = "default")
	{
		m_transform = transform;
		m_controller = m_physicsPtr->addCapsuleController(m_transform->getTranslation(), height, radius, material, this);
		m_transformOffset = transformOffset;
	}

	void move(const XMFLOAT3 &moveTowards, const float &dt)
	{
		if (!canUseController())
			return;

		m_controller->move({moveTowards.x, moveTowards.y, moveTowards.z}, 0.001f, dt, NULL, NULL);

	}

	void setPosition(const XMFLOAT3 &position)
	{
		if (!canUseController())
			return;
		m_controller->setPosition(PxExtendedVec3(position.x, position.y, position.z));
	}

	XMFLOAT3 getFootPosition() const
	{
		if (!canUseController())
			return {0.f, 0.f, 0.f};
		PxExtendedVec3 footPos = m_controller->getFootPosition();
		return XMFLOAT3((float)footPos.x, (float)footPos.y, (float)footPos.z);
	}

	void setControllerSize(const float &height)
	{
		if (!canUseController())
			return;
		m_physicsPtr->setCapsuleSize(m_controller, height);
	}

	void setControllerRadius(const float &radius)
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
		XMFLOAT3 position = XMFLOAT3((float)pos.x + m_transformOffset.x, (float)pos.y + m_transformOffset.y, (float)pos.z + m_transformOffset.z);

		m_transform->setPosition(XMFLOAT3(position.x, position.y, position.z ));
	}
	
	bool checkGround(const Vector3 &origin, const Vector3 &unitDirection, const float &distance) const
	{
		return m_physicsPtr->castRay(origin, unitDirection, distance);
	}

	virtual PxControllerBehaviorFlags getBehaviorFlags(const PxShape& shape, const PxActor& actor) //Controller colides with shapes
	{
		return static_cast<PhysicsComponent*>(actor.userData)->getBehaviorFlag();
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