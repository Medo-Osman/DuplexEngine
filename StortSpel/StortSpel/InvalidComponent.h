#include "3DPCH.h"
#ifndef INVALIDCOMPONENT_H
#define INVALIDCOMPONENT_H

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
	void update(float dt) {}
};

#endif // !INVALIDCOMPONENT_H