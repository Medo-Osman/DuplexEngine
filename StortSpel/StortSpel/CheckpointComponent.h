#pragma once
#include"3DPCH.h"
#include"TriggerComponent.h"

class CheckpointComponent : public TriggerComponent
{
private:
	bool m_isUsed = false;
public:
	CheckpointComponent(Entity* entity)
		:TriggerComponent()
	{
		m_physicsData.triggerType = TriggerType::CHECKPOINT;
		m_physicsData.pointer = entity;
		m_type = ComponentType::CHECKPOINT;
	}

	bool isUsed()
	{
		return m_isUsed;
	}
	void setUsed(bool state)
	{
		m_isUsed = state;
	}


	void update(float dt)
	{
		TriggerComponent::update(dt);
	}
};