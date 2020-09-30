#pragma once
#include "ResourceHandler.h"
class Audio
{
private:
	std::shared_ptr<AudioEngine> m_audioEngine;
	bool m_retryAudio;
	HDEVNOTIFY m_newAudio = nullptr;

	SoundEffect* m_ambient;

	Audio() {}

public:
	Audio(const Audio&) = delete;
	void operator=(Audio const&) = delete;
	static Audio& get()
	{
		static Audio instance;
		return instance;
	}
	~Audio();

	void initialize(HWND &handle);
	void onNewAudioDevice() { m_retryAudio = true; }

	void update();
	void suspend();
	void resume();

};