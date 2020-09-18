#pragma once
#include "Component.h"
#include "Transform.h"
#include "Renderer.h"

class Renderer;

class MeshComponent : public Component, public Transform
{
private:
	unsigned long int m_renderId;
	MeshResource* m_resourcePointer = nullptr;

public:

	MeshComponent(const char* filepath)
	{
		m_type = ComponentType::MESH;
		m_resourcePointer = ResourceHandler::get().loadLRMMesh(filepath, Renderer::get().getDevice());
		Renderer::get().addMeshComponent(this);
	}
	
	~MeshComponent() {}

	void setRenderId(unsigned long int id) { m_renderId = id; }

	MeshResource* getMeshResourcePtr() { return m_resourcePointer; }

	// Update
	virtual void update(float dt) override {}

};