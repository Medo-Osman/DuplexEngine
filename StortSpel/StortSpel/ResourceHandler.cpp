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

ID3D11ShaderResourceView* ResourceHandler::loadTexture(const WCHAR* texturePath)
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
			hr = CreateDDSTextureFromFile(m_devicePtr, path.c_str(), nullptr, &srv);
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
					assert(!L"ERROR, error texture can not be found in tetxure folder!");
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
		OutputDebugStringA(modelPath.c_str());
		OutputDebugString(L"\n");
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
		delete element.second;

	for (std::pair<const char*, MeshResource*> element : m_meshCache)
		delete element.second;
}