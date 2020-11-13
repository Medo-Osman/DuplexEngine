#pragma once
#include"3DPCH.h"
#include"Pickup.h"
#include"ParticleComponent.h"
#include"Particles\PlayerLineParticle.h"

class SpeedPickup : public Pickup
{
private:
	ParticleComponent* m_lineParticleComponentPtr;

public:
	SpeedPickup()
		:Pickup(PickupType::SPEED)
	{
		//m_onPickupSound = L"";
		m_whileActiveSound = L"SpeedSound.wav";
	}
	virtual void update(const float& dt)
	{
		Pickup::update(dt);
	}
	virtual void onPickup(Entity* entityToDoEffectsOn, int duration)
	{
		Pickup::onPickup(entityToDoEffectsOn, duration);
		m_lineParticleComponentPtr = new ParticleComponent(entityToDoEffectsOn, new PlayerLineParticle());
		entityToDoEffectsOn->addComponent("particle", m_lineParticleComponentPtr);
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