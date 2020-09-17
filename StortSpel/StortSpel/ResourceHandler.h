#pragma once
#include <unordered_map>
#include <stdio.h>
#include "Buffer.h"
#include "Layouts.h"
#include "ApplicationLayer.h"

struct MeshBuffers
{
	Buffer<float> vertexBuffer;
	Buffer<std::uint32_t> indexBuffer;

	~MeshBuffers()
	{
		vertexBuffer.release();
		indexBuffer.release();
	}

	void set() // ? Ska denna funktionen finnas här och anropas innan draw eller ska koden ligga i en draw funktion 
	{
		UINT offset = 0;
		GraphicEngine* GE = ApplicationLayer::getInstance().getGraphicsEngine();
		
		GE->getDContext()->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePointer(), &offset);
		GE->getDContext()->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
};

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
	
	std::unordered_map<const char*, MeshBuffers*> m_MeshCache;

public:
	
	//-Texture pekare här-* LoadTexture2D(const char* path);

	//bool getTextureByKey(std::wstring key, Texture** texturePtr);

	MeshBuffers* loadLRMMesh(const char* path);

	void Destroy();
};

