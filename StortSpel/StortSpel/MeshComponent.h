#pragma once
//#include "Renderer.h"
#include "ShaderEnums.h"
#include "Component.h"
#include "Transform.h"
#include "ResourceHandler.h"

//class Renderer;

class MeshComponent : public Component, public Transform
{
private:

	unsigned long int m_renderId;
	MeshResource* m_resourcePointer = nullptr;
	ShaderProgramsEnum m_shaderProgEnum;

public:

	MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT);

	~MeshComponent() {}

	void setRenderId(unsigned long int id) { m_renderId = id; }

	MeshResource* getMeshResourcePtr() { return m_resourcePointer; }
	ShaderProgramsEnum getShaderProgEnum() { return m_shaderProgEnum; }

	// Update
	virtual void update(float dt) override {}

};