#pragma once
//#include "3DPCH.h"
#include "Transform.h"
#include "Component.h"
#include "InvalidComponent.h"
#include <unordered_map>

class Entity : public Transform
{
private:

	std::string m_identifier;

	std::unordered_map<std::string, Component*> m_components;

public:
	Entity(std::string identifier) { m_identifier = identifier; }
	~Entity()
	{
		for (auto& component : m_components)
			delete component.second;
		m_components.clear();
	}

	// Component Handling
	void addComponent(std::string newComponentName, Component* newComponent)
	{
		//newComponentName += std::to_string(m_components.size());
		newComponent->setIdentifier(newComponentName);
		newComponent->setParentEntityIdentifier(m_identifier);
		m_components[newComponentName] = newComponent;
	}

	Component* getComponent(std::string componentName)
	{
		if (m_components.find(componentName) == m_components.end()) // If component does not exist
			return new InvalidComponent();

		return m_components[componentName];
	}
};