#include "3DPCH.h"
#include "AudioHandler.h"
#include "ApplicationLayer.h"

AudioHandler::~AudioHandler()
{
	if (m_audioEngine)
	{
		m_audioEngine->Suspend();
	}
	for (auto& loopingSound : m_loopingSoundInstances)
	{
		loopingSound.reset();
	}

}

void AudioHandler::initialize(HWND& handle)
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = AudioEngine_Default | AudioEngine_Debug;
#endif
	m_audioEngine = std::make_shared<AudioEngine>(eflags);
	m_retryAudio = false;

	DEV_BROADCAST_DEVICEINTERFACE filter = {};
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = KSCATEGORY_AUDIO;
	m_newAudio = RegisterDeviceNotification(handle, &filter,
		DEVICE_NOTIFY_WINDOW_HANDLE);

	ApplicationLayer::getInstance().m_input.Attach(this);
	m_soundInstances.clear();
	m_loopingSoundInstances.clear();
}

int AudioHandler::addSoundInstance(const WCHAR* name, float volume, bool isLooping)
{
	SoundEffect* soundEffect = ResourceHandler::get().loadSound(name, m_audioEngine.get());
	int index;
	if(isLooping)
	{
		index = m_loopingSoundInstances.size();
		m_loopingSoundInstances.push_back(soundEffect->CreateInstance());
		m_loopingSoundInstances[index]->SetVolume(volume);
		m_loopingSoundInstances[index]->Play(true);
	}
	else
	{
		index = m_soundInstances.size();
		m_soundInstances.push_back(soundEffect->CreateInstance());
		m_soundInstances[index]->SetVolume(volume);
	}
	return index;
}

void AudioHandler::playSound(int index)
{
	if(m_soundInstances[index]->GetState() == SoundState::PLAYING)
	{
		m_soundInstances[index]->Stop();
	}
	m_soundInstances[index]->Play();
}

void AudioHandler::setVolume(int index, float volume, bool loop)
{
	if(loop)
	{
		m_loopingSoundInstances[index]->SetVolume(volume);
	}
	else
	{
		m_soundInstances[index]->SetVolume(volume);
	}
}

void AudioHandler::inputUpdate(InputData& inputData)
{
	/*for (std::vector<int>::size_type i = 0; i < inputData.actionData.size(); i++)
	{
		if (inputData.actionData[i] == PLAYSOUND)
		{
			m_explode->Play();
		}
	}*/
}

void AudioHandler::update(float dt)
{
	

	if (m_newAudio)
	{
		UnregisterDeviceNotification(m_newAudio);
	}
	if (m_retryAudio)
	{
		m_retryAudio = false;
		if (m_audioEngine->Reset())
		{
			for(auto& loopingSound : m_loopingSoundInstances)
			{
				loopingSound->Play(true);
			}
		}
	}
	else if (!m_audioEngine->Update())
	{
		if (m_audioEngine->IsCriticalError())
		{
			m_retryAudio = true;
		}
	}
}

void AudioHandler::suspend()
{
	m_audioEngine->Suspend();
}

void AudioHandler::resume()
{
	m_audioEngine->Resume();
}

std::shared_ptr<AudioEngine>* AudioHandler::getAudioEngine()
{
	return &m_audioEngine;
}