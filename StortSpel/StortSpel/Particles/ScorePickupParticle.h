#pragma once
#include"..\3DPCH.h"
#include"Particle.h"


class ScorePickupParticle : public Particle
{
private:
	const std::wstring streamOutVS = L"Particles/StreamOutVertexShader.hlsl";
	const std::wstring streamOutGS = L"Particles/ScorePickupGeometryShaderStreamOut.hlsl";
	const std::wstring drawVS = L"Particles/DrawStreamVertexShader.hlsl";
	const std::wstring drawGS = L"Particles/RainGeometryShaderDraw.hlsl";
	const std::wstring drawPS = L"Particles/DrawParticlePixelShader.hlsl";
	
	void setupCBStreamOutPass()
	{

	}
	void setupCBDrawPass()
	{

	}
	void ownEndOfDraw()
	{

	}

public:
	ScorePickupParticle()
		: Particle(ParticleEffect::SCOREPICKUP, true)
	{
		this->m_maxNrOfParticles = 1000;
		this->m_textureName = L"T_tempTestDog.jpeg";
		this->setShaders(streamOutVS, streamOutGS, drawVS, drawGS, drawPS);
	}
};