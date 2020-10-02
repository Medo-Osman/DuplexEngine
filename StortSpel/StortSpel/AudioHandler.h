#pragma once
#include "ResourceHandler.h"
class AudioHandler
{
private:
	AudioHandler() {}

	std::shared_ptr<AudioEngine> m_audioEngine;
	bool m_retryAudio;
	HDEVNOTIFY m_newAudio = nullptr;

	SoundEffect* m_ambient;
	std::unique_ptr<DirectX::SoundEffectInstance> m_nightLoop;
	float nightVolume;
	float nightSlide;


public:
	AudioHandler(const AudioHandler&) = delete;
	void operator=(AudioHandler const&) = delete;
	static AudioHandler& get()
	{
		static AudioHandler instance;
		return instance;
	}
	~AudioHandler();

	void initialize(HWND &handle);
	void onNewAudioDevice() { m_retryAudio = true; }

	void update(float dt);
	void suspend();
	void resume();
	std::shared_ptr<AudioEngine>* getAudioEngine();

};