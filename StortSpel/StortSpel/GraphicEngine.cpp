#include"3DPCH.h"
#include"GraphicEngine.h"

GraphicEngine::GraphicEngine()
{
	m_rTargetViewsArray = new ID3D11RenderTargetView*[8];

	//Variables
	m_width = m_startWidth;
	m_height = m_startHeight;
}


GraphicEngine::~GraphicEngine()
{

}

void GraphicEngine::release()
{
	m_devicePtr->Release();
	m_dContextPtr->Release();
	m_swapChainPtr->Release();
	m_debugPtr->Release();
	m_rTargetViewPtr->Release();
	m_swapChainBufferPtr->Release();
	m_depthStencilBufferPtr->Release();
	m_depthStencilViewPtr->Release();
	m_depthStencilStatePtr->Release();
	m_vertexLayoutPtr->Release();
	m_vertexBuffer.release();
	m_vertexShaderPtr->Release();
	m_vertexShaderBufferPtr->Release();
	m_vertexShaderConstantBuffer.release();
	m_rasterizerStatePtr->Release();
	m_pixelShaderPtr->Release();
	m_pixelBufferPtr->Release();
	m_psSamplerState->Release();

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


HRESULT GraphicEngine::initialize(const HWND& window)
{
	HRESULT hr;
	m_window = window;
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

	createAndSetShaders();
	hr = setUpInputAssembler();
	assert(SUCCEEDED(hr) && "Failed when configuring inputassember");
	createViewPort(m_defaultViewport, m_width, m_height);
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

	ColorVertex triangle[3] =
	{
		ColorVertex({-1.f, -1.f, 0.f}, {1.f, 0.f, 0.f}),
		ColorVertex({-1.f, 1.f, 0.f}, {1.f, 0.f, 0.f}),
		ColorVertex({1.f, 1.f, 0.f}, {1.f, 0.f, 0.f})
	};
	m_vertexBuffer.initializeBuffer(m_devicePtr.Get(), false, D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER, triangle, 3);
	m_vertexShaderConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &cbVSWVPMatrix(), 1);
	m_camera.setProjectionMatrix(80.f, (float)m_height/(float)m_width, 0.01f, 1000.0f);
	m_camera.setPosition({ 0.0f, 0.0f, -5.0f, 1.0f });

	return hr;
}

HRESULT GraphicEngine::createDeviceAndSwapChain()
{
	HRESULT hr = 0;

	DXGI_SWAP_CHAIN_DESC sChainDesc;
	ZeroMemory(&sChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	sChainDesc.BufferCount = 2;
	sChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sChainDesc.Windowed = true; //Windowed or fullscreen
	sChainDesc.BufferDesc.Height = m_height; //Size of buffer in pixels, height
	sChainDesc.BufferDesc.Width = m_width; //Size of window in pixels, width
	sChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //DXGI Format, 8 bits for Red, green, etc
	sChainDesc.BufferDesc.RefreshRate.Numerator = 60; //IF vSync is enabled and fullscreen, this specifies the max refreshRate
	sChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	sChainDesc.SampleDesc.Quality = 0;
	sChainDesc.SampleDesc.Count = 1;
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

HRESULT GraphicEngine::createDepthStencil()
{
	HRESULT hr = 0;
	//Create depthstencil view, page 35
	D3D11_TEXTURE2D_DESC depthTextureDesc;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
	depthTextureDesc.CPUAccessFlags = 0;
	depthTextureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D32_FLOAT;
	depthTextureDesc.Height = m_height;
	depthTextureDesc.Width = m_width;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.MiscFlags = 0;
	depthTextureDesc.SampleDesc.Quality = 0;
	depthTextureDesc.SampleDesc.Count = 1;
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

HRESULT GraphicEngine::setUpInputAssembler()
{

	HRESULT hr = m_devicePtr->CreateInputLayout(Layouts::colorVertexLayout, //VertexLayout
		ARRAYSIZE(Layouts::colorVertexLayout), //Nr of elements 
		m_vertexShaderBufferPtr->GetBufferPointer(),
		m_vertexShaderBufferPtr->GetBufferSize(), //Bytecode length
		m_vertexLayoutPtr.GetAddressOf()
	);

	if (SUCCEEDED(hr))
	{
		m_dContextPtr->IASetInputLayout(m_vertexLayoutPtr.Get());
		m_dContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	return hr;
}

void GraphicEngine::createViewPort(D3D11_VIEWPORT &viewPort, const int &width, const int &height) const
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

void GraphicEngine::createAndSetShaders()
{
	HRESULT hr = 0;
	//VertexShaderBasic compile and create
	hr = compileShader(L"VertexShaderBasic.hlsl", "main", "vs_5_0", m_vertexShaderBufferPtr.GetAddressOf() );
	assert(SUCCEEDED(hr) && "Error when compiling VertexShader from file");

	hr = m_devicePtr->CreateVertexShader(m_vertexShaderBufferPtr->GetBufferPointer(), m_vertexShaderBufferPtr->GetBufferSize(), NULL, m_vertexShaderPtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error when creating  VertexShader");


	//Basic pixel shader compile and create
	hr = compileShader(L"BasicPixelShader.hlsl", "main", "ps_5_0", m_pixelBufferPtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error when compiling pixelShader from file");

	hr = m_devicePtr->CreatePixelShader(m_pixelBufferPtr->GetBufferPointer(), m_pixelBufferPtr->GetBufferSize(), NULL, m_pixelShaderPtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error when creating  PixelShader");
}

void GraphicEngine::rasterizerSetup()
{
	HRESULT hr = 0;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

	hr = m_devicePtr->CreateRasterizerState(&rasterizerDesc, m_rasterizerStatePtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error creating rasterizerState");
}

void GraphicEngine::handleInput(Mouse* mousePtr, Keyboard* keyboardPtr, const float& dt)
{
	while (!mousePtr->empty())
	{
		MouseEvent mEvent = mousePtr->readEvent();
		if (mEvent.isValid())
		{
			if (mEvent.getEvent() == Event::MouseRAW_MOVE)
			{
				m_camera.controllCameraRotation(mEvent, dt);
			}
		}
	}

	while (!keyboardPtr->empty())
	{
		KeyboardEvent mEvent = keyboardPtr->readKey();
		if (mEvent.isValid())
		{
			//Do on buttondown/button up things here.
			if (mEvent.getEvent() == Event::Pressed)
			{
				if (mEvent.getKey() == 'W')
				{
					//Do things
				}
			}
		}
	}

	m_camera.controllCameraPosition(keyboardPtr, dt); //ControllCameraPosition only uses an array of what keys are pressed.
}

void GraphicEngine::update(const float& dt)
{

}

void GraphicEngine::render()
{

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
	m_dContextPtr->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), m_vertexBuffer.getStridePointer(), &offset);
	m_dContextPtr->RSSetViewports(1, &m_defaultViewport); //Set defaul viewport
	m_rTargetViewsArray[0] = m_rTargetViewPtr.Get();
	m_dContextPtr->OMSetRenderTargets(1, m_rTargetViewsArray, m_depthStencilViewPtr.Get());
	this->setPipelineShaders(m_vertexShaderPtr.Get(), nullptr, nullptr, nullptr, m_pixelShaderPtr.Get());
	m_dContextPtr->RSSetState(m_rasterizerStatePtr.Get());

	cbVSWVPMatrix wvp;
	wvp.wvpMatrix = XMMatrixTranspose( m_camera.getViewMatrix()* m_camera.getProjectionMatrix());
	m_vertexShaderConstantBuffer.updateBuffer(m_dContextPtr.Get(), &wvp);
	m_dContextPtr->VSSetConstantBuffers(0, 1, m_vertexShaderConstantBuffer.GetAddressOf());

	m_dContextPtr->Draw(3, 0);
	
	m_swapChainPtr->Present(0, 0);
}

void GraphicEngine::setPipelineShaders(ID3D11VertexShader* vsPtr, ID3D11HullShader* hsPtr, ID3D11DomainShader* dsPtr, ID3D11GeometryShader* gsPtr, ID3D11PixelShader* psPtr)
{
	m_dContextPtr->VSSetShader(vsPtr, nullptr, 0);
	m_dContextPtr->HSSetShader(hsPtr, nullptr, 0);
	m_dContextPtr->DSSetShader(dsPtr, nullptr, 0);
	m_dContextPtr->GSSetShader(gsPtr, nullptr, 0);
	m_dContextPtr->PSSetShader(psPtr, nullptr, 0);
}

ID3D11Device* GraphicEngine::getDevice()
{
	return m_devicePtr.Get();
}

ID3D11DeviceContext* GraphicEngine::getDContext()
{
	return m_dContextPtr.Get();
}