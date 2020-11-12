#pragma once
#include"..\3DPCH.h"
#include"Particle.h"


class ScorePickupParticle : public Particle
{
private:
	const std::wstring streamOutVS = L"Particles/StreamOutVertexShader.hlsl";
	const std::wstring streamOutGS = L"Particles/ScorePickupGeometryShaderStreamOut.hlsl";
	const std::wstring drawVS = L"Particles/DrawStreamVelocity.hlsl";
	const std::wstring drawGS = L"Particles/DrawLineByVelocityGS.hlsl";
	const std::wstring drawPS = L"Particles/DrawParticlePixelShader.hlsl";
	
	void setupCBStreamOutPass(ID3D11DeviceContext* deviceContext)
	{

	}
	void setupCBDrawPass(ID3D11DeviceContext* deviceContext)
	{

	}
	void ownEndOfDraw(ID3D11DeviceContext* deviceContext)
	{

	}

public:
	ScorePickupParticle()
		: Particle(ParticleEffect::SCOREPICKUP, false)
	{
		this->m_maxNrOfParticles = 31;
		this->m_particleSystemLifeTime = 6;
		this->m_textureName = L"PearlParticle.png";
		this->setShaders(streamOutVS, streamOutGS, drawVS, drawGS, drawPS);
	}
};