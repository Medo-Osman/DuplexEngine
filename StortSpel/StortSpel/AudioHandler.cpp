#include "3DPCH.h"
#include "AudioHandler.h"
#include "ApplicationLayer.h"

AudioHandler::AudioHandler()
{
	m_retryAudio = false;
	m_listenerTransformPtr = nullptr;
}

AudioHandler::~AudioHandler()
{
	
}

void AudioHandler::release()
{
	if (m_audioEngine)
	{
		m_audioEngine->Suspend();
		m_audioEngine.reset();
	}
	for (auto& loopingSound : m_loopingSoundInstances)
	{
		delete loopingSound.second.release();
	}
	m_loopingSoundInstances.clear();

	for (auto& sound : m_soundInstances)
	{
		delete sound.second.release();
	}
	m_soundInstances.clear();

	//m_referencedSources.clear();
	if (DEBUGMODE)
		std::cout << "Clean Audio Handler!" << std::endl;
	m_isReleased = true;
}

bool AudioHandler::getAudioChanged()
{
	return m_hasAudioChanged;
}

void AudioHandler::initialize(HWND& handle)
{
	// Audio Engine Setup
	// - Flags
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_EnvironmentalReverb | AudioEngine_ReverbUseFilters;
	#ifdef _DEBUG
		eflags = eflags | AudioEngine_Debug;
	#endif
	// - Engine
	m_audioEngine = std::make_shared<AudioEngine>(eflags);
	m_audioEngine->SetReverb(Reverb_Plain);
	if (!m_audioEngine->IsAudioDevicePresent())
		ErrorLogger::get().logError("AudioError: No audio device found");
	m_audioEngine->Reset();

	// Audio Listener(for 3d positional audio)
	m_listener.OrientFront = Vector3(0.f, 0.f, 1.f);
	m_listener.SetPosition(Vector3(0.f, 1.f, 0.f));
	m_listener.SetOrientation(Vector3(0.f, 0.f, 1.f), Vector3(0.f, 1.f, 0.f));

	// Audio Emittors
	m_emitter.SetPosition(XMFLOAT3(0.f, 0.f, 0.f));

	// Device Setup
	DEV_BROADCAST_DEVICEINTERFACE filter = {};
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = KSCATEGORY_AUDIO;
	m_newAudio = RegisterDeviceNotification(handle, &filter,
		DEVICE_NOTIFY_WINDOW_HANDLE);

	// Initialize Maps
	m_soundInstances.clear();
	m_loopingSoundInstances.clear();
}

void AudioHandler::setListenerTransformPtr(Transform* listenerTransform)
{
	m_listenerTransformPtr = listenerTransform;
	m_listener.SetPosition(m_listenerTransformPtr->getTranslation());
	m_listener.SetOrientation(m_listenerTransformPtr->getForwardVector(), Vector3(0.f, 1.f, 0.f));
}

void AudioHandler::addReference(int index)
{
	m_referencedSources[index]->addRef();
}

void AudioHandler::removeReference(int index)
{
	if (!m_isReleased) //Because release is called before all the components are cleaned up.
	{
		AudioResource* res = m_referencedSources[index];
		m_referencedSources[index]->deleteRef();
		m_referencedSources.erase(index);
	}
}

int AudioHandler::addSoundInstance(const WCHAR* name, bool isLooping, float volume, float pitch, bool isPositional, Vector3 emitterPosition)
{
	AudioResource* resource = ResourceHandler::get().loadSound(name, m_audioEngine.get());
	SoundEffect* soundEffect = resource->audio;
	SOUND_EFFECT_INSTANCE_FLAGS soundflags = SoundEffectInstance_Default;
		int index = (int)m_idNum++;


	if (isPositional)
	{
		soundflags = SoundEffectInstance_Use3D | SoundEffectInstance_ReverbUseFilters;
		m_emitter.SetPosition(emitterPosition);
	}

	if(isLooping)
	{
		m_loopingSoundInstances[index] = soundEffect->CreateInstance(soundflags);
		m_loopingSoundInstances[index]->SetVolume(volume);
		m_loopingSoundInstances[index]->SetPitch(pitch);
		m_loopingSoundInstances[index]->Play(true);
		if (isPositional)
			m_loopingSoundInstances[index]->Apply3D(m_listener, m_emitter, false);
	}
	else
	{
		m_soundInstances[index] = soundEffect->CreateInstance(soundflags);
		m_soundInstances[index]->SetVolume(volume);
		m_soundInstances[index]->SetPitch(pitch);
		if (isPositional)
			m_soundInstances[index]->Apply3D(m_listener, m_emitter, false);
	}

	m_referencedSources[index] = resource;
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

void AudioHandler::emitSound(int index, Vector3 position)
{
	if (m_soundInstances[index]->GetState() == SoundState::PLAYING)
	{
		m_soundInstances[index]->Stop();
	}

	m_emitter.SetPosition(position);
	m_soundInstances[index]->Play();
	m_soundInstances[index]->Apply3D(m_listener, m_emitter, false);
}

void AudioHandler::setVolume(int index, float volume, bool isLooping)
{
	if(isLooping)
	{
		m_loopingSoundInstances[index]->SetVolume(volume);
	}
	else
	{
		m_soundInstances[index]->SetVolume(volume);
	}
}

void AudioHandler::setPitch(int index, float pitch, bool isLooping)
{
	if (isLooping)
	{
		m_loopingSoundInstances[index]->SetPitch(pitch);
	}
	else
	{
		m_soundInstances[index]->SetPitch(pitch);
	}
}

void AudioHandler::setEmitterPosition(int index, Vector3 position, bool isLooping)
{
	m_emitter.SetPosition(position);
	if (isLooping)
		m_loopingSoundInstances[index]->Apply3D(m_listener, m_emitter, false);
}

int AudioHandler::increaseVolume()
{	
	if (m_volumeAmount < MAX_VOLUME)
	{
		m_hasAudioChanged = true;
		m_volumeAmount += 1;
		return m_volumeAmount;
	}
}

int AudioHandler::decreaseVolume()
{
	if (m_volumeAmount > 0)
	{
		m_hasAudioChanged = true;
		m_volumeAmount -= 1;
		return m_volumeAmount;
	}
}

int AudioHandler::getVolumeAmount()
{
	return m_volumeAmount;
}

void AudioHandler::update(float dt)
{
	
	if (m_listenerTransformPtr)
	{
		m_listener.SetPosition(m_listenerTransformPtr->getTranslation());
		m_listener.SetOrientation(m_listenerTransformPtr->getForwardVector(), Vector3(0.f, 1.f, 0.f));
	}
	
	if (m_newAudio)
	{
		UnregisterDeviceNotification(m_newAudio);
	}
	if (m_retryAudio)
	{
		ErrorLogger::get().logError("Retrying Audio!");
		if (m_audioEngine->Reset())
		{
			for(auto& loopingSound : m_loopingSoundInstances)
				loopingSound.second->Play(true);
		}
		m_retryAudio = false;
	}
	
	else if (!m_audioEngine->Update())
	{
		if (m_audioEngine->IsCriticalError())
		{
			ErrorLogger::get().logError("AudioError: Audio device was lost.");
			m_retryAudio = true;
		}
	}
	for (auto& soundsInstance : m_soundInstances)
	{
		soundsInstance.second->SetVolume(m_volumeAmount / 10.f);
	}
	for (auto& loopingSounds : m_loopingSoundInstances)
	{
		loopingSounds.second->SetVolume(m_volumeAmount / 10.f);
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

void AudioHandler::deleteSound(int index, bool isLooping)
{
	if (isLooping)
	{
		if (m_loopingSoundInstances.empty())
			return;

		if (m_loopingSoundInstances.find(index) == m_loopingSoundInstances.end())
		{
			std::string errormsg("sound not in map! Index: "); errormsg.append(std::to_string(index));
			ErrorLogger::get().logError(errormsg.c_str());
			return;
		}
		
		m_loopingSoundInstances[index]->Stop();
		delete m_loopingSoundInstances[index].release();
		m_loopingSoundInstances.erase(index);
	}
	else
	{
		if (m_soundInstances.empty())
			return;
		
		if (m_soundInstances.find(index) == m_soundInstances.end())
		{
			std::string errormsg("sound not in map! Index: "); errormsg.append(std::to_string(index));
			ErrorLogger::get().logError(errormsg.c_str());
			return;
		}
		delete m_soundInstances[index].release();
		m_soundInstances.erase(index);
	}
}
std::shared_ptr<AudioEngine>* AudioHandler::getAudioEngine()
{
	return &m_audioEngine;
}