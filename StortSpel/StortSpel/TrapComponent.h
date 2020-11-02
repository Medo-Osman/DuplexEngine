#pragma once
#include"3DPCH.h"
#include"TriggerComponent.h"

class TrapComponent : public TriggerComponent
{
private:

public:
	TrapComponent()
		:TriggerComponent()
	{
		m_physicsData.triggerType = TriggerType::TRAP;
		m_physicsData.pointer = entity;
		m_type = ComponentType::TRAP;
	}
};