#include "3DPCH.h"
#include "Material.h"

Material::Material()
	:m_materialId(0), isDefault(true)
{
	ID3D11ShaderResourceView* errorTexturePtr = ResourceHandler::get().loadErrorTexture();
	for (int i = 0; i < 5; i++)
		this->m_textureArray.push_back(errorTexturePtr);
}

Material::Material(std::initializer_list<const WCHAR*> fileNames, MATERIAL_CONST_BUFFER materialConstData)
	:m_materialId(++totalMaterialCount), isDefault(false)
{
	for (auto fileName : fileNames)
	{
		addTexture(fileName);
	}
	m_materialConstData = materialConstData;
}

Material::Material(const Material& other)
{
	//Medlemsvis kopiering

	this->m_materialId = other.m_materialId;

	this->m_textureArray = other.m_textureArray;

	this->m_materialConstData = other.m_materialConstData;

	this->isDefault = other.isDefault;
}

Material::~Material() {}

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
	if (isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		isDefault = false;
	}
	
	this->m_textureArray.push_back(ResourceHandler::get().loadTexture(fileName, isCubeMap));
}

void Material::setUVScale(float scale)
{
	if (isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		isDefault = false;
	}

	this->m_materialConstData.UVScale = scale;
}

void Material::setRoughness(float roughness)
{
	if (isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		isDefault = false;
	}

	this->m_materialConstData.roughness = roughness;
}

void Material::setMetallic(float metallic)
{
	if (isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		isDefault = false;
	}

	this->m_materialConstData.metallic = metallic;
}

void Material::setTextured(int textured)
{
	if (isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		isDefault = false;
	}

	this->m_materialConstData.textured = textured;
}

void Material::setEmissiveStrength(float emissiveStrength)
{
	if (isDefault)
	{
		this->m_textureArray.clear();
		m_materialId = ++totalMaterialCount;
		isDefault = false;
	}

	this->m_materialConstData.emissiveStrength = emissiveStrength;
}

unsigned int long Material::getMaterialId()
{
	return m_materialId;
}

MATERIAL_CONST_BUFFER Material::getMaterialParameters()
{
	return this->m_materialConstData;
}
