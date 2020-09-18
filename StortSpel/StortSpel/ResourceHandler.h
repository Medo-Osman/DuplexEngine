#pragma once
#include "Buffer.h"
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

	//std::unordered_map<const char*, -Texture pekare här-*> m_TextureCache;
	
	std::unordered_map<const char*, MeshResource*> m_MeshCache;

public:
	
	//-Texture pekare här-* LoadTexture2D(const char* path);

	//bool getTextureByKey(std::wstring key, Texture** texturePtr);

	MeshResource* loadLRMMesh(const char* path, ID3D11Device* device);

	void Destroy();
};

