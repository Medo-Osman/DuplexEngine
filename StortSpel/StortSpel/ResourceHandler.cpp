#include "3DPCH.h"
#include "ResourceHandler.h"

void ResourceHandler::isResourceHandlerReady()
{
	if (!DeviceAndContextPtrsAreSet)
	{
		// Renderer::initialize needs to be called and it needs to call setDeviceAndContextPtrs()
		// before this function call be called.
		ErrorLogger::get().logError("Problem in ResourceHandler::isResourceHandlerReady(). \nRenderer::initialize needs to be called and it needs to call setDeviceAndContextPtrs() \nbefore this function call be called. ");
		assert(false);
	}
}

void ResourceHandler::setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr)
{
	m_devicePtr = devicePtr;
	m_dContextPtr = dContextPtr;

	DeviceAndContextPtrsAreSet = true;
}

MeshResource* ResourceHandler::loadLRMMesh(const char* path)
{
	isResourceHandlerReady();
	
	// checks if the mesh is in the cache 
	if (m_MeshCache.find(path) != m_MeshCache.end())
	{
		// returns the buffers
		return m_MeshCache[path];
	}
	
	// or loads the mesh and makes new buffers

	std::ifstream fileStream(path, std::ifstream::in | std::ifstream::binary);

	// Check filestream failure
	if (!fileStream)
	{
		// Error message
		//fprintf(stderr, "loadLRMMesh failed to open filestream: %s\n", path);
		OutputDebugString(L"loadLRMMesh failed to open filestream: " );
		OutputDebugStringA(path);
		OutputDebugString(L"\n");
		// Properly clear and close file buffer
		fileStream.clear();
		fileStream.close();

		return nullptr;
	}

	// Read file vertex count
	std::uint32_t vertexCount;
	fileStream.read((char*)&vertexCount, sizeof(std::uint32_t));

	// Read vertices to array
	LRM_VERTEX* vertexArray = new LRM_VERTEX[vertexCount];
	fileStream.read((char*)&vertexArray[0], sizeof(LRM_VERTEX) * vertexCount);

	// Read file index count
	std::uint32_t indexCount;
	fileStream.read((char*)&indexCount, sizeof(std::uint32_t));

	// Read indices to array
	std::uint32_t* indexArray = new std::uint32_t[indexCount];
	fileStream.read((char*)&indexArray[0], sizeof(std::uint32_t) * indexCount);

	// Make sure all data was read
	char overByte;
	fileStream.read(&overByte, 1);
	if (!fileStream.eof())
	{
		OutputDebugString(L"loadLRMMesh : Filestream did not reach end of: ");
		OutputDebugStringA(path);
		OutputDebugString(L"\n");
		return nullptr;
	}

	// Close filestream
	fileStream.close();

	//Depending on how we do vertex data layout, we might want to init that here (OR we can do it in a shader class because it's gonna be per shader, kinda)

	//Create a new entry in the meshcache
	m_MeshCache[path] = new MeshResource;

	//Init it with the data
	m_MeshCache[path]->getVertexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, vertexArray, vertexCount);
	m_MeshCache[path]->getIndexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, indexArray, indexCount);

	delete[] vertexArray;
	delete[] indexArray;

	//Return the pointer of the new entry
	return m_MeshCache[path];
}

void ResourceHandler::Destroy()
{
	/*for (std::pair<const GLchar*, Texture2D*> element : m_TextureCache)
	{
		delete element.second;
	}*/
	for (std::pair<const char*, MeshResource*> element : m_MeshCache)
	{
		delete element.second;
	}
}