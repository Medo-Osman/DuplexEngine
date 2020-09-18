#pragma once
#include <unordered_map>
#include <stdio.h>
#include "Buffer.h"
#include "Layouts.h"
#include "VertexStructs.h"
//#include "ApplicationLayer.h"

struct MeshResource
{
	Buffer<LRM_VERTEX> vertexBuffer;
	Buffer<std::uint32_t> indexBuffer;

	~MeshResource()
	{
		vertexBuffer.release();
		indexBuffer.release();
	}

	void set(ID3D11DeviceContext* dContext) // ? Ska denna funktionen finnas här och sedan anropas innan draw eller ska koden ligga i en draw funktion 
	{
		UINT offset = 0;
		
		dContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePointer(), &offset);
		dContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
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
	
	std::unordered_map<const char*, MeshResource*> m_MeshCache;

public:
	
	//-Texture pekare här-* LoadTexture2D(const char* path);

	//bool getTextureByKey(std::wstring key, Texture** texturePtr);

	MeshResource* loadLRMMesh(const char* path, ID3D11Device* device);

	void Destroy();
};

