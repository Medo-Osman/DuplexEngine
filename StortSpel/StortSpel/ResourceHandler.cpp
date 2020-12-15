#include "3DPCH.h"
#include "ResourceHandler.h"

ResourceHandler::~ResourceHandler()
{
	m_unloaded = true;
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

void ResourceHandler::checkResources()
{
	/*
	//=========================================== Meshes
	std::vector<std::string> idsForMeshesToRemove;
	for (auto meshStruct : m_meshCache)
	{
		if (meshStruct.second->getRefCount() == 0)
		{
			idsForMeshesToRemove.push_back(meshStruct.first);
		}

	}

	for (int i = 0; i < idsForMeshesToRemove.size(); i++)
	{
		//std::cout << "Removing mesh: " << m_meshCache[idsForMeshesToRemove[i]]->debugName << std::endl;
		delete m_meshCache[idsForMeshesToRemove[i]];
		m_meshCache.erase(idsForMeshesToRemove[i]);
	}

	idsForMeshesToRemove.clear();
	//===========================================

	//=========================================== Textures
	std::vector<std::wstring> textureIdsToRemove;
	for (auto textureStruct : m_textureCache)
	{
		if (textureStruct.second->getRefCount() == 0 && textureStruct.second->m_doReferenceCount)
		{
			textureIdsToRemove.push_back(std::wstring(textureStruct.first.begin(), textureStruct.first.end()));

		}

	}

	for (int i = 0; i < textureIdsToRemove.size(); i++)
	{
		std::string name = m_textureCache[textureIdsToRemove[i]]->debugName;
		//std::cout << "Deleting texture " << name << ", " << m_textureCache[textureIdsToRemove[i]]->getRefCount() << std::endl;
		//m_textureCache[textureIdsToRemove[i]]->Release();
		delete m_textureCache[textureIdsToRemove[i]];
		m_textureCache.erase(textureIdsToRemove[i]);
	}

	//=========================================== 

	//=========================================== Audio
	std::vector<std::wstring> audioIdsToRemove;
	for (auto audioStruct : m_soundCache)
	{
		if (audioStruct.second->getRefCount() == 0)
		{
			audioIdsToRemove.push_back(audioStruct.first);
		}
	}

	for (int i = 0; i < audioIdsToRemove.size(); i++)
	{
		delete m_soundCache[audioIdsToRemove[i]];
		m_soundCache.erase(audioIdsToRemove[i]);
	}
	//=========================================== 

	if (DEBUGMODE)
	{
		std::cout << std::endl;
		std::cout << "mesh: " << m_meshCache.size() << std::endl;
		std::cout << "texture: " << m_textureCache.size() << std::endl;
		std::cout << "audio: " << m_soundCache.size() << std::endl;
	}


	//for (auto& m : m_textureCache)
		//if (m.second->m_doReferenceCount)
			//std::cout << m.second->debugName << ", " << m.second->getRefCount() << ", " << m.second->view << std::endl;
	*/
}

TextureResource* ResourceHandler::loadTexture(std::wstring texturePath, bool isCubeMap, bool referenceBasedDelete)
{
	std::wstring wideString = texturePath;
	std::string string = std::string(wideString.begin(), wideString.end());


	if (m_textureCache.count(texturePath))
	{
		return m_textureCache[texturePath];
	}
	else
	{
		isResourceHandlerReady();

		std::lock_guard<std::mutex> lock(m_lock);
		HRESULT hr;
		ID3D11ShaderResourceView* srv = nullptr;
		std::wstring path = m_TEXTURES_PATH + texturePath;
		m_textureCache[texturePath] = new TextureResource();

		size_t i = path.rfind('.', path.length());
		std::wstring fileExtension = path.substr(i + 1, path.length() - i);
		if (fileExtension == L"dds" || fileExtension == L"DDS")
		{
			if (isCubeMap == true)
			{
				//hr = CreateDDSTextureFromFileEx(m_devicePtr, path.c_str(), 0, D3D11_USAGE_IMMUTABLE, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, NULL, &srv, nullptr);
				hr = CreateDDSTextureFromFileEx(m_devicePtr, m_deferredDContextPtr, path.c_str(), 5, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_TEXTURECUBE, false, NULL, &srv, nullptr);
				m_deferredDContextPtr->FinishCommandList(TRUE, &m_commandList);
			}
			else
			{
				hr = CreateDDSTextureFromFile(m_devicePtr, path.c_str(), nullptr, &srv);
			}

		}
		else
			hr = CreateWICTextureFromFile(m_devicePtr, path.c_str(), nullptr, &srv);

		if (FAILED(hr)) // failed to load new texture, return error texture
		{
			if (m_textureCache.count(m_ERROR_TEXTURE_NAME))  // if error texture is loaded
				return m_textureCache[m_ERROR_TEXTURE_NAME.c_str()];
			else // Load error texture
			{
				//m_textureCache[m_ERROR_TEXTURE_NAME] = new TextureResource();
				path = m_TEXTURES_PATH + m_ERROR_TEXTURE_NAME;
				hr = CreateWICTextureFromFile(m_devicePtr, path.c_str(), nullptr, &m_textureCache[m_ERROR_TEXTURE_NAME]->view);
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
			std::wstring wideString = texturePath;
			m_textureCache[texturePath]->view = srv;
			m_textureCache[texturePath]->m_doReferenceCount = referenceBasedDelete;
			m_textureCache[texturePath]->debugName = std::string(wideString.begin(), wideString.end());
			return m_textureCache[texturePath];
		}
	}
	//return nullptr;
}

TextureResource* ResourceHandler::loadErrorTexture()
{
	return loadTexture(m_ERROR_TEXTURE_NAME.c_str());
}

MeshResource* ResourceHandler::loadLRMMesh(const char* path)
{

	isResourceHandlerReady();
	int num = (int)m_meshCache.count(path);
	// checks if the mesh is in the cache 
	auto it = m_meshCache.find(path);
	if (it != m_meshCache.end())
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
	
	std::uint32_t materialCount = 1;
	std::uint32_t* materialOffsets = nullptr;

	char overByte1;
	fileStream.read(&overByte1, 1);
	if (!fileStream.eof())
	{
		fileStream.seekg(-1, std::ios_base::cur);

		// Read file material count
		fileStream.read((char*)&materialCount, sizeof(std::uint32_t));

		if (materialCount > 1)
		{
			// Read materialOffsets to array
			materialOffsets = new std::uint32_t[materialCount];
			fileStream.read((char*)&materialOffsets[0], sizeof(std::uint32_t) * materialCount);
		}

		// Make sure all data was read
		char overByte;
		fileStream.read(&overByte, 1);
		if (!fileStream.eof())
		{
			std::string errormsg("loadLRMMesh : Filestream did not reach end of: "); errormsg.append(path);
			ErrorLogger::get().logError(errormsg.c_str());
			return nullptr;
		}
	}
	//else
	//	fileStream.seekg(-1, std::ios_base::cur);

	

	// Close filestream
	fileStream.close();

	//Create a new entry in the meshcache
	m_meshCache[path] = new MeshResource;

	//Init it with the data
	m_meshCache[path]->getVertexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, vertexArray, vertexCount, false, sizeof(float) * nrOfFloatsInVertex);
	m_meshCache[path]->getIndexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, indexArray, indexCount);
	if (materialCount > 1)
		m_meshCache[path]->setMaterialOffsetsVector(materialOffsets, materialCount);

	LRM_VERTEX* vertexArray2 = (LRM_VERTEX*)vertexArray;
	XMFLOAT3 min = { 99999, 99999, 99999 }, max = { -99999, -99999, -99999 };
	for (unsigned int i = 0; i < vertexCount; i++)
	{
		XMFLOAT3 currentPos = vertexArray2[i].pos;
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
	m_meshCache[path]->storeVertexArray(vertexArray2, vertexCount);
	m_meshCache[path]->storeIndexArray(indexArray, indexCount);
	m_meshCache[path]->storeFilePath(path);

	delete[] vertexArray;
	delete[] indexArray;
	if(materialOffsets != nullptr)
		delete[] materialOffsets;

	m_meshCache[path]->debugName = path;
	//Return the pointer of the new entry
	return m_meshCache[path];
}

MeshResource* ResourceHandler::loadLRSMMesh(const char* path)
{
	isResourceHandlerReady();

	// checks if the mesh is in the cache 
	if (m_meshCache.find(std::string(path)) != m_meshCache.end())
	{
		// returns the buffers
		return m_meshCache[path];
	}

	// or loads the mesh and makes new buffers
	std::string modelPath = m_MODELS_PATH + path;
	std::ifstream fileStream(modelPath, std::ifstream::in | std::ifstream::binary);
	
	size_t i = modelPath.rfind('.', modelPath.length());
	std::string fileExtension = modelPath.substr(i + 1, modelPath.length() - i);
	bool correctFileType = (fileExtension == "lrsm" || fileExtension == "LRSM");
	
		// Check filestream failure
	if (!fileStream || !correctFileType)
	{
		// Error message
		std::string errormsg("loadLRSMMesh failed to open filestream: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		// Properly clear and close file buffer
		fileStream.clear();
		fileStream.close();

		assert(false);

		//return loadLRMMesh(m_ERROR_MODEL_NAME.c_str()); // recursively load the error model instead, this'll be weird because it needs another input layout, but whatever
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
	
	// Read file joint count
	std::uint32_t jointCount;
	fileStream.read((char*)&jointCount, sizeof(std::uint32_t));
	
	// Read file rootJointIdx
	std::uint32_t rootJointIdx;
	fileStream.read((char*)&rootJointIdx, sizeof(std::uint32_t));

	// Read joints to array
	LRSM_JOINT* jointArray = new LRSM_JOINT[jointCount];
	fileStream.read((char*)&jointArray[0], sizeof(LRSM_JOINT) * jointCount);

	std::uint32_t materialCount = 1;
	std::uint32_t* materialOffsets = nullptr;

	char overByte1;
	fileStream.read(&overByte1, 1);
	if (!fileStream.eof())
	{
		fileStream.seekg(-1, std::ios_base::cur);

		// Read file material count
		fileStream.read((char*)&materialCount, sizeof(std::uint32_t));

		if (materialCount > 1)
		{
			// Read materialOffsets to array
			materialOffsets = new std::uint32_t[materialCount];
			fileStream.read((char*)&materialOffsets[0], sizeof(std::uint32_t) * materialCount);
		}

		// Make sure all data was read
		char overByte;
		fileStream.read(&overByte, 1);
		if (!fileStream.eof())
		{
			std::string errormsg("loadLRMMesh : Filestream did not reach end of: "); errormsg.append(path);
			ErrorLogger::get().logError(errormsg.c_str());
			return nullptr;
		}
	}

	// Close filestream
	fileStream.close();

	SkeletalMeshResource* thisSkelRes = new SkeletalMeshResource;

	//Init it with the data
	thisSkelRes->getVertexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, vertexArray, vertexCount, false, sizeof(float) * nrOfFloatsInVertex);
	thisSkelRes->getIndexBuffer().initializeBuffer(m_devicePtr, false, D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER, indexArray, indexCount);
	thisSkelRes->setJointCount(jointCount);
	thisSkelRes->setRootIndex(rootJointIdx);
	thisSkelRes->setJoints(jointArray);
	if (materialCount > 1)
		thisSkelRes->setMaterialOffsetsVector(materialOffsets, materialCount);

	//Create a new entry in the meshcache
	m_meshCache[path] = thisSkelRes;

	LRM_VERTEX* vertexArray2 = (LRSM_VERTEX*)vertexArray;
	XMFLOAT3 min = { 99999, 99999, 99999 }, max = { -99999, -99999, -99999 };
	for (int i = 0; i < vertexCount; i++)
	{
		XMFLOAT3 currentPos = vertexArray2[i].pos;
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
	m_meshCache[path]->storeVertexArray(vertexArray2, vertexCount);

	delete[] vertexArray;
	delete[] indexArray;
	if (materialOffsets != nullptr)
		delete[] materialOffsets;

	//Return the pointer of the new entry
	return m_meshCache[path];
}

AnimationResource* ResourceHandler::loadAnimation(std::string path)
{
	// checks if the animation is in the cache 
	if (m_animationCache.find(path) != m_animationCache.end())
	{
		// returns the resource
		return m_animationCache[path];
	}

	// or loads the mesh and makes new buffers
	std::string animationPath = m_ANIMATION_PATH + path;
	std::ifstream fileStream(animationPath, std::ifstream::in | std::ifstream::binary);

	size_t i = animationPath.rfind('.', animationPath.length());
	std::string fileExtension = animationPath.substr(i + 1, animationPath.length() - i);
	bool correctFileType = (fileExtension == "lra" || fileExtension == "LRA");

	// Check filestream failure
	if (!fileStream || !correctFileType)
	{
		// Error message
		std::string errormsg("loadAnimation failed to open filestream: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		// Properly clear and close file buffer
		fileStream.clear();
		fileStream.close();

		return nullptr;
	}

	float timeSpan;
	fileStream.read((char*)&timeSpan, sizeof(float));

	std::uint32_t frameCount;
	fileStream.read((char*)&frameCount, sizeof(std::uint32_t));

	std::uint32_t jointCount;
	fileStream.read((char*)&jointCount, sizeof(std::uint32_t));
	
	assert(jointCount <= MAX_JOINT_COUNT); // If it asserts here there are more joints than we support. MAX_JOINT_COUNT might need to be changed if the skeleton can't be adjusted.
	
	std::uint32_t dataSize = (sizeof(float) + sizeof(JOINT_TRANSFORM) * jointCount) * frameCount;

	char* animData = new char[dataSize];
	fileStream.read(animData, dataSize);
	
	//ANIMATION_FRAME* animationFrameArray = new ANIMATION_FRAME[frameCount];
	//fileStream.read((char*)&animationFrameArray[0], sizeof(ANIMATION_FRAME) * frameCount);

	// Make sure all data was read
	char overByte;
	fileStream.read(&overByte, 1);
	if (!fileStream.eof())
	{
		std::string errormsg("loadAnimation : Filestream did not reach end of: "); errormsg.append(path);
		ErrorLogger::get().logError(errormsg.c_str());
		return nullptr;
	}
	
	AnimationResource* animation = new AnimationResource();
	
	animation->setTimeSpan(timeSpan);
	animation->setFrameCount(frameCount);
	animation->setJointCount(jointCount);
	
	ANIMATION_FRAME** animationFramesArray = animation->getFrames();
	*animationFramesArray = new ANIMATION_FRAME[frameCount];
	
	int offset = 0;
	for (unsigned int u = 0; u < frameCount; u++)
	{
		//memcpy(&animations.at(i).frames.at(u).timeStamp, animData + offset, sizeof(float));
		memcpy(&(*animationFramesArray)[u].timeStamp, animData + offset, sizeof(float));

		offset += sizeof(float);

		(*animationFramesArray)[u].jointTransforms = new JOINT_TRANSFORM[jointCount];

		for (unsigned int b = 0; b < jointCount; b++)
		{
			//memcpy(&animations.at(i).frames.at(u).jointTransforms[b], animData + offset, sizeof(JointTransformValues));
			memcpy(&(*animationFramesArray)[u].jointTransforms[b], animData + offset, sizeof(JOINT_TRANSFORM));
			
			offset += sizeof(JOINT_TRANSFORM);
		}
	}

	m_animationCache[path] = animation;

	// Close filestream
	fileStream.close();

	delete[] animData;

	return animation;
}

AudioResource* ResourceHandler::loadSound(std::wstring soundPath, AudioEngine* audioEngine)
{
	if (!m_soundCache.count(soundPath))
	{
		std::wstring path = m_SOUNDS_PATH + soundPath;
		try
		{
			SoundEffect* audio = new SoundEffect(audioEngine, path.c_str());
			m_soundCache[soundPath] = new AudioResource;
			m_soundCache[soundPath]->audio = audio;
			m_soundCache[soundPath]->debugName = std::string(soundPath.begin(), soundPath.end());
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
					SoundEffect* audio = new SoundEffect(audioEngine, path.c_str());
					m_soundCache[m_ERROR_SOUND_NAME] = new AudioResource;
					m_soundCache[m_ERROR_SOUND_NAME]->audio = audio;
					m_soundCache[m_ERROR_SOUND_NAME]->debugName = std::string(m_ERROR_SOUND_NAME.begin(), m_ERROR_SOUND_NAME.end());
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

void ResourceHandler::setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr, ID3D11DeviceContext* deferredDContextPtr)
{
	m_devicePtr = devicePtr;
	m_dContextPtr = dContextPtr;
	m_deferredDContextPtr = deferredDContextPtr;

	DeviceAndContextPtrsAreSet = true;
}

void ResourceHandler::Destroy()
{
	for (std::pair<std::wstring, TextureResource*> element : m_textureCache)
		delete element.second;
		//element.second->Release();
	
	
	/*for (auto it = m_meshCache.cbegin(); it != m_meshCache.cend();)
	{
		m_meshCache.erase(it++);
	}*/

	/*for (std::pair<std::string, MeshResource*> element : m_meshCache)
	{
		delete element.second;
	}*/
	for (std::pair<std::string, AnimationResource*> element : m_animationCache)
	{
		delete element.second;
	}
	/*for (std::pair<std::wstring, AudioResource*> element : m_soundCache)
	{
		delete element.second;
	}*/

	//m_textureCache.clear();
	//m_meshCache.clear();
	m_animationCache.clear();
	//m_soundCache.clear();
}