#pragma once
#include"3DPCH.h"
#include"Pickup.h"

class SpeedPickup : public Pickup
{
private:

public:
	SpeedPickup()
		:Pickup(PickupType::SPEED)
	{
		m_whileActiveSound = L"wind.wav";
	}
	virtual void update(const float& dt)
	{
		Pickup::update(dt);
	}
	virtual void onPickup(Entity* entityToDoEffectsOn, int duration)
	{
		Pickup::onPickup(entityToDoEffectsOn, duration);
	}
	virtual void onDepleted()
	{
		Pickup::onDepleted();
	}

	virtual void onRemove()
	{
		Pickup::onRemove();
	}
};