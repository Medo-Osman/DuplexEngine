#include "3DPCH.h"
#include "ShadowMap.h"


ShadowMap::ShadowMap(UINT width, UINT height, ID3D11Device* devicePtr, Vector4 lightDir)
{
	m_devicePtr = devicePtr;

	m_width = width;
	m_height = height;

	m_viewPort.Width = (float)width;
	m_viewPort.Height = (float)height;
	m_viewPort.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC textureDesc = { 0 }; //DepthTexture
	textureDesc.Width = m_width;
	textureDesc.Height = m_width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* depthMap = nullptr;
	HRESULT succ = m_devicePtr->CreateTexture2D(&textureDesc, 0, &depthMap);
	assert(SUCCEEDED(succ));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	succ = m_devicePtr->CreateDepthStencilView(depthMap, &dsvDesc, &m_depthMapDSV);
	assert(SUCCEEDED(succ));
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	succ = m_devicePtr->CreateShaderResourceView(depthMap, &srvDesc, &m_depthMapSRV);
	assert(SUCCEEDED(succ));

	FLOAT borderColor[4] = { 0.f };
	D3D11_SAMPLER_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(D3D11_SAMPLER_DESC));
	sampleDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampleDesc.MipLODBias = 0;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.MinLOD = -D3D11_FLOAT32_MAX;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	succ = m_devicePtr->CreateSamplerState(&sampleDesc, m_sampleState.GetAddressOf());
	assert(SUCCEEDED(succ));

	m_direction = lightDir;
}

ShadowMap::~ShadowMap()
{
	SAFE_RELEASE(m_rasterizerStatePtr);
	SAFE_RELEASE(m_sampleState);
	SAFE_RELEASE(m_depthMapDSV);
	SAFE_RELEASE(m_depthMapSRV);

}

void ShadowMap::bindResourcesAndSetNullRTV(ID3D11DeviceContext* context)
{

	context->RSSetViewports(1, &m_viewPort);
	context->RSSetState(m_rasterizerStatePtr.Get());

	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	context->OMSetRenderTargets(1, renderTargets, m_depthMapDSV);
	context->PSSetSamplers(1, 1, m_sampleState.GetAddressOf());

	context->ClearDepthStencilView(m_depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void ShadowMap::computeShadowMatrix(Vector3 focusPos)
{
	float radius = 90.f; //Remember to change in the shader as well
	Vector4 lightPos = (-2 * radius * m_direction) + focusPos;
	Vector3 targetPos = focusPos;

	Matrix V = XMMatrixLookAtLH(XMVECTOR(lightPos), targetPos, { 0.0f, 1.0f, 0.0f, 0.0f });//Matrix::CreateLookAt(Vector3(lightPos), targetPos, Vector3(0, 1, 0));
	
	Vector3 sphereCenterLS;
	sphereCenterLS = XMVector3TransformCoord(targetPos, V);

	Matrix P = XMMatrixOrthographicOffCenterLH(
		sphereCenterLS.x - radius, //Left
		sphereCenterLS.x + radius, //Right
		sphereCenterLS.y - radius, //Bottom
		sphereCenterLS.y + radius, //Top
		sphereCenterLS.z - radius, //Near
		sphereCenterLS.z + radius); //Far

	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f );

	Matrix S = V * P * T;

	m_lightViewMatrix = V;
	m_lightProjMatrix = P;
	m_shadowTransform = XMMatrixTranspose(S);

}

void ShadowMap::createRasterState()
{
	HRESULT hr = 0;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterizerDesc.DepthBias = 10000;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 4.0f;
	rasterizerDesc.DepthClipEnable = true;

	hr = m_devicePtr->CreateRasterizerState(&rasterizerDesc, m_rasterizerStatePtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error creating rasterizerState");
}
