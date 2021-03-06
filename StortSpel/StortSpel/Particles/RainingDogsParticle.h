#pragma once
#include"3DPCH.h"
#include"Particles\Particle.h"


class RainingDogParticle : public Particle
{
private:
	const std::wstring streamOutVS = L"Particles/StreamOutVertexShader.hlsl";
	const std::wstring streamOutGS = L"Particles/RainGeometryShaderStreamOut.hlsl";
	const std::wstring drawVS = L"Particles/DrawStreamVertexShader.hlsl";
	const std::wstring drawGS = L"Particles/RainGeometryShaderDraw.hlsl";
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
	RainingDogParticle()
		: Particle(ParticleEffect::RAININGDOG, true)
	{
		this->m_maxNrOfParticles = 360;
		this->m_textureName = L"T_tempTestDog.jpeg";
		this->setShaders(streamOutVS, streamOutGS, drawVS, drawGS, drawPS);
	}
};