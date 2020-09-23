#pragma once
//#include "Buffer.h"
#include <unordered_map>
#include <stdio.h>
#include "Layouts.h"
#include "VertexStructs.h"
#include "MeshResource.h"
//#include "ApplicationLayer.h"

class ResourceHandler
{
private:
	ResourceHandler() {}
public:
	ResourceHandler(const ResourceHandler&) = delete;
	void operator=(ResourceHandler const&) = delete;
	static ResourceHandler& get()
	{
		static ResourceHandler instance;
		return instance;
	}
	
private:

	ID3D11Device* m_devicePtr = NULL;
	ID3D11DeviceContext* m_dContextPtr = NULL;

	bool DeviceAndContextPtrsAreSet = false; //This bool just insures that no one tries to use the resourcehandler before Renderer::initialize has been called
	void isResourceHandlerReady();

	//std::unordered_map<const char*, -Texture pekare här-*> m_TextureCache;
	
	std::unordered_map<const char*, MeshResource*> m_MeshCache;

public:
	
	void setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr);
	

	//-Texture pekare här-* LoadTexture2D(const char* path);

	//bool getTextureByKey(std::wstring key, Texture** texturePtr);

	MeshResource* loadLRMMesh(const char* path);

	void Destroy();
};

