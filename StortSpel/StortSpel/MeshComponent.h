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
	std::vector<ShaderProgramsEnum> m_shaderProgEnums;
	std::vector<Material> m_materials;

	bool m_castShadow = true;
	bool m_visible = true;
	
protected:
	std::string m_filePath;

public:

	MeshComponent(const char* filepath, std::initializer_list<ShaderProgramsEnum> shaderEnums = { ShaderProgramsEnum::DEFAULT }, std::initializer_list<Material> materials = { Material() });
	MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, std::initializer_list<Material> materials = { Material() });
	MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material);
	MeshComponent(const char* filepath, std::initializer_list<Material> materials);
	MeshComponent(const char* filepath, Material material);
	MeshComponent(char* paramData);

	~MeshComponent();

	void setRenderId(const unsigned long int id) { m_renderId = id; }
	const unsigned long int& getRenderId() { return m_renderId; }

	void setMeshResourcePtr(MeshResource* newResourcePointer) { m_resourcePointer = newResourcePointer; }
	MeshResource* getMeshResourcePtr() const { return m_resourcePointer; }
	const std::string& getFilePath() const { return m_filePath; }
	
	ShaderProgramsEnum getShaderProgEnum(int index);
	Material* getMaterialPtr(int index);

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

	bool isVisible();
	void setVisible(bool val);

protected:
	MeshComponent(std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials = { Material() });
	MeshComponent() {};

	void init(std::initializer_list<ShaderProgramsEnum> shaderEnums = { ShaderProgramsEnum::DEFAULT }, std::initializer_list<Material> materials = { Material() });
	void init(const char* filepath, std::initializer_list<ShaderProgramsEnum> shaderEnums = { }, std::initializer_list<Material> materials = { });
};