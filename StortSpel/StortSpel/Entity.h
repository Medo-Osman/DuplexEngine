#pragma once
#include "3DPCH.h"
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

		//std::vector<std::string> stringsToRemove;
		//std::cout << m_identifier << std::endl;
		for (auto& component : m_components)
		{
			//std::cout << "\t" << component.second->getIdentifier() << std::endl;

			//stringsToRemove.push_back(component.first);
			delete component.second;
			component.second = nullptr;
		}

		/*for (int i = 0; i < stringsToRemove.size(); i++)
		{
			m_components.erase(stringsToRemove.at(i));
			delete m_components[stringsToRemove.at(i)];
		}*/
		

		m_components.clear();
	}

	void onSceneLoad()
	{
		for (auto component : m_components)
			component.second->onSceneLoad();
	}

	void update(const float &dt)
	{
		for (auto& component : m_components)
			component.second->update(dt);
	}

	std::string getIdentifier()
	{
		return m_identifier;
	}

	// Component Handling
	void addComponent(std::string newComponentName, Component* newComponent)
	{
		newComponent->setIdentifier(newComponentName);
		newComponent->setParentEntityIdentifier(m_identifier);
		m_components[newComponentName] = newComponent;
		m_components[newComponentName]->setComponentMapPointer(&m_components);
	}
	
	void removeComponent(Component* component)
	{
		int deleted = (int)m_components.erase(component->getIdentifier());
	}

	Component* getComponent(std::string componentName)
	{
		if (m_components.find(componentName) == m_components.end()) // If component does not exist
		{
			//ErrorLogger::get().logError("Attempt to retrieve component by name failed, does not exist.");

			//No longer an error, it is a valid way to check if a component exists externally.
			return nullptr;
		}
			
		return m_components[componentName];
	}
	
	void getComponentsOfType(std::vector<Component*> &compVec, ComponentType type)
	{
		for (std::pair<std::string, Component*> component : m_components)
		{
			if (component.second->getType() == type)
			{
				compVec.push_back(component.second);
			}
		}
	}

	bool hasComponentsOfType(ComponentType type)
	{
		bool hasType = false;

		for (std::pair<std::string, Component*> component : m_components)
			if (component.second->getType() == type)
			{
				hasType = true;
				break;
			}

		return hasType;
	}

	bool m_canCull = true;
};