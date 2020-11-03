#pragma once
#include "ResourceHandler.h"
#include "Input.h"
class AudioHandler : public InputObserver
{
private:
	AudioHandler() {}
	std::shared_ptr<AudioEngine> m_audioEngine;

	/*std::vector<std::unique_ptr<SoundEffectInstance>> m_soundInstances;
	std::vector<std::unique_ptr<SoundEffectInstance>> m_loopingSoundInstances;*/

	bool m_retryAudio;
	HDEVNOTIFY m_newAudio = nullptr;

	std::unordered_map<int, std::unique_ptr<SoundEffectInstance>>  m_soundInstances;
	std::unordered_map<int, std::unique_ptr<SoundEffectInstance>>  m_loopingSoundInstances;

	
	size_t m_idNum = 0;

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

	int addSoundInstance(const WCHAR* name, float volume, bool isLooping);
	void playSound(int index);
	void setVolume(int index, float volume, bool loop);
	void inputUpdate(InputData& inputData);
	void update(float dt);
	void suspend();
	void resume();
	void deleteSound(int index, bool isLooping);
	std::shared_ptr<AudioEngine>* getAudioEngine();

};