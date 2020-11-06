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
	std::string  m_filePath;
	Material m_material;

	bool m_castShadow = true;
public:

	MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT, Material material = Material());
	MeshComponent(const char* filepath, Material material);

	~MeshComponent() {}

	void setRenderId(const unsigned long int id) { m_renderId = id; }
	const unsigned long int& getRenderId() { return m_renderId; }

	void setMeshResourcePtr(MeshResource* newResourcePointer) { m_resourcePointer = newResourcePointer; }
	MeshResource* getMeshResourcePtr() const { return m_resourcePointer; }
	ShaderProgramsEnum getShaderProgEnum() const { return m_shaderProgEnum; }
	const std::string& getFilePath() const { return m_filePath; }
	
	Material* getMaterialPtr();

	// Update
	virtual void update(float dt) override {}

	bool castsShadow()
	{
		return m_castShadow;
	}

	void setCastsShadow(bool value)
	{
		m_castShadow = value;
	}

protected:
	MeshComponent(ShaderProgramsEnum shaderEnum = ShaderProgramsEnum::DEFAULT, Material material = Material());

};