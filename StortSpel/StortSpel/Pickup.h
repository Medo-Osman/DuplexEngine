#pragma once
#include"Entity.h"
#include"AudioComponent.h"
#include"Timer.h"

enum class PickupType
{
	SPEED,
	SCORE,
	COUNT,
};



class Pickup
{
//STATIC
protected:
	static bool PICKUP_VECTOR_INIT;
	static std::vector<Pickup*> PICKUP_VECTOR;
public:
	static bool hasInitPickupArray()
	{
		return PICKUP_VECTOR_INIT;
	}

	static void initPickupArray(std::vector<Pickup*>& vec)
	{
		if (!PICKUP_VECTOR_INIT)
		{
			PICKUP_VECTOR.resize(vec.size());
			for (size_t i = 0; i < vec.size(); i++)
			{
				PICKUP_VECTOR[i] = vec[i];
			}
			PICKUP_VECTOR_INIT = true;
		}
	}

	static Pickup* getPickupByID(int ID)
	{
		return PICKUP_VECTOR[ID];
	}

//END OF STATIC

protected:
	//Sound
	std::wstring m_onPickupSound = L"OnPickup.wav";
	std::wstring m_whileActiveSound = L"";
	std::wstring m_depletedSound = L"OnDepleted.wav";

	PickupType m_pickupType = PickupType::SPEED;
	std::vector<AudioComponent*> m_audioComponents;
	Entity* m_entityToDoEffectsOn;

	bool m_doneDepleted;

	float m_timer;
	float m_duration;
	float m_removeTime;

	AudioComponent* m_whileAudio;


	AudioComponent* addAudioComponent(const std::wstring& sound, const bool& loop = false, float volume = 1.0f)
	{
		m_audioComponents.emplace_back(new AudioComponent(sound, loop, volume));
		return m_audioComponents.back();
	}
public:
	Pickup(PickupType type)
	{
		m_pickupType = type;
		m_removeTime = 1.f;
		m_doneDepleted = false;
		m_entityToDoEffectsOn = nullptr;
		m_whileAudio = nullptr;
	}
	const PickupType& getPickupType()
	{
		return m_pickupType;
	}
	bool isDepleted()
	{
		return m_timer >= m_duration;
	}

	bool shouldDestroy()
	{
		return m_timer >= m_duration + m_removeTime;
	}

	virtual void update(const float& dt)
	{
		m_timer += dt;
		if (!m_doneDepleted && isDepleted())
		{
				this->onDepleted();
				m_doneDepleted = true;
		}
	}
	virtual void onPickup(Entity* entityToDoEffectsOn, int duration)
	{
		m_timer = 0.f;
		m_duration = duration;
		m_entityToDoEffectsOn = entityToDoEffectsOn;
		if (m_onPickupSound != L"")
		{
			entityToDoEffectsOn->addComponent("OnPickup", addAudioComponent(m_onPickupSound, false, 0.3f));
			m_audioComponents.back()->playSound();
		}

		if (m_whileActiveSound != L"")
		{
			entityToDoEffectsOn->addComponent("WhileUsingPickup", m_whileAudio = addAudioComponent(m_whileActiveSound, true, 0.3f));
			m_audioComponents.back()->playSound();
		}
	}
	virtual void onDepleted()
	{
		if (m_whileActiveSound != L"")
		{
			m_entityToDoEffectsOn->removeComponent(m_audioComponents.at(1));
			m_audioComponents.erase(m_audioComponents.begin() + 1);
			delete m_whileAudio;
		}
		if (m_depletedSound != L"")
		{
			m_entityToDoEffectsOn->addComponent("OnDepeleted", addAudioComponent(m_depletedSound, false, 0.01f));
			m_audioComponents.back()->playSound();
		}
	}

	virtual void onRemove()
	{
		for (size_t i = 0; i < m_audioComponents.size(); i++)
		{
			m_entityToDoEffectsOn->removeComponent(m_audioComponents.at(i));
			delete m_audioComponents.at(i);
		}
		m_audioComponents.clear();
		m_doneDepleted = false;
	}
};