#pragma once
//#include "3DPCH.h"
#include "Component.h"

class InvalidComponent : public Component
{
public:
	InvalidComponent()
	{
		m_type = ComponentType::INVALID;
		std::string errormsg("Creating an invalid component for some reason.");
		ErrorLogger::get().logError(errormsg.c_str());
	}
	~InvalidComponent() {}
	
	// Update
	virtual void update(float dt) override {}
};