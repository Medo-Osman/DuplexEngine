#include "3DPCH.h"
#include "AudioHandler.h"
#include "ApplicationLayer.h"

AudioHandler::~AudioHandler()
{
	if (m_audioEngine)
	{
		m_audioEngine->Suspend();
	}
	m_nightLoop.reset();
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

	m_explode = ResourceHandler::get().loadSound(L"Explo1.wav", m_audioEngine.get());
	m_ambient = ResourceHandler::get().loadSound(L"NightAmbienceSimple_02.wav", m_audioEngine.get());
	m_nightLoop = m_ambient->CreateInstance();
	m_nightLoop->Play(true);

	nightVolume = 0.5f;
	nightSlide = -0.1f;
	m_nightLoop->SetVolume(nightVolume);
}

void AudioHandler::inputUpdate(InputData& inputData)
{
	for (std::vector<int>::size_type i = 0; i < inputData.actionData.size(); i++)
	{
		if (inputData.actionData[i] == PLAYSOUND)
		{
			m_explode->Play();
		}
	}
}

void AudioHandler::update(float dt)
{
	nightVolume += dt * nightSlide;
	if (nightVolume < 0.f)
	{
		nightVolume = 0.f;
		nightSlide = -nightSlide;
	}
	else if (nightVolume > 0.5f)
	{
		nightVolume = 0.5f;
		nightSlide = -nightSlide;
	}
	m_nightLoop->SetVolume(nightVolume);



	if (m_newAudio)
	{
		UnregisterDeviceNotification(m_newAudio);
	}
	if (m_retryAudio)
	{
		m_retryAudio = false;
		if (m_audioEngine->Reset())
		{
			if (m_nightLoop)
				m_nightLoop->Play(true);
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

