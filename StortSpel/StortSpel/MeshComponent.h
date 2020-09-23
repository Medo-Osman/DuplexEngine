#pragma once
//#include "Renderer.h"
#include "Component.h"
#include "Transform.h"
#include "ResourceHandler.h"

//class Renderer;

class MeshComponent : public Component, public Transform
{
private:

	unsigned long int m_renderId;
	MeshResource* m_resourcePointer = nullptr;

public:

	MeshComponent(const char* filepath);

	
	~MeshComponent() {}

	void setRenderId(unsigned long int id) { m_renderId = id; }

	MeshResource* getMeshResourcePtr() { return m_resourcePointer; }

	// Update
	virtual void update(float dt) override {}

};