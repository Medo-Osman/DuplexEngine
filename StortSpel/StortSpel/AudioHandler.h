#pragma once
#include "ResourceHandler.h"
#include "Input.h"
#include "Transform.h"


class AudioHandler
{
private:
	AudioHandler();
	std::shared_ptr<AudioEngine> m_audioEngine;

	bool m_retryAudio;
	HDEVNOTIFY m_newAudio = nullptr;

	// Sound Instances
	std::unordered_map<int, AudioResource*> m_referencedSources;
	std::unordered_map<int, std::unique_ptr<SoundEffectInstance>>  m_soundInstances;
	std::unordered_map<int, std::unique_ptr<SoundEffectInstance>>  m_loopingSoundInstances;
	size_t m_idNum = 0;
	
	// 3D Positional Audio
	AudioEmitter m_emitter;

	AudioListener m_listener;
	Transform* m_listenerTransformPtr;

	bool m_isReleased = false;

public:
	AudioHandler(const AudioHandler&) = delete;
	void operator=(AudioHandler const&) = delete;
	static AudioHandler& get()
	{
		static AudioHandler instance;
		return instance;
	}
	~AudioHandler();

	void release();

	void initialize(HWND &handle);
	void setListenerTransformPtr(Transform* listenerTransform);
	void onNewAudioDevice() { m_retryAudio = true; }

	void addReference(int index);
	void removeReference(int index);

	int addSoundInstance(const WCHAR* name, bool isLooping, float volume, float pitch, bool isPositional, Vector3 position);
	void playSound(int index);
	void emitSound(int index, Vector3 position);
	void setVolume(int index, float volume, bool isLooping);
	void setPitch(int index, float pitch, bool isLooping);
	void setEmitterPosition(int index, Vector3 position, bool isLooping);
	void update(float dt);
	void suspend();
	void resume();
	void deleteSound(int index, bool isLooping);
	std::shared_ptr<AudioEngine>* getAudioEngine();
};