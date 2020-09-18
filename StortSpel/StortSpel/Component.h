#pragma once

class Entity;

enum class ComponentType { MESH, AUDIO, PHYSICS, TEST, INVALID, UNASSIGNED };

class Component
{
protected:
	ComponentType m_type;
	Entity* m_parentEntity;

public:
	Component()
	{
		m_type = ComponentType::UNASSIGNED;
		m_parentEntity = nullptr;
	}
	~Component() {}
	
	// Operators
	bool operator==(const Component& otherComponent)
	{
		return this->m_type == otherComponent.m_type;
	}

	// Getters
	ComponentType getType() const
	{
		return m_type;
	}
	Entity* getParentEntity() const
	{
		return m_parentEntity;
	}

	// Setters
	void setParentEntity(Entity* newParentEntity)
	{
		m_parentEntity = newParentEntity;
	}

	// Update
	virtual void update(float dt) = 0;
};