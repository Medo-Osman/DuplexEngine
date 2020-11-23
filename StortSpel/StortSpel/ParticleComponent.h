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
public:
	ParticleComponent(Transform* transform, Particle* particle)
		:Component()
	{
		m_type = ComponentType::PARTICLE;
		m_particle = particle;
		particle->initParticle(Engine::get().getDevicePtr(), Engine::get().getCameraPtr(), transform);
	}

	void activate()
	{
		if(m_particle)
			if (!m_active)
				m_active = true;
	}

	Particle* getParticlePointer()
	{
		return m_particle;
	}

	void setTransform(Transform* m_transform)
	{
		m_particle->changeTransform(m_transform);
	}

	void deactivate()
	{
		m_active = false;
	}

	void update(float dt)
	{
		if (m_active)
		{
			m_particle->update(dt, Engine::get().getGameTime());
		}
	}

	void draw(ID3D11DeviceContext* deviceContextPtr)
	{
		if (m_active)
		{
			if (m_particle->draw(deviceContextPtr))
			{
				SAFE_DELETE(m_particle);
				m_active = false;
			}
		}
	}
	~ParticleComponent()
	{
		SAFE_DELETE(m_particle);
	}

};

