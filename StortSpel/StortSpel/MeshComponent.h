#pragma once
//#include "Renderer.h"
#include "ShaderEnums.h"
#include "Component.h"
#include "Transform.h"
#include "ResourceHandler.h"
#include "Material.h"

//class Renderer;

class MeshComponent : public Component, public Transform
{
private:

	unsigned long int m_renderId;
	MeshResource* m_resourcePointer = nullptr;
	ShaderProgramsEnum m_shaderProgEnum;
	Material m_material;

public:

	MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT, Material material = Material());
	MeshComponent(const char* filepath, Material material);

	~MeshComponent() {}

	void setRenderId(unsigned long int id) { m_renderId = id; }

	void setMeshResourcePtr(MeshResource* newResourcePointer) { m_resourcePointer = newResourcePointer; }
	MeshResource* getMeshResourcePtr() { return m_resourcePointer; }
	ShaderProgramsEnum getShaderProgEnum() { return m_shaderProgEnum; }
	Material* getMaterialPtr();

	// Update
	virtual void update(float dt) override {}

protected:
	MeshComponent(ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT, Material material = Material());

};