#pragma once
#include "3DPCH.h"
#include <string>

enum class ComponentType { MESH, AUDIO, PHYSICS, CHARACTERCONTROLLER, TRIGGER, TEST, INVALID, UNASSIGNED, ROTATEAROUND, ROTATE, LIGHT, SWEEPING, FLIPPING, CHECKPOINT, TRAP};

class Component
{
protected:
	ComponentType m_type;
	std::string m_parentEntityIdentifier;
	std::string m_identifier;
	std::unordered_map<std::string, Component*> *m_siblingComponentsPtr;

public:
	Component()
	{
		m_type = ComponentType::UNASSIGNED;
		m_siblingComponentsPtr = nullptr;
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

	Component* findSiblingComponentOfType(ComponentType type)
	{
		Component* componentPtr = nullptr;
		if (m_siblingComponentsPtr)
		{
			for (std::pair<std::string, Component*> component : *m_siblingComponentsPtr)
			{
				if (component.second->getType() == type)
				{
					componentPtr = component.second;
				}
			}
		}

		return componentPtr;
	}

	virtual void setComponentMapPointer(std::unordered_map<std::string, Component*>* componentMap)
	{
		m_siblingComponentsPtr = componentMap;
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