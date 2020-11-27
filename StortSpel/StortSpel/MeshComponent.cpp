#include "3DPCH.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent(const char* filepath, std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials)
{
	m_type = ComponentType::MESH;
	m_resourcePointer = ResourceHandler::get().loadLRMMesh(filepath);
	m_filePath = filepath;
	for (auto& se : shaderEnums)
	{
		m_shaderProgEnums.push_back(se);
	}
	for (auto& mat : materials)
	{
		m_materials.push_back(mat);
	}

	for (int i = 0; i < m_materials.size(); i++)
	{
		m_materials[i].addMaterialRefs();
	}

	m_resourcePointer->addRef();
}

MeshComponent::MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, std::initializer_list<Material> materials)
	:MeshComponent(filepath, { shaderEnum }, materials)
{}

MeshComponent::MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material)
	:MeshComponent(filepath, shaderEnum, { material })
{}

MeshComponent::MeshComponent(const char* filepath, std::initializer_list<Material> materials)
	:MeshComponent(filepath, ShaderProgramsEnum::DEFAULT, materials)
{}

MeshComponent::MeshComponent(const char* filepath, Material material = Material())
	: MeshComponent(filepath, { material })
{}

MeshComponent::MeshComponent(char* paramData)
{
	m_type = ComponentType::MESH;
	m_resourcePointer = ResourceHandler::get().loadLRMMesh(paramData);
	m_resourcePointer->addRef();

	m_filePath = paramData;
	Material mat = Material();
	mat.addMaterialRefs();
	m_materials.push_back(mat);
	m_shaderProgEnums.push_back(ShaderProgramsEnum::DEFAULT);
}

MeshComponent::~MeshComponent()
{
	m_resourcePointer->deleteRef();
	for (int i = 0; i < m_materials.size(); i++)
	{
		m_materials[i].removeRefs();
	}
}

ShaderProgramsEnum MeshComponent::getShaderProgEnum(int index)
{
	if (index >= m_shaderProgEnums.size())
		return m_shaderProgEnums.at(0);

	return m_shaderProgEnums.at(index);
}

Material* MeshComponent::getMaterialPtr(int index)
{
	//assert(index < m_materials.size()); //Assert that the mesh has as many materials as it resource says.

	if (index >= m_materials.size())
		return &m_materials.at(0);

	return &m_materials.at(index);
}

bool MeshComponent::isVisible()
{
	return m_visible;
}

void MeshComponent::setVisible(bool val)
{
	m_visible = val;
}

MeshComponent::MeshComponent(std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials)
{
	m_type = ComponentType::MESH;
	for (auto& mat : materials)
	{
		m_materials.push_back(mat);
	}
	for (auto& se : shaderEnums)
	{
		m_shaderProgEnums.push_back(se);
	}

	for (int i = 0; i < m_materials.size(); i++)
	{
		m_materials[i].addMaterialRefs();
	}
}
