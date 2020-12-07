#pragma once
#include"3DPCH.h"
#include"Pickup.h"
#include"ParticleComponent.h"
#include"Particles\PlayerLineParticle.h"

class HeightPickup : public Pickup
{
private:
	ParticleComponent* m_particleEffect;

public:
	HeightPickup()
		:Pickup(PickupType::HEIGHTBOOST)
	{
		m_onUseSound = L"spring.wav";
		m_onPickupSound = L"";
		//m_whileActiveSound = L"SpeedSound.wav";
	
		m_activateOnPickup = false;
		m_isTimeBased = false;
		m_pickupIcon = L"TrampolinIcon.png";
	}
	~HeightPickup()
	{
		SAFE_DELETE(m_particleEffect);
	}

	virtual void update(const float& dt)
	{
		Pickup::update(dt);
	}
	virtual void onPickup(Entity* entityToDoEffectsOn, bool isEnvironmental)
	{
		Pickup::onPickup(entityToDoEffectsOn, isEnvironmental);
	}

	virtual void onUse()
	{
		Pickup::onUse();
		m_particleEffect = new ParticleComponent(m_entityToDoEffectsOn, new PlayerLineParticle());
		m_entityToDoEffectsOn->addComponent("heightParticle", m_particleEffect);
		m_particleEffect->activate();
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
};