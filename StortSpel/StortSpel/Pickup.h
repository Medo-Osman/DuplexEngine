#pragma once
#include"Entity.h"
#include"AudioComponent.h"
#include"Timer.h"

enum class PickupType //Add new pickuptypes -before- score and count. Also add the coresponding pickup class in Player.cpp. Look for function call initPickupArray(vec).
{
	SPEED,
	HEIGHTBOOST,
	CANNON,
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
	static const bool hasInitPickupArray()
	{
		return PICKUP_VECTOR_INIT;
	}

	static void clearStaticPickupArrayPlz()
	{
		//k
		for (size_t i = 0; i < PICKUP_VECTOR.size(); i++)
		{
			SAFE_DELETE(PICKUP_VECTOR[i]);
		}
		PICKUP_VECTOR.clear();
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
	std::wstring m_onUseSound = L"";
	std::wstring m_whileActiveSound = L"";
	std::wstring m_depletedSound = L"OnDepleted.wav";

	PickupType m_pickupType = PickupType::SPEED;
	std::vector<AudioComponent*> m_audioComponents;
	Entity* m_entityToDoEffectsOn;
	float m_modifierValue;

	bool m_doneDepleted;
	bool m_activateOnPickup;
	bool m_active;
	bool m_isTimeBased;

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
		m_active = false;
		m_modifierValue = 0;
		m_isTimeBased = true;
	}

	const bool& isActive() const
	{
		return m_active;
	}

	const bool& shouldActivateOnPickup() const
	{
		return m_activateOnPickup;
	}

	const float& getModifierValue() const
	{
		return m_modifierValue;
	}

	void setModifierValue(float modifierValue)
	{
		m_modifierValue = modifierValue;
	}


	const PickupType& getPickupType() const
	{
		return m_pickupType;
	}
	const bool isDepleted() const
	{
		if (m_isTimeBased)
			return m_timer >= m_duration;
		else
			return false;
	}

	const bool shouldDestroy() const
	{
		bool destroy = false;
		if (m_isTimeBased)
			destroy = m_timer >= m_duration + m_removeTime;
		
		return destroy;
	}

	virtual void update(const float& dt)
	{
		if (m_active)
		{
			if (m_isTimeBased)
			{
				m_timer += dt;
				if (!m_doneDepleted && isDepleted())
				{
					this->onDepleted();
					m_doneDepleted = true;
				}
			}

		}
	}

	virtual void onUse()
	{
		this->m_active = true;
		if (m_onUseSound != L"")
		{
			m_entityToDoEffectsOn->addComponent("OnUse", addAudioComponent(m_onUseSound, false, 0.3f));
			m_audioComponents.back()->playSound();
		}

		if (m_whileActiveSound != L"")
		{
			m_entityToDoEffectsOn->addComponent("WhileUsingPickup", m_whileAudio = addAudioComponent(m_whileActiveSound, true, 0.3f));
			m_audioComponents.back()->playSound();
		}
	}
	virtual void onPickup(Entity* entityToDoEffectsOn)
	{
		m_timer = 0.f;
		m_entityToDoEffectsOn = entityToDoEffectsOn;
		if (m_onPickupSound != L"")
		{
			entityToDoEffectsOn->addComponent("OnPickup", addAudioComponent(m_onPickupSound, false, 0.3f));
			m_audioComponents.back()->playSound();
		}
	}
	virtual void onDepleted()
	{
		if (m_whileActiveSound != L"")
		{
			bool removed = false;
			m_entityToDoEffectsOn->removeComponent(m_whileAudio);
			for (size_t i = 0; i < m_audioComponents.size() && !removed; i++)
			{
				if (m_audioComponents[i] == m_whileAudio)
				{
					m_audioComponents.erase(m_audioComponents.begin() + i);
					removed = true;
				}
			}
			delete m_whileAudio;
		}
		if (m_depletedSound != L"")
		{
			m_entityToDoEffectsOn->addComponent("OnDepeleted", addAudioComponent(m_depletedSound, false, 0.3f));
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