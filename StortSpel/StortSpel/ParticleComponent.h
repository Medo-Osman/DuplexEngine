#pragma once
#include"3DPCH.h"
#include"Component.h"
#include"Particles\Particle.h"
#include"Engine.h"

class ParticleComponent : public Component
{
private:
	Particle* m_particle;
	bool m_active = false;
	float time = 0;
public:
	ParticleComponent(Transform* transform, Particle* particle, ID3D11Device* device)
		:Component()
	{
		m_type = ComponentType::PARTICLE;
		m_particle = particle;
		particle->initParticle(device, Engine::get().getCameraPtr(), transform);
	}

	void activate()
	{
		if (!m_active)
			m_active = true;
	}

	void setTransform(Transform* m_transform)
	{
		m_particle->changeTransform(m_transform);
	}

	void update(float dt)
	{
		if (m_active)
		{
			time += dt;
			m_particle->update(dt, Engine::get().getGameTime());
		}
	}

	void draw(ID3D11DeviceContext* deviceContextPtr)
	{
		if (m_active)
			m_particle->draw(deviceContextPtr);
	}
	~ParticleComponent()
	{

	}

};

