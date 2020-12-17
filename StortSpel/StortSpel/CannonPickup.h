#pragma once
#include"3DPCH.h"
#include"Pickup.h"
#include"ParticleComponent.h"
#include"Particles\PlayerLineParticle.h"
#include"Particles\LineArrayParticle.h"

class CannonPickup : public Pickup
{
private:
	ParticleComponent* m_particleEffect;
	std::wstring m_shootSound = L"Explo1.wav";
	std::wstring m_loadSound = L"CannonLoad.wav";
	AudioComponent* m_shootAudioComponent;
public:
	CannonPickup()
		:Pickup(PickupType::CANNON)
	{
		//m_onPickupSound = L"";
		//m_whileActiveSound = L"SpeedSound.wav";
		m_activateOnPickup = false;
		m_isTimeBased = false;
		m_shootAudioComponent = nullptr;
		m_particleEffect = nullptr;
		m_pickupIcon = L"CanonIcon.png";
	}
	virtual void update(const float& dt)
	{
		Pickup::update(dt);
		if (m_particleEffect)
		{
			if (!Engine::get().getPlayerPtr()->m_shouldDrawLine)
			{
				if(m_particleEffect->isActive())
					m_particleEffect->deactivate();
			}
			else
			{
				if (!m_particleEffect->isActive())
				{
					m_particleEffect->activate();
				}
			}
		}
	}
	virtual void onPickup(Entity* entityToDoEffectsOn, bool isEnvironmental)
	{
		Pickup::onPickup(entityToDoEffectsOn, isEnvironmental);
		addAudioComponent(m_loadSound, false);
	}

	virtual void onUse()
	{

		Engine* eng = &Engine::get();
		Pickup::onUse();
		m_particleEffect = new ParticleComponent(m_entityToDoEffectsOn, new LineArrayParticle(L"red.png")); // T_Missing_D.dds
		m_entityToDoEffectsOn->addComponent("trajectory", m_particleEffect);
		m_particleEffect->getParticlePointer()->initParticle(eng->getDevicePtr(), eng->getCameraPtr(), m_entityToDoEffectsOn);
		m_particleEffect->activate();

		m_audioComponents.back()->playSound();
		m_shootAudioComponent = addAudioComponent(m_shootSound, false);
	}
	virtual void onDepleted()
	{
		Pickup::onDepleted();
		m_particleEffect->deactivate();
	}

	virtual void onRemove()
	{
		Pickup::onRemove();
		m_entityToDoEffectsOn->removeComponent(m_particleEffect);
		delete m_particleEffect;
		m_particleEffect = nullptr;
	}

	void geFyr()
	{
		m_shootAudioComponent->playSound();
	}
};