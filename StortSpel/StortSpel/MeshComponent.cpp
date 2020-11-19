#include "3DPCH.h"
#include "MeshComponent.h"


MeshComponent::MeshComponent(const char* filepath, std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials)
{
	init(filepath, shaderEnums, materials);
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

MeshComponent::MeshComponent(const char* filepath, Material material)
	: MeshComponent(filepath, { material })
{}

MeshComponent::MeshComponent(char* paramData)
{
	// Read data from package
	int offset = 0;
	
	// Initialize
	init(readStringFromChar(paramData, offset).c_str(), { ShaderProgramsEnum::DEFAULT }, { Material() });
	
	// Read and posibly set offset transform
	bool hasTransformOffset = readDataFromChar<bool>(paramData, offset);

	if (hasTransformOffset)
	{
		Vector3 pos = readDataFromChar<Vector3>(paramData, offset);
		Quaternion rotQuat = readDataFromChar<Quaternion>(paramData, offset);
		Vector3 scale = readDataFromChar<Vector3>(paramData, offset);

		this->setPosition(pos);
		this->setRotationQuat(rotQuat);
		this->setScale(scale);
	}

	/*
	m_filePath = readStringFromChar(paramData, offset);
	m_type = ComponentType::MESH;
	m_resourcePointer = ResourceHandler::get().loadLRMMesh(m_filePath.c_str());
	m_filePath = paramData;
	m_materials.push_back(Material());
	m_shaderProgEnums.push_back(ShaderProgramsEnum::DEFAULT);
	*/
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

MeshComponent::MeshComponent(std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials)
{
	init(shaderEnums, materials);
}

void MeshComponent::init(std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials)
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
}

void MeshComponent::init(const char* filepath, std::initializer_list<ShaderProgramsEnum> shaderEnums, std::initializer_list<Material> materials)
{
	m_type = ComponentType::MESH;
	m_resourcePointer = ResourceHandler::get().loadLRMMesh(filepath);
	m_filePath = filepath;
	for (auto& mat : materials)
	{
		m_materials.push_back(mat);
	}
	for (auto& se : shaderEnums)
	{
		m_shaderProgEnums.push_back(se);
	}
}
