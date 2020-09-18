#pragma once
#include "3DPCH.h"

class Entity
{
private:
	XMFLOAT3 m_scale;
	XMFLOAT3 m_rotation;
	XMFLOAT3 m_position;

	std::unordered_map<std::string, Component*> m_components;

public:
	Entity()
	{
		m_scale		= { 1.f, 1.f, 1.f };
		m_rotation	= { 0.f, 0.f, 0.f };
		m_position	= { 0.f, 0.f, 0.f };
	}
	~Entity()
	{
		for (auto& component : m_components)
			delete component.second;
		m_components.clear();
	}

	// Component Handeling
	void addComponent(std::string newComponentName, Component* newComponent)
	{
		m_components[newComponentName] = newComponent;
		m_components[newComponentName]->setParentEntity(this);
	}

	Component* getComponent(std::string componentName)
	{
		if (m_components.find(componentName) == m_components.end()) // If component does not exist
			return new InvalidComponent();

		return m_components[componentName];
	}
};