#pragma once
#include "3DPCH.h"
#include "AudioHandler.h"
#include "Component.h"

class AudioComponent : public Component
{
private:
	int m_audioIndex;
	bool m_isLooping;
	bool m_isPositional;
	Transform* m_emitterTransform;
public:
	AudioComponent(std::wstring soundName, bool loop = false, float volume = 1.0f, bool isPositional = false, Transform* emitterTransform = nullptr)
	{
		m_type = ComponentType::AUDIO;
		m_isLooping = loop;
		m_isPositional = isPositional;

		// Emitter Position for positional audio
		Vector3 emitterPosition;
		m_emitterTransform = emitterTransform;
		if (m_emitterTransform)
			emitterPosition = m_emitterTransform->getTranslation();
		else
			emitterPosition = Vector3(0.f);
		
		m_audioIndex = AudioHandler::get().addSoundInstance(soundName.c_str(), volume, loop, isPositional, emitterPosition);
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
			if (m_isPositional)
				AudioHandler::get().emitSound(m_audioIndex, m_emitterTransform->getTranslation());
			else
				AudioHandler::get().playSound(m_audioIndex);
		}
	}
	// Update
	virtual void update(float dt) override
	{

	}
};
