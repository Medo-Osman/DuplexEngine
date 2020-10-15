#include"3DPCH.h"
//#include "MeshComponent.h"
#include"Renderer.h"

Renderer::Renderer()
{
	m_rTargetViewsArray = new ID3D11RenderTargetView * [8];
	
	//Variables

}

void Renderer::setPointLightRenderStruct(lightBufferStruct& buffer)
{
	m_lightBuffer.updateBuffer(m_dContextPtr.Get(), &buffer);
}

void Renderer::release()
{
	m_devicePtr.Get()->Release();
	m_dContextPtr->Release();
	m_swapChainPtr->Release();
	m_debugPtr->Release();
	m_rTargetViewPtr->Release();
	m_swapChainBufferPtr->Release();
	m_depthStencilBufferPtr->Release();
	m_depthStencilViewPtr->Release();
	m_depthStencilStatePtr->Release();

	m_geometryShaderResourceView->Release();
	m_geometryUnorderedAccessView->Release();
	m_geometryTexture->Release();

	m_downSampledShaderResourceView->Release();
	m_downSampledUnorderedAccessView->Release();
	m_downSampledTexture->Release();

	//m_vertexShaderConstantBuffer.release();
	m_rasterizerStatePtr->Release();

	m_rTargetViewPtr->Release();
	for (int i = 0; i < 8; i++)
	{
		if (m_rTargetViewsArray != nullptr)
		{
			SAFE_RELEASE(m_rTargetViewsArray[i]);
		}
	}

	delete[] m_rTargetViewsArray;
}

Renderer::~Renderer()
{
	for (std::pair<ShaderProgramsEnum, ShaderProgram*> element : m_compiledShaders)
	{
		delete element.second;
	}
}

HRESULT Renderer::initialize(const HWND& window)
{
	HRESULT hr;
	m_window = window;
	
	m_settings = Engine::get().getSettings();

	hr = createDeviceAndSwapChain(); //Device and context creation
	if (!SUCCEEDED(hr)) return hr;

	//Get swapchian buffer
	hr = m_swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)m_swapChainBufferPtr.GetAddressOf());

	if (!SUCCEEDED(hr)) return hr;

	//Get Debugger
	hr = m_devicePtr->QueryInterface(__uuidof(ID3D11Debug), (void**)m_debugPtr.GetAddressOf());
	if (!SUCCEEDED(hr)) return hr;


	hr = m_devicePtr->CreateRenderTargetView(m_swapChainBufferPtr.Get(), 0, m_rTargetViewPtr.GetAddressOf());
	if (!SUCCEEDED(hr)) return hr;

	hr = createDepthStencil();
	if (!SUCCEEDED(hr)) return hr;
	
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.Width = m_settings.width;
	textureDesc.Height = m_settings.height;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &m_geometryTexture);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateRenderTargetView(m_geometryTexture.Get(), 0, &m_rTargetViewsArray[1]);
	if (!SUCCEEDED(hr)) return hr;

	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.Width = m_settings.width/2;
	textureDesc.Height = m_settings.height/2;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &m_downSampledTexture);
	if (!SUCCEEDED(hr)) return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = m_devicePtr->CreateShaderResourceView(m_downSampledTexture.Get(), &srvDesc, &m_downSampledShaderResourceView);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateShaderResourceView(m_geometryTexture.Get(), &srvDesc, &m_geometryShaderResourceView);
	if (!SUCCEEDED(hr)) return hr;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	
	hr = m_devicePtr->CreateUnorderedAccessView(m_downSampledTexture.Get(), &uavDesc, &m_downSampledUnorderedAccessView);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateUnorderedAccessView(m_geometryTexture.Get(), &uavDesc, &m_geometryUnorderedAccessView);
	if (!SUCCEEDED(hr)) return hr;

	compileAllShaders(&m_compiledShaders, m_devicePtr.Get(), m_dContextPtr.Get(), m_depthStencilViewPtr.Get());

	createViewPort(m_defaultViewport, m_settings.width, m_settings.height);
	rasterizerSetup();



	//Setup samplerstate
	D3D11_SAMPLER_DESC samplerStateDesc;
	ZeroMemory(&samplerStateDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.MipLODBias = 0.0f;
	samplerStateDesc.MaxAnisotropy = 1;
	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerStateDesc.MinLOD = 0;
	samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_devicePtr->CreateSamplerState(&samplerStateDesc, &m_psSamplerState);
	assert(SUCCEEDED(hr) && "Failed to create SampleState");
	m_dContextPtr->PSSetSamplers(0, 1, m_psSamplerState.GetAddressOf());

	m_perObjectConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &perObjectMVP(), 1);
	m_dContextPtr->VSSetConstantBuffers(0, 1, m_perObjectConstantBuffer.GetAddressOf());
	m_skyboxConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &skyboxMVP(), 1);
	m_dContextPtr->VSSetConstantBuffers(1, 1, m_skyboxConstantBuffer.GetAddressOf());

	lightBufferStruct initalLightData; //Not sure why, but it refuses to take &lightBufferStruct() as argument on line below
	m_lightBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &initalLightData, 1);
	m_dContextPtr->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());

	m_cameraBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &cameraBufferStruct(), 1);
	m_dContextPtr->PSSetConstantBuffers(1, 1, m_cameraBuffer.GetAddressOf());

	m_dContextPtr->PSSetConstantBuffers(2, 1, m_perObjectConstantBuffer.GetAddressOf());

	Engine::get().setDeviceAndContextPtrs(m_devicePtr.Get(), m_dContextPtr.Get());
	ResourceHandler::get().setDeviceAndContextPtrs(m_devicePtr.Get(), m_dContextPtr.Get());
	m_camera = Engine::get().getCameraPtr();

	/////////////////////////////////////////////////
	D3D11_DEPTH_STENCIL_DESC skyboxDSD;
	ZeroMemory(&skyboxDSD, sizeof(D3D11_DEPTH_STENCIL_DESC));
	skyboxDSD.DepthEnable = true;
	skyboxDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	skyboxDSD.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_devicePtr->CreateDepthStencilState(&skyboxDSD, &skyboxDSSPtr);
	/////////////////////////////////////////////////

	return hr;
}

HRESULT Renderer::createDeviceAndSwapChain()
{
	HRESULT hr = 0;

	DXGI_SWAP_CHAIN_DESC sChainDesc;
	ZeroMemory(&sChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	sChainDesc.BufferCount = 2;
	sChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sChainDesc.Windowed = true; //Windowed or fullscreen
	sChainDesc.BufferDesc.Height = m_settings.height; //Size of buffer in pixels, height
	sChainDesc.BufferDesc.Width = m_settings.width; //Size of window in pixels, width
	sChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI Format, 8 bits for Red, green, etc
	sChainDesc.BufferDesc.RefreshRate.Numerator = 60; //IF vSync is enabled and fullscreen, this specifies the max refreshRate
	sChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	sChainDesc.SampleDesc.Quality = 0;
	sChainDesc.SampleDesc.Count = 8;
	sChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; //What to do with buffer when swap occur
	sChainDesc.OutputWindow = m_window;

	hr = D3D11CreateDeviceAndSwapChain(
		NULL, //Adapter, null is default
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, //Driver to use
		NULL, //Software Driver DLL, Use if software driver is used in the above parameter.
		D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG,
		featureLevelArray, //Array of featurelevels
		nrOfFeatureLevels, //nr of featurelevels
		D3D11_SDK_VERSION, //SDK Version, D3D11_SDL_VERSION grabs the currently installed sdk version.
		&sChainDesc, //SwapChain Description
		m_swapChainPtr.GetAddressOf(), //Swapchain pointer
		m_devicePtr.GetAddressOf(), //Device pointer
		&m_fLevel, //Feature Level
		m_dContextPtr.GetAddressOf() //Device COntext Pointer
	);

	return hr;
}

HRESULT Renderer::createDepthStencil()
{
	HRESULT hr = 0;
	//Create depthstencil view, page 35
	D3D11_TEXTURE2D_DESC depthTextureDesc;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.CPUAccessFlags = 0;
	depthTextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	depthTextureDesc.Height = m_settings.height;
	depthTextureDesc.Width = m_settings.width;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.MiscFlags = 0;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.SampleDesc.Count = 8;
	depthTextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	hr = m_devicePtr->CreateTexture2D(&depthTextureDesc, 0, &m_depthStencilBufferPtr);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateDepthStencilView(m_depthStencilBufferPtr.Get(), NULL, m_depthStencilViewPtr.GetAddressOf());

	//Binding rendertarget and depth target to pipline
//Best practice to use an array even if only using one rendertarget
	m_rTargetViewsArray[0] = m_rTargetViewPtr.Get();
	m_dContextPtr->OMSetRenderTargets(1, m_rTargetViewsArray, m_depthStencilViewPtr.Get());

	//Depth stencil state desc
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL; //Function to determin if pixel should be drawn. LESS_EQUAL means if z value is less or equal then overwrite.
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL; //Stencil active with MAKS_ALL
	depthStencilDesc.DepthEnable = true; //Enable depth test

	hr = m_devicePtr->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStatePtr.GetAddressOf());

	return hr;
}

void Renderer::createViewPort(D3D11_VIEWPORT& viewPort, const int& width, const int& height) const
{
	//Viewport desc
	ZeroMemory(&viewPort, sizeof(D3D11_VIEWPORT));

	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (float)width;
	viewPort.Height = (float)height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
}

void Renderer::rasterizerSetup()
{
	HRESULT hr = 0;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

	hr = m_devicePtr->CreateRasterizerState(&rasterizerDesc, m_rasterizerStatePtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error creating rasterizerState");
}

void Renderer::update(const float& dt)
{
	
}

void Renderer::render()
{
	//Update camera position for pixel shader buffer
	cameraBufferStruct cameraStruct = cameraBufferStruct{ m_camera->getPosition() };
	m_cameraBuffer.updateBuffer(m_dContextPtr.Get(), &cameraStruct);

	//Clear the context of render and depth target
	m_dContextPtr->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, m_rTargetViewsArray, m_depthStencilViewPtr.GetAddressOf());
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
	{
		if (m_rTargetViewsArray[i] != NULL)
		{
			m_dContextPtr->ClearRenderTargetView(m_rTargetViewsArray[i], m_clearColor);
		}
	}

	if (m_depthStencilViewPtr)
	{
		m_dContextPtr->ClearDepthStencilView(m_depthStencilViewPtr.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	//NormalPass
	m_dContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT offset = 0;

	m_dContextPtr->RSSetViewports(1, &m_defaultViewport); //Set default viewport
	m_rTargetViewsArray[0] = m_rTargetViewPtr.Get();
	m_dContextPtr->OMSetRenderTargets(1, m_rTargetViewsArray, m_depthStencilViewPtr.Get());
	m_dContextPtr->RSSetState(m_rasterizerStatePtr.Get());

	// For Texture Testing only
	//ID3D11ShaderResourceView* srv = ResourceHandler::get().loadTexture(L"T_CircusTent_D.png");
	//m_dContextPtr->PSSetShaderResources(0, 1, &srv);
	// Skybox constant buffer:
	m_dContextPtr->OMSetDepthStencilState(skyboxDSSPtr, 0);
	skyboxMVP constantBufferSkyboxStruct;
	XMMATRIX W = XMMatrixTranslation(XMVectorGetX(m_camera->getPosition()), XMVectorGetY(m_camera->getPosition()), XMVectorGetZ(m_camera->getPosition()));
	XMMATRIX V = m_camera->getViewMatrix();
	XMMATRIX P = m_camera->getProjectionMatrix();
	constantBufferSkyboxStruct.mvpMatrix = XMMatrixTranspose(W * V * P);
	m_skyboxConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferSkyboxStruct);

	for (auto& component : *Engine::get().getMeshComponentMap())
	{
		ShaderProgramsEnum meshShaderEnum = component.second->getShaderProgEnum();
		if (m_currentSetShaderProg != meshShaderEnum)
		{
			m_compiledShaders[meshShaderEnum]->setShaders();
			m_currentSetShaderProg = meshShaderEnum;
		}
			
		
		Material* meshMatPtr = component.second->getMaterialPtr();
		if (m_currentSetMaterialId != meshMatPtr->getMaterialId())
		{
			meshMatPtr->setMaterial(m_compiledShaders[meshShaderEnum], m_dContextPtr.Get());
			m_currentSetMaterialId = meshMatPtr->getMaterialId();
		}
			
		perObjectMVP constantBufferPerObjectStruct;
		component.second->getMeshResourcePtr()->set(m_dContextPtr.Get());
		constantBufferPerObjectStruct.projection = XMMatrixTranspose(m_camera->getProjectionMatrix());
		constantBufferPerObjectStruct.view = XMMatrixTranspose(m_camera->getViewMatrix());
		constantBufferPerObjectStruct.world = XMMatrixTranspose(Engine::get().getEntity(component.second->getParentEntityIdentifier())->calculateWorldMatrix() * component.second->calculateWorldMatrix());
		constantBufferPerObjectStruct.mvpMatrix = constantBufferPerObjectStruct.projection * constantBufferPerObjectStruct.view * constantBufferPerObjectStruct.world;

		m_perObjectConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferPerObjectStruct);

		m_dContextPtr->DrawIndexed(component.second->getMeshResourcePtr()->getIndexBuffer().getSize(), 0, 0);
	}

	// Bloom Filter
	//m_dContextPtr->CSSetShader()
	m_dContextPtr->CSSetShaderResources(0, 1, &m_downSampledShaderResourceView);
	m_dContextPtr->CSGetUnorderedAccessViews(0, 1, &m_downSampledUnorderedAccessView);
	m_swapChainPtr->Present(0, 0);
}

ID3D11Device* Renderer::getDevice()
{
	return m_devicePtr.Get();
}

ID3D11DeviceContext* Renderer::getDContext()
{
	return m_dContextPtr.Get();
}

ID3D11DepthStencilView* Renderer::getDepthStencilView()
{
	return m_depthStencilViewPtr.Get();
}