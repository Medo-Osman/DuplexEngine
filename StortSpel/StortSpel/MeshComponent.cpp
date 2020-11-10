#include "3DPCH.h"
#include "MeshComponent.h"


MeshComponent::MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material)
	:m_shaderProgEnum(shaderEnum), m_material(Material(material))
{
	m_type = ComponentType::MESH;
	m_resourcePointer = ResourceHandler::get().loadLRMMesh(filepath);
	m_filePath = filepath;
}

MeshComponent::MeshComponent(const char* filepath, Material material)
	:MeshComponent(filepath, ShaderProgramsEnum::DEFAULT, material)
{}

MeshComponent::MeshComponent(char* paramData)
	:MeshComponent(paramData, ShaderProgramsEnum::DEFAULT, Material())
{}

Material* MeshComponent::getMaterialPtr()
{
	return &m_material;
}

MeshComponent::MeshComponent(ShaderProgramsEnum shaderEnum, Material material)
	:m_shaderProgEnum(shaderEnum), m_material(Material(material))
{
	m_type = ComponentType::MESH;
}
