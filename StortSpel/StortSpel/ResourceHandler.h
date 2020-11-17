#pragma once
#include <unordered_map>
#include <stdio.h>
#include "Layouts.h"
#include "VertexStructs.h"
#include "MeshResource.h"
#include "SkeletalMeshResource.h"
#include "AnimationResource.h"
#include "ConstantBufferTypes.h"

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

	~ResourceHandler();
private:

	ID3D11Device* m_devicePtr = NULL;
	ID3D11DeviceContext* m_dContextPtr = NULL;

	bool DeviceAndContextPtrsAreSet = false; //This bool just insures that no one tries to use the resourcehandler before Renderer::initialize has been called
	void isResourceHandlerReady();

	// Textures
	std::unordered_map<std::wstring, ID3D11ShaderResourceView*> m_textureCache;
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
	std::unordered_map<std::wstring, SoundEffect*> m_soundCache;
	const std::wstring m_SOUNDS_PATH = L"../res/audio/";
	const std::wstring m_ERROR_SOUND_NAME = L"ErrorSound.wav";


public:
	ID3D11ShaderResourceView* loadTexture(const WCHAR* texturePath, bool isCubeMap = false);
	ID3D11ShaderResourceView* loadErrorTexture();
	MeshResource* loadLRMMesh(const char* path);
	MeshResource* loadLRSMMesh(const char* path);
	AnimationResource* loadAnimation(std::string path);
	SoundEffect* loadSound(std::wstring soundPath, AudioEngine* audioEngine);

	void setDeviceAndContextPtrs(ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr);
	void Destroy();
};