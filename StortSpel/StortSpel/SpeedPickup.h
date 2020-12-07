#pragma once
#include"3DPCH.h"
#include"Pickup.h"
#include"ParticleComponent.h"
#include"Particles\PlayerLineParticle.h"

class SpeedPickup : public Pickup
{
private:
	ParticleComponent* m_lineParticleComponentPtr;
	const float SPEEDDURATION = 4.f;
public:
	SpeedPickup()
		:Pickup(PickupType::SPEED)
	{
		//m_onPickupSound = L"";
		m_whileActiveSound = L"SpeedSound.wav";
		m_activateOnPickup = false;
		m_duration = SPEEDDURATION;
		m_isTimeBased = true;
		m_pickupIcon = L"SpeedIcon.png";
	}
	~SpeedPickup()
	{
		SAFE_DELETE(m_lineParticleComponentPtr);
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
		m_lineParticleComponentPtr = new ParticleComponent(m_entityToDoEffectsOn, new PlayerLineParticle());
		m_entityToDoEffectsOn->addComponent("speedParticle", m_lineParticleComponentPtr);
		m_lineParticleComponentPtr->activate();
	}
	virtual void onDepleted()
	{
		Pickup::onDepleted();
		m_lineParticleComponentPtr->deactivate();
	}

	virtual void onRemove()
	{
		Pickup::onRemove();
		m_entityToDoEffectsOn->removeComponent(m_lineParticleComponentPtr);
		delete m_lineParticleComponentPtr;
		m_lineParticleComponentPtr = nullptr;
	}
};