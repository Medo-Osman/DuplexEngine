#pragma once
#include "ResourceHandler.h"
#include "ShaderProgram.h"
#include <filesystem>

static unsigned int long totalMaterialCount;

struct MATERIAL_INIT_STRUCT
{
	std::vector<std::wstring> fileNames;
	int MaterialID;
};

struct MATERIAL_CONST_BUFFER
{
	float UVScale			= 1.0f;
	float roughness			= 0.5f;
	float metallic			= 0.f;
	int textured			= 1;
	float emissiveStrength	= 0.f; // 0 to 100
	XMFLOAT3 pad;
};

class Material
{
private:

	std::vector< ID3D11ShaderResourceView* > m_textureArray;
	std::vector<TextureResource*> m_referencedResources;
	MATERIAL_CONST_BUFFER m_materialConstData;
	unsigned int long m_materialId;
	bool m_isDefault;

	bool m_isPRB;
	inline static std::wstring m_currentSkyboxTex = L"";
	inline static ID3D11ShaderResourceView* pbrTextures[3];
	inline static bool pbrIsSet = false;

public:

	Material();
	Material(std::initializer_list<const WCHAR*> fileNames, MATERIAL_CONST_BUFFER materialConstData = MATERIAL_CONST_BUFFER());
	Material(std::wstring materialName, bool isPBR = false);
	Material(const Material& other);
	~Material();

	void setMaterial(ShaderProgram* shader, ID3D11DeviceContext* dContextPtr);
	void setMaterial(bool shaderNeedsResource[5], bool shaderNeedsCBuffer[5], ID3D11DeviceContext* dContextPtr);

	void addTexture(const WCHAR* fileName, bool isCubeMap = false);
	void swapTexture(const WCHAR* fileName, int index, bool isCubeMap = false);

	void setUVScale(float scale);
	void setRoughness(float roughness);
	void setMetallic(float metallic);
	void setTextured(int textured);
	void setEmissiveStrength(float emissiveStrength);
	void addMaterialRefs();
	void removeRefs();

	unsigned int long getMaterialId();
	MATERIAL_CONST_BUFFER getMaterialParameters();

	static std::wstring getCurrentSkybox();
	static void setCurrentSkybox(std::wstring newSkyboxTex, std::wstring newIRTex = L"");

	static void readMaterials();
};

//static std::unordered_map<std::string, Material> m_MaterialCache;
static std::unordered_map<std::wstring, MATERIAL_INIT_STRUCT> m_MaterialCache;
static const std::wstring m_TEXTURES_PATH = L"../res/textures/";


