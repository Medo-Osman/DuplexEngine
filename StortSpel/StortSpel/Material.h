#pragma once
#include "ResourceHandler.h"
#include "ShaderProgram.h"

static unsigned int long totalMaterialCount;

struct MATERIAL_CONST_BUFFER // ? Not sure if we should have this like this
{
	float UVScale;
	float roughness;
	float metallic;
	int textured;
};

class Material
{
private:

	std::vector< ID3D11ShaderResourceView* > m_textureArray;
	MATERIAL_CONST_BUFFER m_materialConstData;
	unsigned int long m_materialId;
	bool isDefault;

public:

	Material();
	Material(std::initializer_list<const WCHAR*> fileNames);
	Material(const Material& other);
	~Material();

	void setMaterial(ShaderProgram* shader, ID3D11DeviceContext* dContextPtr);
	void setMaterial(bool shaderNeedsResource[5], bool shaderNeedsCBuffer[5], ID3D11DeviceContext* dContextPtr);
	
	void addTexture(const WCHAR* fileName, bool isCubeMap = false);

	void setUVScale(float scale);
	void setRoughness(float roughness);
	void setMetallic(float metallic);
	void setTextured(int textured);

	unsigned int long getMaterialId();
	MATERIAL_CONST_BUFFER getMaterialParameters();
};

