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

MeshComponent::MeshComponent(const char* filepath, Material material = Material())
	: MeshComponent(filepath, { material })
{}

MeshComponent::MeshComponent(char* paramData)
{
	// Read data from package
	int offset = 0;
	
	std::string fileName = readStringFromChar(paramData, offset);

	// Read material
	int matCount = readDataFromChar<int>(paramData, offset);
	for (int i = 0; i < matCount; i++)
	{
		std::string strMatName = readStringFromChar(paramData, offset);
		std::wstring matName = std::wstring(strMatName.begin(), strMatName.end());

		size_t indexAt_ = matName.find_last_of(L"_");

		if (indexAt_ != std::string::npos)
		{
			//std::string suffix = matName.substr(indexAt_ + 1, matName.length());
			char suffix = matName.back();
			
			matName = matName.substr(0, indexAt_);
			
			ShaderProgramsEnum sp = charToShaderEnum(suffix);

			m_shaderProgEnums.push_back(sp);
		}
		else
			m_shaderProgEnums.push_back(ShaderProgramsEnum::DEFAULT);
		
		m_materials.push_back(Material(matName));
	}

	if (matCount == 0)
	{
		m_materials.push_back(Material());
		m_shaderProgEnums.push_back(ShaderProgramsEnum::DEFAULT);
	}

	// Initialize
	init(fileName.c_str());
	
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

	m_visible = readDataFromChar<bool>(paramData, offset);
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