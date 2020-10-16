#include "3DPCH.h"
#include "ResourceHandler.h"

ResourceHandler::~ResourceHandler()
{
	Destroy();
}

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

ID3D11ShaderResourceView* ResourceHandler::loadTexture(const WCHAR* texturePath, bool isCubeMap)
{
	if (m_textureCache.count(texturePath))
		return m_textureCache[texturePath];
	else
	{
		isResourceHandlerReady();

		HRESULT hr;
		ID3D11ShaderResourceView* srv = nullptr;
		std::wstring path = m_TEXTURES_PATH + texturePath;

		size_t i = path.rfind('.', path.length());
		std::wstring fileExtension = path.substr(i + 1, path.length() - i);
		if (fileExtension == L"dds" || fileExtension == L"DDS")
		{
			if (isCubeMap == true)
				hr = CreateDDSTextureFromFileEx(m_devicePtr, path.c_str(), 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, NULL, &srv, nullptr);
			else
				hr = CreateDDSTextureFromFile(m_devicePtr, path.c_str(), nullptr, &srv);
		}
		else
			hr = CreateWICTextureFromFile(m_devicePtr, path.c_str(), nullptr, &srv);

		if (FAILED(hr)) // failed to load new texture, return error texture
		{
			if (m_textureCache.count(m_ERROR_TEXTURE_NAME))  // if error texture is loaded
				return m_textureCache[m_ERROR_TEXTURE_NAME.c_str()];
			else // Load error texture
			{
				path = m_TEXTURES_PATH + m_ERROR_TEXTURE_NAME;
				hr = CreateWICTextureFromFile(m_devicePtr, path.c_str(), nullptr, &m_textureCache[m_ERROR_TEXTURE_NAME]);
				if (SUCCEEDED(hr))
					return m_textureCache[m_ERROR_TEXTURE_NAME];
				else
				{
					//std::wstring errorMessage = L"ERROR, '" + m_ERROR_TEXTURE_NAME + L"' texture can not be found in '" + m_TEXTURES_PATH + L"'!";
					ErrorLogger::get().logError("error texture can not be found in texture folder!");
					assert(!L"ERROR, error texture can not be found in texture folder!");
				}
			}
		}
		else
		{
			m_textureCache[texturePath] = srv;
			return srv;
		}
	}
	return nullptr;
}

ID3D11ShaderResourceView* ResourceHandler::loadErrorTexture()
{
	return loadTexture(m_ERROR_TEXTURE_NAME.c_str());
}

MeshResource* ResourceHandler::loadLRMMesh(const char* path)
{
	isResourceHandlerReady();
	
	// checks if the mesh is in the cache 
	if (m_meshCache.find(path) != m_meshCache.end())
	{
		// returns the buffers
		return m_meshCache[path];
	}
	
	// or loads the mesh and makes new buffers
	std::string modelPath = m_MODELS_PATH + path;
	std::ifstream fileStream(modelPath, std::ifstream::in | std::ifstream::binary);

	// Check filestream failure
	if (!fileStream)
	{
		// Error message
		std::string errormsg("loadLRMMesh failed to open filestream: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		// Properly clear and close file buffer
		fileStream.clear();
		fileStream.close();
		
		// If the path that couldn't be opened was the error model path.
		if (path == m_ERROR_MODEL_NAME.c_str())
		{
			ErrorLogger::get().logError("error model can not be found in model folder");
			assert(!L"ERROR, error model can not be found in model folder!");
			return nullptr;
		}
		else
			return loadLRMMesh(m_ERROR_MODEL_NAME.c_str()); // recursively load the error model instead
	}

	// .lrm has 14 floats per vertex
	const std::uint32_t nrOfFloatsInVertex = 14;
	
	// Read file vertex count
	std::uint32_t vertexCount;
	fileStream.read((char*)&vertexCount, sizeof(std::uint32_t));
	
	// Read vertices to array
	float* vertexArray = new float[vertexCount * nrOfFloatsInVertex];
	fileStream.read((char*)&vertexArray[0], sizeof(float) * vertexCount * nrOfFloatsInVertex);

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
		std::string errormsg("loadLRMMesh : Filestream did not reach end of: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		return nullptr;
	}

	// Close filestream
	fileStream.close();

	//Create a new entry in the meshcache
	m_meshCache[path] = new MeshResource;

	//Init it with the data
	m_meshCache[path]->getVertexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, vertexArray, vertexCount, false, sizeof(float) * nrOfFloatsInVertex);
	m_meshCache[path]->getIndexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, indexArray, indexCount);

	delete[] vertexArray;
	delete[] indexArray;

	//Return the pointer of the new entry
	return m_meshCache[path];
}

MeshResource* ResourceHandler::loadLRSMMesh(const char* path)
{
	isResourceHandlerReady();

	// checks if the mesh is in the cache 
	if (m_meshCache.find(path) != m_meshCache.end())
	{
		// returns the buffers
		return m_meshCache[path];
	}

	// or loads the mesh and makes new buffers
	std::string modelPath = m_MODELS_PATH + path;
	std::ifstream fileStream(modelPath, std::ifstream::in | std::ifstream::binary);
	
	std::string thisPath = m_MODELS_PATH + path;
	size_t i = thisPath.rfind('.', thisPath.length());
	std::string fileExtension = thisPath.substr(i + 1, thisPath.length() - i);
	bool correctFileType = (fileExtension == "lrsm" || fileExtension == "LRSM");
	
		// Check filestream failure
	if (!fileStream || !correctFileType)
	{
		// Error message
		std::string errormsg("loadLRMMesh failed to open filestream: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		// Properly clear and close file buffer
		fileStream.clear();
		fileStream.close();

		return loadLRMMesh(m_ERROR_MODEL_NAME.c_str()); // recursively load the error model instead, this'll be weird because it needs another input layout, but whatever
	}

	// .lrm has 14 floats per vertex
	const std::uint32_t nrOfFloatsInVertex = 22;

	// Read file vertex count
	std::uint32_t vertexCount;
	fileStream.read((char*)&vertexCount, sizeof(std::uint32_t));

	// Read vertices to array
	float* vertexArray = new float[vertexCount * nrOfFloatsInVertex];
	fileStream.read((char*)&vertexArray[0], sizeof(float) * vertexCount * nrOfFloatsInVertex);

	// Read file index count
	std::uint32_t indexCount;
	fileStream.read((char*)&indexCount, sizeof(std::uint32_t));

	// Read indices to array
	std::uint32_t* indexArray = new std::uint32_t[indexCount];
	fileStream.read((char*)&indexArray[0], sizeof(std::uint32_t) * indexCount);
	
	// Read file index count
	std::uint32_t jointCount;
	fileStream.read((char*)&jointCount, sizeof(std::uint32_t));
	
	// Read indices to array
	LRSM_JOINT* jointArray = new LRSM_JOINT[jointCount];
	fileStream.read((char*)&jointArray[0], sizeof(LRSM_JOINT) * jointCount);

	// Make sure all data was read
	char overByte;
	fileStream.read(&overByte, 1);
	if (!fileStream.eof())
	{
		std::string errormsg("loadLRMMesh : Filestream did not reach end of: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		return nullptr;
	}

	// Close filestream
	fileStream.close();

	SkeletalMeshResource* thisSkelRes = new SkeletalMeshResource;

	//Init it with the data
	thisSkelRes->getVertexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, vertexArray, vertexCount, false, sizeof(float) * nrOfFloatsInVertex);
	thisSkelRes->getIndexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, indexArray, indexCount);
	thisSkelRes->setJointCount(jointCount);
	thisSkelRes->setJoints(jointArray);

	//Create a new entry in the meshcache
	m_meshCache[path] = thisSkelRes;

	delete[] vertexArray;
	delete[] indexArray;

	//Return the pointer of the new entry
	return m_meshCache[path];
}

void ResourceHandler::setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr)
{
	m_devicePtr = devicePtr;
	m_dContextPtr = dContextPtr;

	DeviceAndContextPtrsAreSet = true;
}

void ResourceHandler::Destroy()
{
	for (std::pair<std::wstring, ID3D11ShaderResourceView*> element : m_textureCache)
		element.second->Release();
		//delete element.second;
	m_textureCache.clear();
	
	/*for (auto it = m_meshCache.cbegin(); it != m_meshCache.cend();)
	{
		m_meshCache.erase(it++);
	}*/

	for (std::pair<const char*, MeshResource*> element : m_meshCache)
	{
		delete element.second;
	}
		
	m_meshCache.clear();
}