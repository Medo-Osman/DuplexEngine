#pragma once
#include"3DPCH.h"
#include"Component.h"
#include"Entity.h"
#include"Physics.h"

class CharacterControllerComponent : public Component
{
private:
	Physics* m_physicsPtr;
	Transform* m_transform;

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

	void initController(Transform* transform, float height, float radius, std::string material = "default")
	{
		m_transform = transform;
		m_controller = m_physicsPtr->addCapsuleController(m_transform->getTranslation(), height, radius, material);
	}

	void move(XMFLOAT3 moveTowards, const float &dt)
	{
		if (!canUseController())
			return;

		moveTowards.y -= 0.9;
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
		m_transform->translation(XMFLOAT3(pos.x, pos.y, pos.z ));
	}
	
};