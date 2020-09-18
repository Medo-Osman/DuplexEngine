#pragma once
#include "3DPCH.h"
#include "Component.h"

class InvalidComponent : public Component
{
public:
	InvalidComponent()
	{
		m_type = ComponentType::INVALID;
	}
	~InvalidComponent() {}
	
	// Update
	virtual void update(float dt) override {}
};