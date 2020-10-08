#pragma once
#include "3DPCH.h"
#include "AudioHandler.h"
#include "Component.h"

class AudioComponent : public Component
{
private:
	int m_audioIndex;
	bool m_isLooping;
public:
	AudioComponent(std::wstring soundName, bool loop = false, float volume = 1.0f)
	{
		m_type = ComponentType::AUDIO;
		m_isLooping = loop;
		m_audioIndex = AudioHandler::get().addSoundInstance(soundName.c_str(), volume, loop);
	}
	virtual ~AudioComponent() override 
	{
		AudioHandler::get().deleteSound(m_audioIndex, m_isLooping);
	}

	void setVolume(float volume)
	{
		AudioHandler::get().setVolume(m_audioIndex, volume, m_isLooping);
	}

	void playSound()
	{
		if(!m_isLooping)
		{
			AudioHandler::get().playSound(m_audioIndex);
		}
	}
	// Update
	virtual void update(float dt) override
	{

	}
};
