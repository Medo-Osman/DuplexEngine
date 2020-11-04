#pragma once
#include "3DPCH.h"
#include "Camera.h"

struct BoundingSphere
{
	Vector3 center;
	float radius;
};

class ShadowMap
{

private:
	D3D11_VIEWPORT m_viewPort;
	ID3D11Device* m_devicePtr = nullptr;
	ID3D11DepthStencilView* m_depthMapDSV = nullptr;

	UINT m_width, m_height = 0;
	Vector4 m_direction;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStatePtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampleState = NULL;

public:
	ShadowMap(UINT width, UINT height, ID3D11Device* devicePtr, Vector4 lightDir);
	~ShadowMap();
	void bindResourcesAndSetNullRTV(ID3D11DeviceContext* context);
	void computeShadowMatrix(Vector3 playerPos);
	void createRasterState();
	
	ID3D11ShaderResourceView* m_depthMapSRV = nullptr;
	XMMATRIX m_lightViewMatrix;
	XMMATRIX m_lightProjMatrix;
	XMMATRIX m_shadowTransform;
};