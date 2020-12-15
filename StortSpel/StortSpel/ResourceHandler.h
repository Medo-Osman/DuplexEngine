#pragma once
#include <unordered_map>
#include <stdio.h>
#include "Layouts.h"
#include "VertexStructs.h"
#include "MeshResource.h"
#include "TextureResource.h"
#include "SkeletalMeshResource.h"
#include "AnimationResource.h"
#include "AudioResource.h"
#include "ConstantBufferTypes.h"
#include <mutex>

class ResourceHandler
{
private:
	ResourceHandler() {}
	std::mutex m_lock;

public:
	bool m_unloaded = false;
	ResourceHandler(const ResourceHandler&) = delete;
	void operator=(ResourceHandler const&) = delete;
	static ResourceHandler& get()
	{
		static ResourceHandler instance;
		return instance;
	}

	~ResourceHandler();
private:

	ID3D11Device* m_devicePtr = NULL;
	ID3D11DeviceContext* m_dContextPtr = NULL;
	ID3D11DeviceContext* m_deferredDContextPtr = NULL;

	bool DeviceAndContextPtrsAreSet = false; //This bool just ensures that no one tries to use the resourcehandler before Renderer::initialize has been called
	void isResourceHandlerReady();

	// Textures
	std::unordered_map<std::wstring, TextureResource*> m_textureCache;
	const std::wstring m_TEXTURES_PATH = L"../res/textures/";
	const std::wstring m_ERROR_TEXTURE_NAME = L"error_dun_gofed.jpg";
	// Models
	std::unordered_map < std::string, MeshResource* > m_meshCache;
	const std::string m_MODELS_PATH = "../res/models/";
	const std::string m_ERROR_MODEL_NAME = "error.lrm";
	// Animation
	std::unordered_map<std::string, AnimationResource*> m_animationCache;
	const std::string m_ANIMATION_PATH = "../res/animations/";
	// Audio
	std::unordered_map<std::wstring, AudioResource*> m_soundCache;
	const std::wstring m_SOUNDS_PATH = L"../res/audio/";
	const std::wstring m_ERROR_SOUND_NAME = L"ErrorSound.wav";

public:
	ID3D11CommandList* m_commandList = nullptr;

	void checkResources();
	TextureResource* loadTexture(std::wstring texturePath, bool isCubeMap = false, bool isTexture3D = false, bool referenceBasedDelete = false);
	TextureResource* loadErrorTexture();
	MeshResource* loadLRMMesh(const char* path);
	MeshResource* loadLRSMMesh(const char* path);
	AnimationResource* loadAnimation(std::string path);
	AudioResource* loadSound(std::wstring soundPath, AudioEngine* audioEngine);

	void setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr, ID3D11DeviceContext* deferredDContextPtr);
	void Destroy();
};