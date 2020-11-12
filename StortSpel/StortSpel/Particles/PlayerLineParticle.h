#pragma once
#include"..\3DPCH.h"
#include"Particle.h"

struct float3Data
{
	XMFLOAT3 float3;
	UINT uint;
};

class PlayerLineParticle : public Particle
{
private:
	const std::wstring streamOutVS = L"Particles/StreamOutVertexShader.hlsl";
	const std::wstring streamOutGS = L"Particles/PlayerLineStreamOutGS.hlsl";
	const std::wstring drawVS = L"Particles/PlayerLineDrawVertexShader.hlsl";
	const std::wstring drawGS = L"Particles/PlayerLineDrawGS.hlsl";
	const std::wstring drawPS = L"Particles/DrawParticlePixelShader.hlsl";

	Buffer<float3Data> m_playerVelocityBuffer;
	float3Data m_playerVelocityData;
	
	Buffer<float3Data> m_rightOffsetBuffer;
	float3Data m_rightOffsetData;

	Player* m_player;

	void setupCBStreamOutPass(ID3D11DeviceContext* deviceContext)
	{
		m_playerVelocityData.float3 = m_player->getVelocity();
		m_playerVelocityBuffer.updateBuffer(deviceContext, &m_playerVelocityData);
		deviceContext->GSSetConstantBuffers(1, 1, m_playerVelocityBuffer.GetAddressOf());

		m_rightOffsetData.float3 = m_player->getPlayerEntity()->getRightVector();
		m_rightOffsetData.uint = m_player->isRunning();
		m_rightOffsetBuffer.updateBuffer(deviceContext, &m_rightOffsetData);
		deviceContext->GSSetConstantBuffers(2, 1, m_rightOffsetBuffer.GetAddressOf());
	}

	void setupCBDrawPass(ID3D11DeviceContext* deviceContext)
	{

	}

	void ownEndOfDraw(ID3D11DeviceContext* deviceContext)
	{

	}

public:
	PlayerLineParticle()
		: Particle(ParticleEffect::PLAYERLINES, true)
	{
		this->m_maxNrOfParticles = 6000;
		this->m_textureName = L"GrayTexture.png";
		this->setShaders(streamOutVS, streamOutGS, drawVS, drawGS, drawPS);
		m_playerVelocityBuffer.initializeBuffer(Engine::get().getDevicePtr(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &m_playerVelocityData, 1);
		m_rightOffsetBuffer.initializeBuffer(Engine::get().getDevicePtr(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &m_rightOffsetData, 1);
		m_player = Engine::get().getPlayerPtr();
	}
};