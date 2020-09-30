#include "3DPCH.h"
#include "Audio.h"

Audio::~Audio()
{
	if (m_audioEngine)
	{
		m_audioEngine->Suspend();
	}
}

void Audio::initialize(HWND& handle)
{
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags |= AudioEngine_Debug;
#endif
	m_audioEngine = std::make_unique<AudioEngine>(eflags);
	m_retryAudio = false;

	DEV_BROADCAST_DEVICEINTERFACE filter = {};
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = KSCATEGORY_AUDIO;
	m_newAudio = RegisterDeviceNotification(handle, &filter,
		DEVICE_NOTIFY_WINDOW_HANDLE);

	m_ambient = ResourceHandler::get().loadSound(L"NightAmbienceSimple_02.wav", m_audioEngine);
}

void Audio::update()
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

void Audio::suspend()
{
	m_audioEngine->Suspend();
}

void Audio::resume()
{
	m_audioEngine->Resume();
}
