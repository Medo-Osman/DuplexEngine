#pragma once
#include <string>

enum class ComponentType { MESH, AUDIO, PHYSICS, TEST, INVALID, UNASSIGNED };

class Component
{
protected:
	ComponentType m_type;
	//Entity* m_parentEntity;
	std::string m_parentEntityIdentifier;
	std::string m_identifier;

public:
	Component()
	{
		m_type = ComponentType::UNASSIGNED;
		//m_parentEntity = nullptr;
	}
	~Component() {}

	

	
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