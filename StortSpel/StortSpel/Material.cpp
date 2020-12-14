#include "3DPCH.h"
#include "Material.h"

Material::Material()
	:m_materialId(0), m_isDefault(true)
{
	TextureResource* errorTexturePtr = ResourceHandler::get().loadErrorTexture();
	for (int i = 0; i < 5; i++)
		this->m_textureArray.push_back(errorTexturePtr->view);
}

Material::Material(std::initializer_list<const WCHAR*> fileNames, MATERIAL_CONST_BUFFER materialConstData)
	:m_materialId(++totalMaterialCount), m_isDefault(false)
{
	for (auto fileName : fileNames)
	{
		addTexture(fileName);
	}
	m_materialConstData = materialConstData;
}

Material::Material(std::wstring materialName, bool isPBR)
	/*:Material(m_MaterialCache[materialName])*/
	:m_materialId(m_MaterialCache[materialName].MaterialID), m_isDefault(false)
{
	if (isPBR)
	{
		addTexture(L"skybox_bluesky_2.dds", true);
		addTexture(L"skybox_bluesky_2.dds", true);
		addTexture(L"ibl_brdf_lut.png");
	}
	
	for (auto fileName : m_MaterialCache[materialName].fileNames)
	{
		addTexture(fileName.c_str());
	}

	if (m_MaterialCache[materialName].fileNames.empty())
	{
		m_isDefault = true;
		m_materialId = 0;

		TextureResource* errorTexturePtr = ResourceHandler::get().loadErrorTexture();
		for (int i = 0; i < 5; i++)
			this->m_textureArray.push_back(errorTexturePtr->view);
	}
}

Material::Material(const Material& other)
{
	//Medlemsvis kopiering

	this->m_materialId = other.m_materialId;

	this->m_textureArray = other.m_textureArray;

	this->m_materialConstData = other.m_materialConstData;

	this->m_referencedResources = other.m_referencedResources;

	this->m_isDefault = other.m_isDefault;
}

Material::~Material() 
{

}

void Material::setMaterial(ShaderProgram* shader, ID3D11DeviceContext* dContextPtr)
{
	setMaterial(shader->getShadersNeedsResource(), shader->getShaderNeedsCBuffer(), dContextPtr);
}

void Material::setMaterial(bool shaderNeedsResource[5], bool shaderNeedsCBuffer[5], ID3D11DeviceContext* dContextPtr)
{
	/*if (shaderNeedsCBuffer[ShaderType::Vertex])
		dContextPtr->VSSetConstantBuffers(2, 1, this->constBuffer.GetAddressOf());

	if (shaderNeedsCBuffer[ShaderType::Hull])
		dContextPtr->HSSetConstantBuffers(2, 1, this->constBuffer.GetAddressOf());
	if (shaderNeedsCBuffer[ShaderType::Domain])
		dContextPtr->DSSetConstantBuffers(2, 1, this->constBuffer.GetAddressOf());
	if (shaderNeedsCBuffer[ShaderType::Geometry])
		dContextPtr->GSSetConstantBuffers(2, 1, this->constBuffer.GetAddressOf());
	if (shaderNeedsCBuffer[ShaderType::Pixel])
		dContextPtr->PSSetConstantBuffers(2, 1, this->constBuffer.GetAddressOf());

		To be more like the per model const buffer, there should be one constant buffer in renderer that gets sent in here as opposed
		to the material having its own const buffer.
		But I should ask if there should even be a per material const buffer in the first place.
		(The way that the srvs get sent into set shader res is diffrent, could be trouble.)
	*/

	if (this->m_textureArray.size() < 1)
		return;

	if (shaderNeedsResource[ShaderType::Vertex])
		dContextPtr->VSSetShaderResources(0, (UINT)this->m_textureArray.size(), &this->m_textureArray[0]);

	if (shaderNeedsResource[ShaderType::Hull])
		dContextPtr->HSSetShaderResources(0, (UINT)this->m_textureArray.size(), &this->m_textureArray[0]);

	if (shaderNeedsResource[ShaderType::Domain])
		dContextPtr->DSSetShaderResources(0, (UINT)this->m_textureArray.size(), &this->m_textureArray[0]);

	if (shaderNeedsResource[ShaderType::Geometry])
		dContextPtr->GSSetShaderResources(0, (UINT)this->m_textureArray.size(), &this->m_textureArray[0]);

	if (shaderNeedsResource[ShaderType::Pixel])
		dContextPtr->PSSetShaderResources(0, (UINT)this->m_textureArray.size(), &this->m_textureArray[0]);
	//TODO: check what is already set and if it should be overwritten, maybe might already be in the drivers
}

void Material::addTexture(const WCHAR* fileName, bool isCubeMap)
{
	std::wstring wideString = fileName;
	std::string string = std::string(wideString.begin(), wideString.end());
	//std::cout << "Requiring mat: " << string << std::endl;

	if (m_isDefault)
	{
		this->m_textureArray.clear();
		m_referencedResources.clear();
		m_materialId = ++totalMaterialCount;
		m_isDefault = false;
	}

	TextureResource* loadedTexResource = ResourceHandler::get().loadTexture(fileName, isCubeMap, true);
	//std::cout << "\tMaterial loaded in: " << loadedTexResource->debugName << std::endl;
	//loadedTexResource->addRef();

	this->m_textureArray.push_back(loadedTexResource->view);
	this->m_referencedResources.push_back(loadedTexResource);
}

void Material::swapTexture(const WCHAR* fileName, int index, bool isCubeMap)
{
	m_referencedResources.at(index)->deleteRef(); //Decrement old source
	TextureResource* res = ResourceHandler::get().loadTexture(fileName, isCubeMap);

	m_referencedResources.at(index) = res; //Set new source
	m_referencedResources.at(index)->addRef(); //Add ref to new source
	this->m_textureArray.at(index) = res->view; //Swap the actual texture
}

void Material::setUVScale(float scale)
{
	if (m_isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		m_isDefault = false;
	}

	this->m_materialConstData.UVScale = scale;
}

void Material::setRoughness(float roughness)
{
	if (m_isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		m_isDefault = false;
	}

	this->m_materialConstData.roughness = roughness;
}

void Material::setMetallic(float metallic)
{
	if (m_isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		m_isDefault = false;
	}

	this->m_materialConstData.metallic = metallic;
}

void Material::setTextured(int textured)
{
	if (m_isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		m_isDefault = false;
	}

	this->m_materialConstData.textured = textured;
}

void Material::setEmissiveStrength(float emissiveStrength)
{
	if (m_isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		m_isDefault = false;
	}

	this->m_materialConstData.emissiveStrength = emissiveStrength;
}

void Material::addMaterialRefs()
{
	for (int i = 0; i < m_referencedResources.size(); i++)
	{
		m_referencedResources.at(i)->addRef();
		//std::cout << "\t\tAdding ref to: " << m_referencedResources.at(i)->debugName << ", " << m_referencedResources.at(i)->getRefCount() << ", " << m_referencedResources.at(i) << std::endl;
	}
}

void Material::removeRefs()
{
	for (int i = 0; i < m_referencedResources.size(); i++)
	{
		if (!ResourceHandler::get().m_unloaded)
			m_referencedResources.at(i)->deleteRef();
	}

	m_referencedResources.clear();
	m_textureArray.clear();
}

unsigned int long Material::getMaterialId()
{
	return m_materialId;
}

MATERIAL_CONST_BUFFER Material::getMaterialParameters()
{
	return this->m_materialConstData;
}

void Material::readMaterials()
{
	std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::wstring>> materials;
	std::vector<std::wstring> textureNames;
	std::wstring letters[4] = { L"D", L"E", L"N", L"ORM" };

	for (const auto& file : std::filesystem::directory_iterator(m_TEXTURES_PATH))
	{
		std::wstring filePath = file.path();
		std::wstring fileName = filePath.substr(filePath.find_last_of(L"/") + 1);
		std::wstring rawFileName = L"";
		std::wstring textureName = L"";
		if (fileName.rfind(L"T_", 0) == 0)
		{
			rawFileName = fileName.substr(0, fileName.size() - 4);
			textureName = rawFileName.substr(2);						// Remove start "T_"
			textureName = textureName.substr(0, textureName.find_last_of(L"_")); // Remove ending "_D"
			if (textureName.find_last_of(L"_") != std::wstring::npos)
				textureName = textureName.substr(0, textureName.find_last_of(L"_")); // Remove shaderprog letter "_E"
			bool isTextrue = false;

			for (int l = 0; l < 4; l++)
			{
				std::wstring testVariable1 = rawFileName.substr(rawFileName.size() - (l == 3 ? 4 : 2), std::wstring::npos);
				std::wstring testVariable2 = L"_" + letters[l];

				if (rawFileName.substr(rawFileName.size() - (l == 3 ? 4 : 2), std::wstring::npos) == L"_" + letters[l])
				{
					materials[textureName][letters[l]] = (rawFileName);
					isTextrue = true;
				}
			}
			if (std::find(textureNames.begin(), textureNames.end(), textureName) == textureNames.end() && isTextrue == true) // If unique textureName
			{
				textureNames.push_back(textureName);
			}
		}
	}

	for (int i = 0; i < materials.size(); i++)
	{
		//Material mat;
		MATERIAL_INIT_STRUCT mat;
		mat.MaterialID = ++totalMaterialCount;
		
		for (int l = 0; l < 4; l++)
		{
			if (materials[textureNames[i]].find(letters[l]) == materials[textureNames[i]].end())
			{
				materials[textureNames[i]][letters[l]] = L"T_Missing_" + letters[l];
			}
		}

		for (int l = 0; l < 4; l++)
		{
			std::wstring name = materials[textureNames[i]][letters[l]] + L".png";
			//mat.addTexture(std::wstring(name.begin(), name.end()).c_str());
			mat.fileNames.push_back(name);
		}
		
		m_MaterialCache[textureNames[i]] = mat;
	}
}
