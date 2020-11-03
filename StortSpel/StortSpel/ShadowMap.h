#pragma once
#include "3DPCH.h"
#include "Camera.h"

class ShadowMap
{

private:
	D3D11_VIEWPORT m_viewPort;
	ID3D11Device* m_devicePtr = nullptr;
	ID3D11DepthStencilView* m_depthMapDSV = nullptr;
	ID3D11ShaderResourceView* m_depthMapSRV = nullptr;

	UINT m_width, m_height = 0;
	Vector4 m_direction;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStatePtr = NULL;

public:
	ShadowMap(UINT width, UINT height, ID3D11Device* devicePtr, Vector4 lightDir);
	void bindResourcesAndSetNullRTV(ID3D11DeviceContext* context);
	void computeShadowMatrix();
	void createRasterState();
	
	XMMATRIX m_lightViewMatrix;
	XMMATRIX m_lightProjMatrix;
	XMMATRIX m_shadowTransform;
};