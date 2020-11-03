#include "3DPCH.h"
#include "ShadowMap.h"


ShadowMap::ShadowMap(UINT width, UINT height, ID3D11Device* devicePtr, Vector4 lightDir)
{
	m_devicePtr = devicePtr;

	m_width = width;
	m_height = width;

	m_viewPort.Width = (float)width;
	m_viewPort.Height = (float)height;
	m_viewPort.MaxDepth = 1.0f;

	D3D11_TEXTURE2D_DESC textureDesc = { 0 }; //DepthTexture
	textureDesc.Width = m_width;
	textureDesc.Height = m_width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //24 bit for the red channel, 8 for the green.
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* depthMap = nullptr;
	HRESULT succ = m_devicePtr->CreateTexture2D(&textureDesc, 0, &depthMap);
	assert(SUCCEEDED(succ));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //32-bit z-buffer, 24 for depth and 8 for stencil.
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	succ = m_devicePtr->CreateDepthStencilView(depthMap, &dsvDesc, &m_depthMapDSV);
	assert(SUCCEEDED(succ));
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	succ = m_devicePtr->CreateShaderResourceView(depthMap, &srvDesc, &m_depthMapSRV);
	assert(SUCCEEDED(succ));

	m_direction = lightDir;

	//depthMap->Release();

}

void ShadowMap::bindResourcesAndSetNullRTV(ID3D11DeviceContext* context)
{

	context->RSSetViewports(1, &m_viewPort);
	context->RSSetState(m_rasterizerStatePtr.Get());

	/*ID3D11RenderTargetView* renderTargets[1] = { 0 };
	context->OMSetRenderTargets(1, renderTargets, m_depthMapDSV);*/

	
	//context->ClearDepthStencilView(m_depthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::computeShadowMatrix(Vector3 playerPos)
{
	float radius = 60.f;
	Vector4 lightPos = -2 * radius * m_direction;
	Vector3 targetPos = playerPos;//Vector3(0, 0, 0);

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
	/*Matrix T = Matrix();
	T._11 = 0.5f;
	T._22 = -0.5f;
	T._33 = 1.0f;
	T._41 = 0.5f;
	T._42 = 0.5f;
	T._44 = 1.0;*/
	//m_transformMatrix = XMMatrixOrthographicLH((float)m_width, m_height, 0.f, 1.f);//XMMatrixShadow(m_direction, m_direction * -5); //Calc actual radius here

	//Matrix mat = XMMatrixOrthographicOffCenterLH()


	Matrix S = V * P * T;

	m_lightViewMatrix = V;
	m_lightProjMatrix = P;
	m_shadowTransform = S;

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
	rasterizerDesc.SlopeScaledDepthBias = 1.0f;
	rasterizerDesc.DepthClipEnable = true;


	hr = m_devicePtr->CreateRasterizerState(&rasterizerDesc, m_rasterizerStatePtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error creating rasterizerState");
}
