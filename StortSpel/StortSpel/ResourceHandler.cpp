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
					assert(!"ERROR, error texture can not be found in texture folder!");
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
			assert(!"ERROR, error model can not be found in model folder!");
			return nullptr;
		}
		else
			return loadLRMMesh(m_ERROR_MODEL_NAME.c_str()); // recursively load the error model instead
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
		std::string errormsg("loadLRMMesh : Filestream did not reach end of: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		return nullptr;
	}

	// Close filestream
	fileStream.close();

	//Depending on how we do vertex data layout, we might want to init that here (OR we can do it in a shader class because it's gonna be per shader, kinda)

	//Create a new entry in the meshcache
	m_meshCache[path] = new MeshResource;

	//Init it with the data
	m_meshCache[path]->getVertexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, vertexArray, vertexCount);
	m_meshCache[path]->getIndexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, indexArray, indexCount);

	XMFLOAT3 min = { 99999, 99999, 99999 }, max = { -99999, -99999, -99999 };
	for (int i = 0; i < vertexCount; i++)
	{
		XMFLOAT3 currentPos = vertexArray[i].pos;
		if (currentPos.x >= max.x)
			max.x = currentPos.x;
		if (currentPos.y >= max.y)
			max.y = currentPos.y;
		if (currentPos.z >= max.z)
			max.z = currentPos.z;

		if (currentPos.x <= min.x)
			min.x = currentPos.x;
		if (currentPos.y <= min.y)
			min.y = currentPos.y;
		if (currentPos.z <= min.z)
			min.z = currentPos.z;

	}

	m_meshCache[path]->setMinMax(min, max);
	m_meshCache[path]->storeVertexArray(vertexArray, vertexCount);

	delete[] vertexArray;
	delete[] indexArray;

	//Return the pointer of the new entry
	return m_meshCache[path];
}

SoundEffect* ResourceHandler::loadSound(const WCHAR* soundPath, AudioEngine* audioEngine)
{
	if (!m_soundCache.count(soundPath))
	{
		std::wstring path = m_SOUNDS_PATH + soundPath;
		try
		{
			m_soundCache[soundPath] = new SoundEffect(audioEngine, path.c_str());
		}
		catch (std::exception e) // Error, could not load sound file
		{
			std::wstring error(soundPath);
			error = L"Could not load sound file: " + error;
			ErrorLogger::get().logError(error.c_str());

			if (!m_soundCache.count(m_ERROR_SOUND_NAME)) // if error sound is not loaded
			{
				path = m_SOUNDS_PATH + m_ERROR_SOUND_NAME;
				try
				{
					m_soundCache[m_ERROR_SOUND_NAME] = new SoundEffect(audioEngine, path.c_str());
				}
				catch (std::exception e) // Fatal Error, error sound file does not exist
				{
					ErrorLogger::get().logError("error sound file can not be found in audio folder!");
					assert(!"ERROR, error sound file can not be found in audio folder!");
				}
			}
			return m_soundCache[m_ERROR_SOUND_NAME];
		}
	}
	return m_soundCache[soundPath];
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

	for (std::pair<std::string, MeshResource*> element : m_meshCache)
	{
		delete element.second;
	}
	for (std::pair<std::wstring, SoundEffect*> element : m_soundCache)
	{
		delete element.second;
	}
		
	m_meshCache.clear();
}