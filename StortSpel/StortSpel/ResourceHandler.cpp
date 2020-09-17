#include "ResourceHandler.h"

MeshBuffers* ResourceHandler::loadLRMMesh(const char* path)
{
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
		fprintf(stderr, "loadLRMMesh failed to open filestream: %s\n", path);

		// Properly clear and close file buffer
		fileStream.clear();
		fileStream.close();

		return nullptr;
	}

	// Vertex format description
	// .abm has 12 floats per vertex
	const int vertexSize = 12;

	// Read file vertex count
	std::uint32_t vertexCount;
	fileStream.read((char*)&vertexCount, sizeof(std::uint32_t));

	// Read vertices to array
	float* vertexArray = new float[vertexCount * vertexSize];
	fileStream.read((char*)&vertexArray[0], sizeof(float) * vertexCount * vertexSize);

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
		fprintf(stderr, "loadLRMMesh : Filestream did not reach eof: %s\n", path);
		return nullptr;
	}

	// Close filestream
	fileStream.close();

	//Depending on how we do vertex data layout, we might want to init that here (OR we can do it in a shader class cause it's gonna be per shader, kinda)

	//HRESULT hr = m_devicePtr->CreateInputLayout(Layouts::LRMVertexLayout, //VertexLayout
	//	ARRAYSIZE(Layouts::LRMVertexLayout), //Nr of elements 
	//	m_vertexShaderBufferPtr->GetBufferPointer(),
	//	m_vertexShaderBufferPtr->GetBufferSize(), //Bytecode length
	//	m_vertexLayoutPtr.GetAddressOf()
	//);

	//Create a new entry in the meshcache
	m_MeshCache[path] = new MeshBuffers;

	//Init it with the data
	GraphicEngine* GE = ApplicationLayer::getInstance().getGraphicsEngine();
	m_MeshCache[path]->vertexBuffer.initializeBuffer(GE->getDevice(), false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, vertexArray, vertexCount);
	m_MeshCache[path]->indexBuffer.initializeBuffer(GE->getDevice(), false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, indexArray, indexCount);

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
	for (std::pair<const char*, MeshBuffers*> element : m_MeshCache)
	{
		delete element.second;
	}
}