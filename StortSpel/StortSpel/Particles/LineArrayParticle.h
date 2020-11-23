#pragma once
#include"..\3DPCH.h"
#include"..\Player.h"
#include"Particle.h"
#include"..\Engine.h"
#include"..\ConstantBufferTypes.h"


class LineArrayParticle : public Particle
{
private:
	const std::wstring streamOutVS = L"Particles/StreamOutVertexShader.hlsl";
	const std::wstring streamOutGS = L"Particles/ArrayOfLinesStreamOut.hlsl";
	const std::wstring drawVS = L"Particles/PlayerLineDrawVertexShader.hlsl";
	const std::wstring drawGS = L"Particles/PlayerLineDrawGS.hlsl";
	const std::wstring drawPS = L"Particles/DrawParticlePixelShader.hlsl";

	Buffer<LineData> m_lineDataBuffer;
	LineData m_lineData[10];

	Player* m_player;

	void setupCBStreamOutPass(ID3D11DeviceContext* deviceContext)
	{
		m_lineDataBuffer.updateBuffer(deviceContext, m_player->getLineDataArray(), 10);
		deviceContext->GSSetConstantBuffers(1, 1, m_lineDataBuffer.GetAddressOf());
	}

	void setupCBDrawPass(ID3D11DeviceContext* deviceContext)
	{

	}

	void ownEndOfDraw(ID3D11DeviceContext* deviceContext)
	{

	}

public:
	LineArrayParticle()
		: Particle(ParticleEffect::LINEARRAY, true)
	{
		this->m_maxNrOfParticles = 100;
		this->m_textureName = L"white.png";
		this->setShaders(streamOutVS, streamOutGS, drawVS, drawGS, drawPS);
		m_player = Engine::get().getPlayerPtr();
		m_lineDataBuffer.initializeBuffer(Engine::get().getDevicePtr(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, m_player->getLineDataArray(), 10);

	}
};