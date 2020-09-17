#include "3DPCH.h"
#ifndef COMPONENT_H
#define COMPONENT_H

enum class ComponentType { MESH, AUDIO, PHYSICS, TEST, UNASIGNED };

class Component
{
protected:
	ComponentType m_type;

public:
	Component()
	{
		m_type = ComponentType::UNASIGNED;
	}
	~Component()
	{

	}

	// Getters
	ComponentType getType() const
	{
		return m_type;
	}

	// Update
	virtual void update(float dt) = 0;
};

#endif // !COMPONENT_H