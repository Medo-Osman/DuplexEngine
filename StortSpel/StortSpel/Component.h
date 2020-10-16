#pragma once
#include "3DPCH.h"
#include <string>

enum class ComponentType { MESH, AUDIO, PHYSICS, TEST, INVALID, UNASSIGNED, ROTATEAROUND, LIGHT, SWEEPING, FLIPPING};

class Component
{
protected:
	ComponentType m_type;
	std::string m_parentEntityIdentifier;
	std::string m_identifier;

public:
	Component()
	{
		m_type = ComponentType::UNASSIGNED;
	}
	virtual ~Component() {}


	// Operators
	bool operator==(const Component& otherComponent)
	{
		return m_type == otherComponent.m_type;
	}

	// Getters
	ComponentType getType() const
	{
		return m_type;
	}
	std::string getParentEntityIdentifier() const
	{
		return m_parentEntityIdentifier;
	}
	std::string getIdentifier()
	{
		return m_identifier;
	}

	void setParentEntityIdentifier(std::string newParentEntity)
	{
		m_parentEntityIdentifier = newParentEntity;
	}
	void setIdentifier(std::string identifier)
	{
		m_identifier = identifier;
	}

	// Update
	virtual void update(float dt) = 0;
};