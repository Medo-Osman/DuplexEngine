#include "3DPCH.h"
#include "MeshComponent.h"


MeshComponent::MeshComponent(const char* filepath, ShaderProgramsEnum shaderEnum, Material material)
	:m_shaderProgEnum(shaderEnum), m_material(Material(material))
{
	m_type = ComponentType::MESH;
	m_resourcePointer = ResourceHandler::get().loadLRMMesh(filepath);

	// If we do material file reading, there needs to be functionality here to check for such a file and read it or use deafult if none is found.
}

MeshComponent::MeshComponent(const char* filepath, Material material)
	:MeshComponent(filepath, ShaderProgramsEnum::DEFAULT, material)
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
