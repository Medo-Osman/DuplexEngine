#include"3DPCH.h"
//#include "MeshComponent.h"
#include"Renderer.h"
#include"ParticleComponent.h"
#include<memory>
#include"OptimizationConfiguration.h"


Renderer::Renderer()
{
	//m_rTargetViewsArray = new ID3D11RenderTargetView * [8];
	
	//Variables
	m_shadowMap = nullptr;
}

void Renderer::drawBoundingVolumes()
{
	m_dContextPtr->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	m_dContextPtr->OMSetDepthStencilState(m_states->DepthNone(), 0);
	m_dContextPtr->RSSetState(m_states->CullNone());

	m_dContextPtr->IASetInputLayout(m_inputLayout.Get());

	m_batch->Begin();

	this->m_effect->SetView(m_switchCamera ? m_testCamera.getViewMatrix() : m_camera->getViewMatrix());
	this->m_effect->SetProjection(m_switchCamera ? m_testCamera.getProjectionMatrix() : m_camera->getProjectionMatrix());
	this->m_effect->Apply(m_dContextPtr.Get());

	for (int i = 0; i < (int)m_boundingVolumes.size(); i++)
	{
		BoundingVolumeHolder bvh = m_boundingVolumes[i];
		switch (bvh.getBoundingVolumeType())
		{
		case BoundingVolumeTypes::FRUSTUM:
			DX::Draw(m_batch.get(), *(BoundingFrustum*)bvh.getBoundingVolumePtr(), Colors::Red);
			break;
		case BoundingVolumeTypes::BOX:
			DX::Draw(m_batch.get(), *(BoundingBox*)bvh.getBoundingVolumePtr(), Colors::Blue);
			break;
		case BoundingVolumeTypes::ORIENTEDBOX:
		{
			BoundingOrientedBox b = *(BoundingOrientedBox*)bvh.getBoundingVolumePtr();
			DX::Draw(m_batch.get(), b, Colors::Cyan);
			break;
		}
		case BoundingVolumeTypes::SPHERE:
			DX::Draw(m_batch.get(), *(DirectX::BoundingSphere*)bvh.getBoundingVolumePtr(), Colors::Blue);
			break;
		default:
			break;
		}

		delete bvh.getBoundingVolumePtr();
	}

	m_batch->End();

	m_boundingVolumes.clear();

	this->m_dContextPtr->OMSetDepthStencilState(m_depthStencilStatePtr.Get(), NULL);
	m_dContextPtr->RSSetState(m_rasterizerStatePtr.Get());

	m_dContextPtr->PSSetConstantBuffers(4, 1, m_globalConstBuffer.GetAddressOf());
	m_dContextPtr->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());
	m_dContextPtr->PSSetConstantBuffers(1, 1, m_cameraBuffer.GetAddressOf());
	m_dContextPtr->PSSetConstantBuffers(2, 1, m_perObjectConstantBuffer.GetAddressOf());
	m_dContextPtr->PSSetConstantBuffers(3, 1, m_currentMaterialConstantBuffer.GetAddressOf());
	m_dContextPtr->PSSetConstantBuffers(5, 1, m_atmosphericFogConstBuffer.GetAddressOf());

}



bool preformCullOnMeshComponent(BoundingFrustum* frust ,MeshComponent* meshComponent, Entity* parentEntity)
{
	bool draw = true;
	XMFLOAT3 min, max;
	Vector3 scale = meshComponent->getScaling() * parentEntity->getScaling();
	XMVECTOR pos = parentEntity->getTranslation();
	pos += meshComponent->getTranslation();
	pos += meshComponent->getMeshResourcePtr()->getBoundsCenter() * scale;
	XMFLOAT3 posFloat3;
	XMStoreFloat3(&posFloat3, pos);

	if (frust->Contains(pos) != ContainmentType::CONTAINS)
	{
		meshComponent->getMeshResourcePtr()->getMinMax(min, max);
		XMFLOAT3 ext = (max - min) / 2;
		ext = ext * scale;
		XMFLOAT4 rot = parentEntity->getRotation() * meshComponent->getRotation();
		BoundingOrientedBox box(posFloat3, ext, rot);

		ContainmentType contType = frust->Contains(box);
		draw = (contType == ContainmentType::INTERSECTS || contType == ContainmentType::CONTAINS);
	}
	else
	{
		draw = true;
	}


	return draw;
}

void Renderer::renderSortedScene(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P)
{
	m_drawn = 0;

	std::vector<std::vector<DrawCallStruct>>* drawCalls = Engine::get().getDrawCallsPtr();

	for (int i = 0; i < NR_OF_SHADER_PROGRAM_ENUMS; i++) // Loop over all different types of shader enums
	{
		m_currentSetShaderProg = ShaderProgramsEnum(i);
		if (m_compiledShaders[m_currentSetShaderProg] == nullptr)
			continue;
		m_compiledShaders[m_currentSetShaderProg]->setShaders();

		for (int j = 0; j < drawCalls->at(i).size(); j++) // Loop over all meshes using that shader enum
		{
			// >>>>>>>>>>>>>>>>>>>>> Step 1: Get entity & meshComp to use
			MeshComponent* meshComponent = drawCalls->at(i).at(j).mesh;
			Entity* parentEntity;
			std::unordered_map<std::string, Entity*>* entityMap = Engine::get().getEntityMap();

			if (meshComponent->getParentEntityIdentifier() == PLAYER_ENTITY_NAME)
				parentEntity = Engine::get().getPlayerPtr()->getPlayerEntity();
			else if (meshComponent->getParentEntityIdentifier() == (const std::string) "3DMarker")
				parentEntity = Engine::get().getPlayerPtr()->get3DMarkerEntity();
			else
				parentEntity = (*Engine::get().getEntityMap())[meshComponent->getParentEntityIdentifier()];

			
			// >>>>>>>>>>>>>>>>>>>>> Step 2: Cull all meshes not seen
			bool meshIsInsideFrustrum = true;
			if (m_camera->frustumCullingOn && parentEntity->m_canCull && USE_FRUSTUM_CULLING)
			{
				meshIsInsideFrustrum = preformCullOnMeshComponent(frust, meshComponent, parentEntity);
			}

			// >>>>>>>>>>>>>>>>>>>>> Step 3: Finalize mesh for drawing
			if (meshIsInsideFrustrum && meshComponent->isVisible())
			{
				// ----------------- Step 3.1: Set constant buffers
				perObjectMVP constantBufferPerObjectStruct;
				meshComponent->getMeshResourcePtr()->set(m_dContextPtr.Get());
				constantBufferPerObjectStruct.projection = XMMatrixTranspose(m_camera->getProjectionMatrix());
				constantBufferPerObjectStruct.view = XMMatrixTranspose(m_camera->getViewMatrix());
				constantBufferPerObjectStruct.world = XMMatrixTranspose(XMMatrixMultiply(meshComponent->calculateWorldMatrix(), parentEntity->calculateWorldMatrix()));
				constantBufferPerObjectStruct.mvpMatrix = constantBufferPerObjectStruct.projection * constantBufferPerObjectStruct.view * constantBufferPerObjectStruct.world;
				m_perObjectConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferPerObjectStruct);

				// ----------------- Step 3.2: Check if the mesh has animations
				AnimatedMeshComponent* animMeshComponent = dynamic_cast<AnimatedMeshComponent*>(meshComponent);
				if (animMeshComponent != nullptr)
				{
					m_skelAnimationConstantBuffer.updateBuffer(m_dContextPtr.Get(), animMeshComponent->getAllAnimationTransforms());
				}

				// ----------------- Step 3.3: Set material constant buffers
				int material_ID = drawCalls->at(i).at(j).material_ID;
				int material_IDX = drawCalls->at(i).at(j).material_IDX;
				if (material_ID != m_currentSetMaterialId)
				{
					Material* meshMaterial = meshComponent->getMaterialPtr(material_IDX);
					meshMaterial->setMaterial(m_compiledShaders[m_currentSetShaderProg], m_dContextPtr.Get());
					m_currentSetMaterialId = material_ID;

					MATERIAL_CONST_BUFFER currentMaterialConstantBufferData;
					currentMaterialConstantBufferData.UVScale = meshMaterial->getMaterialParameters().UVScale;
					currentMaterialConstantBufferData.roughness = meshMaterial->getMaterialParameters().roughness;
					currentMaterialConstantBufferData.metallic = meshMaterial->getMaterialParameters().metallic;
					currentMaterialConstantBufferData.textured = meshMaterial->getMaterialParameters().textured;
					currentMaterialConstantBufferData.emissiveStrength = meshMaterial->getMaterialParameters().emissiveStrength;
					m_currentMaterialConstantBuffer.updateBuffer(m_dContextPtr.Get(), &currentMaterialConstantBufferData);
				}

				// ----------------- Step 3.4:
				m_dContextPtr->PSSetShaderResources(7, 1, m_shadowMap->m_depthMapSRV.GetAddressOf());
				m_dContextPtr->PSSetShaderResources(8, 1, m_SSAOShaderResourceViewPtr.GetAddressOf());
				std::pair<std::uint32_t, std::uint32_t> offsetAndSize = meshComponent->getMeshResourcePtr()->getMaterialOffsetAndSize(material_IDX);
				m_dContextPtr->DrawIndexed(offsetAndSize.second, offsetAndSize.first, 0);
			}
		}
	}
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_dContextPtr->PSSetShaderResources(7, 1, nullSRV);
}

void Renderer::setPointLightRenderStruct(lightBufferStruct& buffer)
{
	m_lightBuffer.updateBuffer(m_dContextPtr.Get(), &buffer);
}

void Renderer::setFullScreen(BOOL val)
{
	m_swapChainPtr->SetFullscreenState(val, NULL);
	m_isFullscreen = val;
}

bool Renderer::isFullscreen()
{
	return m_isFullscreen;
}

void Renderer::release()
{
	delete m_shadowMap;
	skyboxDSSPtr->Release();
	m_blendStateNoBlendPtr->Release();

	for (std::pair<ShaderProgramsEnum, ShaderProgram*> element : m_compiledShaders)
	{
		delete element.second;
	}
	
	Particle::cleanStaticDataForParticles();

	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

Renderer::~Renderer()
{
	/*delete *m_devicePtr.ReleaseAndGetAddressOf();
	m_dContextPtr.Reset();
	m_swapChainPtr.Reset();
	m_geometryRenderTargetViewPtr.Reset();
	m_finalRenderTargetViewPtr.Reset();

	m_depthStencilViewPtr.Reset();
	m_depthStencilStatePtr.Reset();

	m_glowMapShaderResourceView.Reset();
	m_glowMapRenderTargetViewPtr.Reset();

	m_downSampledShaderResourceView.Reset();
	m_downSampledUnorderedAccessView.Reset();
	m_geometryShaderResourceView.Reset();
	m_geometryUnorderedAccessView.Reset();
	m_blurShaderResourceView.Reset();
	m_blurUnorderedAccessView.Reset();

	m_CSDownSample.Reset();
	m_CSBlurr.Reset();

	m_rasterizerStatePtr.Reset();

	m_psSamplerState.Reset();

	m_perObjectConstantBuffer.release();
	m_lightBuffer.release();
	m_cameraBuffer.release();
	m_skyboxConstantBuffer.release();
	m_shadowConstantBuffer.release();
	
	m_skelAnimationConstantBuffer.release();
	m_currentMaterialConstantBuffer.release();

	m_blurBuffer.release();
	m_renderQuadBuffer.release();*/

	//m_geometryPassRTVs[0]->Release();
	//m_geometryPassRTVs[1]->Release();

	//HRESULT hr = this->m_debugPtr->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//assert(SUCCEEDED(hr));
	//m_debugPtr.ReleaseAndGetAddressOf();

	printLiveObject();
}

HRESULT Renderer::initialize(const HWND& window)
{
	HRESULT hr;
	m_window = window;
	
	m_settings = Engine::get().getSettings();
	m_flyingCamera.setIsFlyingCamera(true);

	hr = createDeviceAndSwapChain(); //Device and context creation
	if (!SUCCEEDED(hr)) return hr;

	//Get swapchian buffer
	ID3D11Texture2D* swapChainBufferPtr;
	hr = m_swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&swapChainBufferPtr);
	if (!SUCCEEDED(hr)) return hr;

#ifdef _DEBUG
	//Get Debugger
	hr = m_devicePtr->QueryInterface(IID_PPV_ARGS(m_debugPtr.GetAddressOf()));
						//QueryInterface(IID_PPV_ARGS(&debug))
	if (!SUCCEEDED(hr)) return hr;

//	ID3D11InfoQueue* infoQueue = nullptr;
//	hr = m_debugPtr->QueryInterface(__uuidof(ID3D11InfoQueue), (void**) & infoQueue);
//
//#ifdef _DEBUG
//	infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
//	infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
#endif


	hr = m_devicePtr->CreateRenderTargetView(swapChainBufferPtr, 0, m_finalRenderTargetViewPtr.GetAddressOf());
	if (!SUCCEEDED(hr)) return hr;
	swapChainBufferPtr->Release();

	hr = createDepthStencil();
	if (!SUCCEEDED(hr)) return hr;
	
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Width = m_settings.width;
	textureDesc.Height = m_settings.height;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 2;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* geometryTexture;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &geometryTexture);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateRenderTargetView(geometryTexture, 0, m_geometryRenderTargetViewPtr.GetAddressOf());
	if (!SUCCEEDED(hr)) return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = m_devicePtr->CreateShaderResourceView(geometryTexture, &srvDesc, &m_geometryShaderResourceView);
	if (!SUCCEEDED(hr)) return hr;

	geometryTexture->Release();

	// Blur
	m_blurBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &m_blurData, 1);

	// SSAO
	m_ssaoBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &m_ssaoData, 1);
	buildRandomVectorTexture();
	buildFrustumFarCorners((80.f / 360.f) * DirectX::XM_2PI, 1000.0f); // Change this to whatever global constants that get available later xD
	buildOffsetVectors();
	calculateBlurWeights(m_ssaoBlurData, BLUR_RADIUS, m_ssaoSigma);
	createViewPort(m_SSAOViewport, m_settings.width, m_settings.height);

	// Bloom
	initializeBloomFilter();
	compileAllShaders(&m_compiledShaders, m_devicePtr.Get(), m_dContextPtr.Get(), m_depthStencilViewPtr.Get());

	// Default stuff
	createViewPort(m_defaultViewport, m_settings.width, m_settings.height);
	rasterizerSetup();

	// Setup glowMap
	ID3D11Texture2D* glowTexture;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &glowTexture);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateRenderTargetView(glowTexture, 0, m_glowMapRenderTargetViewPtr.GetAddressOf());
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateShaderResourceView(glowTexture, &srvDesc, &m_glowMapShaderResourceView);
	if (!SUCCEEDED(hr)) return hr;

	glowTexture->Release();

	ID3D11Texture2D* normalsNDepthTexture;

	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = textureDesc.Format;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &normalsNDepthTexture);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateRenderTargetView(normalsNDepthTexture, 0, m_normalsNDepthRenderTargetViewPtr.GetAddressOf());
	if (!SUCCEEDED(hr)) return hr;


	hr = m_devicePtr->CreateShaderResourceView(normalsNDepthTexture, &srvDesc, &m_normalsNDepthSRV);
	if (!SUCCEEDED(hr)) return hr;

	normalsNDepthTexture->Release();

	ID3D11Texture2D* SSAOTexture;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;

	srvDesc.Format = textureDesc.Format;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &SSAOTexture);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateRenderTargetView(SSAOTexture, 0, m_SSAORenderTargetViewPtr.GetAddressOf());
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateShaderResourceView(SSAOTexture, &srvDesc, &m_SSAOShaderResourceViewPtr);
	if (!SUCCEEDED(hr)) return hr;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;
	hr = m_devicePtr->CreateUnorderedAccessView(SSAOTexture, &uavDesc, &m_SSAOUnorderedAccessViewPtr);
	if (!SUCCEEDED(hr)) return hr;

	SSAOTexture->Release();

	ID3D11Texture2D* SSAOblurTexture;
	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &SSAOblurTexture);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateShaderResourceView(SSAOblurTexture, &srvDesc, &m_SSAOblurSrv);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateUnorderedAccessView(SSAOblurTexture, &uavDesc, &m_SSAOblurUav);
	if (!SUCCEEDED(hr)) return hr;


	m_geometryPassRTVs[0] = m_geometryRenderTargetViewPtr.Get();
	m_geometryPassRTVs[1] = m_glowMapRenderTargetViewPtr.Get();
	//m_geometryPassRTVs[2] = m_normalsNDepthRenderTargetViewPtr.Get();

	

	D3D11_BLEND_DESC BlendState;
	ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC));
	BlendState.AlphaToCoverageEnable = FALSE;
	BlendState.IndependentBlendEnable = FALSE;
	BlendState.RenderTarget[0].BlendEnable = FALSE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_devicePtr->CreateBlendState(&BlendState, &m_blendStateNoBlendPtr);

	BlendState.RenderTarget[0].BlendEnable = TRUE;
	BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

	m_devicePtr->CreateBlendState(&BlendState, &m_blendStateWithBlendPtr);

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
	m_dContextPtr->DSSetSamplers(0, 1, m_psSamplerState.GetAddressOf());

	// SSAO Random sampler
	
	ZeroMemory(&samplerStateDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;
	samplerStateDesc.MipLODBias = 0;
	samplerStateDesc.MaxAnisotropy = 1;
	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerStateDesc.BorderColor[0] = 1.f;
	samplerStateDesc.BorderColor[1] = 1.f;
	samplerStateDesc.BorderColor[2] = 1.f;
	samplerStateDesc.BorderColor[3] = 1.f;
	hr = m_devicePtr->CreateSamplerState(&samplerStateDesc, &m_SSAOSamplerStateRAND);
	assert(SUCCEEDED(hr) && "Failed to create SamplerState");
	m_dContextPtr->PSSetSamplers(2, 1, m_SSAOSamplerStateRAND.GetAddressOf());

	// SSAO Normal & Depth Sampler

	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerStateDesc.BorderColor[0] = 0.f;
	samplerStateDesc.BorderColor[1] = 0.f;
	samplerStateDesc.BorderColor[2] = 0.f;
	samplerStateDesc.BorderColor[3] = 1e5f;

	hr = m_devicePtr->CreateSamplerState(&samplerStateDesc, &m_SSAOSamplerStateNRM);
	assert(SUCCEEDED(hr) && "Failed to create SamplerState");
	m_dContextPtr->PSSetSamplers(3, 1, m_SSAOSamplerStateNRM.GetAddressOf());


	m_perObjectConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &perObjectMVP(), 1);
	m_dContextPtr->VSSetConstantBuffers(0, 1, m_perObjectConstantBuffer.GetAddressOf());
	m_dContextPtr->HSSetConstantBuffers(0, 1, m_perObjectConstantBuffer.GetAddressOf());
	m_dContextPtr->DSSetConstantBuffers(0, 1, m_perObjectConstantBuffer.GetAddressOf());

	skyboxMVP skyboxMVP;
	m_skyboxConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &skyboxMVP, 1);
	m_dContextPtr->VSSetConstantBuffers(1, 1, m_skyboxConstantBuffer.GetAddressOf());
	skeletonAnimationCBuffer skeletonAnimationCBuffer;
	m_skelAnimationConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &skeletonAnimationCBuffer, 1);
	m_dContextPtr->VSSetConstantBuffers(2, 1, m_skelAnimationConstantBuffer.GetAddressOf());

	globalConstBuffer globalConstBuffer;
	m_globalConstBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &globalConstBuffer, 1);
	m_dContextPtr->PSSetConstantBuffers(4, 1, m_globalConstBuffer.GetAddressOf());
	m_dContextPtr->DSSetConstantBuffers(1, 1, m_globalConstBuffer.GetAddressOf());

	lightBufferStruct initalLightData; //Not sure why, but it refuses to take &lightBufferStruct() as argument on line below
	m_lightBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &initalLightData, 1);
	m_dContextPtr->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());

	shadowBuffer shadowBuffer;
	m_shadowConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &shadowBuffer, 1);

	cameraBufferStruct cameraBufferStruct;
	m_cameraBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &cameraBufferStruct, 1);
	m_dContextPtr->PSSetConstantBuffers(1, 1, m_cameraBuffer.GetAddressOf());

	m_dContextPtr->PSSetConstantBuffers(2, 1, m_perObjectConstantBuffer.GetAddressOf());

	MATERIAL_CONST_BUFFER MATERIAL_CONST_BUFFER;
	m_currentMaterialConstantBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &MATERIAL_CONST_BUFFER, 1);
	m_dContextPtr->PSSetConstantBuffers(3, 1, m_currentMaterialConstantBuffer.GetAddressOf());

	atmosphericFogConstBuffer atmosphericFogConstBuffer;
	m_atmosphericFogConstBuffer.initializeBuffer(m_devicePtr.Get(), true, D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER, &atmosphericFogConstBuffer, 1);
	m_dContextPtr->PSSetConstantBuffers(5, 1, m_atmosphericFogConstBuffer.GetAddressOf());

	Engine::get().setDeviceAndContextPtrs(m_devicePtr.Get(), m_dContextPtr.Get());
	ResourceHandler::get().setDeviceAndContextPtrs(m_devicePtr.Get(), m_dContextPtr.Get(), m_deferredContext.Get());
	m_camera = Engine::get().getCameraPtr();
	m_camera->setIsPlayerCamera(true);


	/////////////////////////////////////////////////
	D3D11_DEPTH_STENCIL_DESC skyboxDSD;
	ZeroMemory(&skyboxDSD, sizeof(D3D11_DEPTH_STENCIL_DESC));
	skyboxDSD.DepthEnable = true;
	skyboxDSD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	skyboxDSD.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	m_devicePtr->CreateDepthStencilState(&skyboxDSD, &skyboxDSSPtr);
	/////////////////////////////////////////////////

	initRenderQuad();
	

	// ImGui initialization
	IMGUI_CHECKVERSION();
	ImGuiContext* imguictx = ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::SetCurrentContext(imguictx);

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(m_devicePtr.Get(), m_dContextPtr.Get());

	Particle::setupStaticDataForParticle(m_devicePtr.Get());

	//Shadows - don't forget to update resolution constant in shader(s) as well
	m_shadowMap = new ShadowMap();
	m_shadowMap->initialize((UINT)4096, (UINT)4096, m_devicePtr.Get(), Engine::get().getSkyLightDir());
	m_shadowMap->createRasterState();

	//RenderBatch/Effect
	m_states = std::make_unique<CommonStates>(this->m_devicePtr.Get());
	m_effect = std::make_unique<BasicEffect>(this->m_devicePtr.Get());
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	hr = m_devicePtr->CreateInputLayout(VertexType::InputElements,
			VertexType::InputElementCount,
			shaderByteCode, byteCodeLength,
			m_inputLayout.ReleaseAndGetAddressOf());

	m_batch = std::make_unique<PrimitiveBatch<VertexType>>(this->m_dContextPtr.Get());


	m_testCamera.initialize(m_testCamera.fovAmount, (float)m_settings.width / (float)m_settings.height, 0.01f, 1000.0f);
	m_testCamera.setPosition(Vector3(0, 100, 300));
	m_testCamera.setRotation(Vector4(0.7071068f, 0.f, 0.f, 0.7071068f));

	m_flyingCamera.initialize(m_flyingCamera.fovAmount, (float)m_settings.width / (float)m_settings.height, 0.01f, 1000.0f);
	m_flyingCamera.setPosition(Vector3(0, 100, 300));
	m_flyingCamera.setRotation(Vector4(0.7071068f, 0.f, 0.f, 0.7071068f));
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
	//sChainDesc.Windowed = false; //Windowed or fullscreen
	sChainDesc.BufferDesc.Height = m_settings.height; //Size of buffer in pixels, height
	sChainDesc.BufferDesc.Width = m_settings.width; //Size of window in pixels, width
	sChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; //DXGI Format, 8 bits for Red, green, etc
	sChainDesc.BufferDesc.RefreshRate.Numerator = 60; //IF vSync is enabled and fullscreen, this specifies the max refreshRate
	sChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	sChainDesc.SampleDesc.Quality = 0;
	sChainDesc.SampleDesc.Count = 2;
	sChainDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD; //What to do with buffer when swap occur
	sChainDesc.OutputWindow = m_window;

	UINT creationFlags = 0;
#if defined(_DEBUG)
	creationFlags |= D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;
#endif

	hr = D3D11CreateDeviceAndSwapChain(
		NULL, //Adapter, null is default
		D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, //Driver to use
		NULL, //Software Driver DLL, Use if software driver is used in the above parameter.
		creationFlags,//creationFlags,
		featureLevelArray, //Array of featurelevels
		nrOfFeatureLevels, //nr of featurelevels
		D3D11_SDK_VERSION, //SDK Version, D3D11_SDL_VERSION grabs the currently installed sdk version.
		&sChainDesc, //SwapChain Description
		m_swapChainPtr.GetAddressOf(), //Swapchain pointer
		m_devicePtr.GetAddressOf(), //Device pointer
		&m_fLevel, //Feature Level
		m_dContextPtr.GetAddressOf() //Device COntext Pointer
	);
	assert(SUCCEEDED(hr));

	hr = m_devicePtr->CreateDeferredContext(0, m_deferredContext.GetAddressOf());
	assert(SUCCEEDED(hr));

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
	depthTextureDesc.SampleDesc.Count = 2;
	depthTextureDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;

	ID3D11Texture2D* depthStencilBufferPtr;


	hr = m_devicePtr->CreateTexture2D(&depthTextureDesc, 0, &depthStencilBufferPtr);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateDepthStencilView(depthStencilBufferPtr, NULL, m_depthStencilViewPtr.GetAddressOf());
	depthStencilBufferPtr->Release();

	// Normals & Depth depthStencil
	ID3D11Texture2D* NandDDepthBufferPtr;

	depthTextureDesc.SampleDesc.Count = 1;

	hr = m_devicePtr->CreateTexture2D(&depthTextureDesc, 0, &NandDDepthBufferPtr);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateDepthStencilView(NandDDepthBufferPtr, NULL, m_NormalDepthStencilViewPtr.GetAddressOf());

	NandDDepthBufferPtr->Release();

	//Binding rendertarget and depth target to pipline
//Best practice to use an array even if only using one rendertarget
	m_dContextPtr->OMSetRenderTargets(1, m_geometryRenderTargetViewPtr.GetAddressOf(), m_depthStencilViewPtr.Get());

	//Depth stencil state desc
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS; //Function to determin if pixel should be drawn. LESS_EQUAL means if z value is less or equal then overwrite.
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL; //Stencil active with MAKS_ALL
	depthStencilDesc.DepthEnable = true; //Enable depth test

	hr = m_devicePtr->CreateDepthStencilState(&depthStencilDesc, m_depthStencilStateCompLessPtr.GetAddressOf());

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL; //Function to determin if pixel should be drawn. LESS_EQUAL means if z value is less or equal then overwrite.
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

HRESULT Renderer::buildRandomVectorTexture()
{
	HRESULT hr;
	int dimensions = 256;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = dimensions;
	texDesc.Height = dimensions;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DYNAMIC;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texDesc.MiscFlags = 0;

	m_randomColors = new XMVECTOR[dimensions * dimensions];
	for (int i = 0; i < dimensions; i++)
	{
		for(int j = 0; j < dimensions; j++)
		{
			m_randomColors[i * dimensions + j] = XMVectorSet(randomFloat(), randomFloat(), randomFloat(), randomFloat());
			//std::cout << Vector4(m_randomColors[i * 16 + j]).x << ", " << Vector4(m_randomColors[i * 16 + j]).y << ", " << Vector4(m_randomColors[i * 16 + j]).z << "\n";
		}
	}

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = m_randomColors;
	subData.SysMemPitch = texDesc.Width * sizeof(XMVECTOR);
	//subData.SysMemSlicePitch = texDesc.Width * texDesc.Height * sizeof(float) * 4;

	hr = m_devicePtr->CreateTexture2D(&texDesc, &subData, &m_randomTexture);
	if (!SUCCEEDED(hr)) return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	hr = m_devicePtr->CreateShaderResourceView(m_randomTexture.Get(), &srvDesc, &m_randomVectorsSRV);
	if (!SUCCEEDED(hr)) return hr;
}

void Renderer::buildOffsetVectors()
{
	// 8 cube corners
	m_offsetVectors[0] = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
	m_offsetVectors[1] = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

	m_offsetVectors[2] = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
	m_offsetVectors[3] = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

	m_offsetVectors[4] = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
	m_offsetVectors[5] = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

	m_offsetVectors[6] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
	m_offsetVectors[7] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6 centers of cube faces
	m_offsetVectors[8] = XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	m_offsetVectors[9] = XMFLOAT4(+1.0f, 0.0f, 0.0f, 0.0f);

	m_offsetVectors[10] = XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_offsetVectors[11] = XMFLOAT4(0.0f, +1.0f, 0.0f, 0.0f);

	m_offsetVectors[12] = XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	m_offsetVectors[13] = XMFLOAT4(0.0f, 0.0f, +1.0f, 0.0f);

	
	for (int i = 0; i < 14; ++i) {
		float s = randomFloat(0.25f, 1.0f);
		m_offsetVectors[i].Normalize();
		Vector4 v = s * m_offsetVectors[i];
		m_offsetVectors[i] = v;
	}
}

void Renderer::buildFrustumFarCorners(float fovY, float farZ)
{
	float aspect = (float)(m_settings.width) / (float)(m_settings.height);

	float halfHeight = farZ * tanf(0.5f * fovY);
	//float halfHeight = farZ * tanf(fovY);
	float halfWidth = aspect * halfHeight;

	m_frustumFarCorners[0] = Vector4(-halfWidth, -halfHeight, farZ, 0);
	m_frustumFarCorners[1] = Vector4(-halfWidth, +halfHeight, farZ, 0);
	m_frustumFarCorners[2] = Vector4(+halfWidth, +halfHeight, farZ, 0);
	m_frustumFarCorners[3] = Vector4(+halfWidth, -halfHeight, farZ, 0);
}

void Renderer::computeSSAOPass()
{
	UINT offset = 0;
	int vertexCount = 6;

	m_dContextPtr->RSSetViewports(1, &m_SSAOViewport); 

	m_dContextPtr->OMSetRenderTargets(1, &nullRtv, NULL);


	m_dContextPtr->PSSetShaderResources(0, 1, &nullSrv);
	m_dContextPtr->PSSetShaderResources(1, 1, &nullSrv);
	m_dContextPtr->PSSetShaderResources(8, 1, &nullSrv);

	/*Matrix T = Matrix::Identity;
	T._11 = 0.5f;
	T._22 = -0.5f;
	T._41 = 0.5f;
	T._42 = 0.5f;*/
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	Matrix P = m_camera->getProjectionMatrix();
	Matrix pt = P * T;

	m_ssaoData.viewToTexSpace = XMMatrixTranspose(pt);

	for(UINT i = 0; i < 4; i++)
	{
		m_ssaoData.frustumCorners[i] = m_frustumFarCorners[i];
	}

	for(UINT i = 0; i < 14; i++)
	{
		m_ssaoData.offsetVectors[i] = m_offsetVectors[i];
	}

	m_ssaoBuffer.updateBuffer(m_dContextPtr.Get(), &m_ssaoData);

	m_dContextPtr->IASetVertexBuffers(0, 1, m_renderQuadBuffer.GetAddressOf(), m_renderQuadBuffer.getStridePointer(), &offset);

	m_dContextPtr->OMSetRenderTargets(1, m_SSAORenderTargetViewPtr.GetAddressOf(), NULL);
	m_dContextPtr->ClearRenderTargetView(m_SSAORenderTargetViewPtr.Get(), m_blackClearColor);

	m_compiledShaders[ShaderProgramsEnum::SSAO_MAP]->setShaders();
	m_currentSetShaderProg = ShaderProgramsEnum::SSAO_MAP;

	m_dContextPtr->VSSetConstantBuffers(4, 1, m_ssaoBuffer.GetAddressOf());
	m_dContextPtr->PSSetConstantBuffers(4, 1, m_ssaoBuffer.GetAddressOf());
	m_dContextPtr->PSSetShaderResources(0, 1, m_normalsNDepthSRV.GetAddressOf());
	m_dContextPtr->PSSetShaderResources(1, 1, m_randomVectorsSRV.GetAddressOf());

	m_dContextPtr->Draw(vertexCount, 0);

	m_dContextPtr->RSSetViewports(1, &m_defaultViewport); //Set default viewport

	m_dContextPtr->OMSetRenderTargets(1, &nullRtv, NULL);

	/*m_dContextPtr->PSSetShaderResources(0, 1, &nullSrv);
	m_dContextPtr->PSSetShaderResources(1, 1, &nullSrv);*/

}

HRESULT Renderer::initializeBloomFilter()
{
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	ID3D11Texture2D* downSampledTexture;
	ID3D11Texture2D* blurTexture;
	LPCWSTR downSampleShaderName = L"DownSampleShader.hlsl";
	LPCWSTR blurShaderName = L"BlurShader.hlsl";

	HRESULT hr;

	calculateBlurWeights(m_blurData);

	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	textureDesc.Width = m_settings.width / 2;
	textureDesc.Height = m_settings.height / 2;
	textureDesc.SampleDesc.Count = 1;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &downSampledTexture);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateTexture2D(&textureDesc, NULL, &blurTexture);
	if (!SUCCEEDED(hr)) return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	hr = m_devicePtr->CreateShaderResourceView(downSampledTexture, &srvDesc, &m_downSampledShaderResourceView);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateShaderResourceView(blurTexture, &srvDesc, &m_blurShaderResourceView);
	if (!SUCCEEDED(hr)) return hr;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};

	uavDesc.Format = textureDesc.Format;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION::D3D11_UAV_DIMENSION_TEXTURE2D;
	uavDesc.Texture2D.MipSlice = 0;

	hr = m_devicePtr->CreateUnorderedAccessView(downSampledTexture, &uavDesc, &m_downSampledUnorderedAccessView);
	if (!SUCCEEDED(hr)) return hr;

	hr = m_devicePtr->CreateUnorderedAccessView(blurTexture, &uavDesc, &m_blurUnorderedAccessView);
	if (!SUCCEEDED(hr)) return hr;

	downSampledTexture->Release();
	blurTexture->Release();

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	hr = D3DCompileFromFile(
		downSampleShaderName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"cs_5_0",
		flags,
		0,
		Blob.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (Blob) { Blob->Release(); }
		assert(false);
	}

	hr = m_devicePtr->CreateComputeShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, m_CSDownSample.GetAddressOf());
	assert(SUCCEEDED(hr));

	hr = D3DCompileFromFile(
		blurShaderName,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"cs_5_0",
		flags,
		0,
		Blob.GetAddressOf(),
		errorBlob.GetAddressOf());

	if (FAILED(hr)) {
		if (errorBlob) {
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		if (Blob) { Blob->Release(); }
		assert(false);
	}

	hr = m_devicePtr->CreateComputeShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, m_CSBlurr.GetAddressOf());
	assert(SUCCEEDED(hr));
	return hr;
}

void Renderer::calculateBlurWeights(CS_BLUR_CBUFFER& bufferData, int radius, float sigma)
{
	bufferData.direction = 0;
	bufferData.radius = radius;

	// Blur Kernal Weights Calculator
	float sum = 0.f;
	float newSum = 0.f;
	float twoSigmaSq = 2 * sigma * sigma; // pow(sigma, 2.f)

	for (size_t i = 0; i <= bufferData.radius; ++i)
	{
		float temp = (1.f / sigma) * std::expf(-static_cast<float>(i * i) / twoSigmaSq);
		bufferData.weights[i] = temp;
		sum += 2 * temp; // Add 2 times to sum because we only compute half of the curve(1 dimension)
	}
	sum -= bufferData.weights[0];

	for (int i = 0; i <= bufferData.radius; ++i)
		bufferData.weights[i] /= sum;
}

void Renderer::downSamplePass()
{
	m_dContextPtr->OMSetRenderTargets(1, &nullRtv, NULL);

	m_dContextPtr->VSSetShader(NULL, NULL, 0);
	m_dContextPtr->GSSetShader(NULL, NULL, 0);

	m_dContextPtr->PSSetShader(NULL, NULL, 0);

	m_dContextPtr->CSSetShader(m_CSDownSample.Get(), 0, 0);
	m_dContextPtr->CSSetShaderResources(0, 1, m_glowMapShaderResourceView.GetAddressOf());
	m_dContextPtr->CSSetUnorderedAccessViews(0, 1, m_downSampledUnorderedAccessView.GetAddressOf(), 0);
	m_dContextPtr->Dispatch(m_settings.width / 16, m_settings.height / 16, 1);

	m_dContextPtr->CSSetShaderResources(0, 1, &nullSrv);
	m_dContextPtr->CSSetUnorderedAccessViews(0, 1, &nullUav, 0);
}

void Renderer::blurPass()
{
	UINT offset = 0;
	int vertexCount = 6;
	

	m_dContextPtr->PSSetShaderResources(0, 1, &nullSrv);
	m_dContextPtr->PSSetShaderResources(1, 1, &nullSrv);
	m_dContextPtr->CSSetShader(m_CSBlurr.Get(), 0, 0);

	ID3D11ShaderResourceView* blurSRVs[] = { m_downSampledShaderResourceView.Get(), m_blurShaderResourceView.Get() };

	ID3D11UnorderedAccessView* blurUAVs[] = { m_blurUnorderedAccessView.Get(), m_downSampledUnorderedAccessView.Get() };
	UINT cOffset = -1;
	
	for (UINT i = 0; i < 2; i++)
	{
		// Blur Constant Buffer
		this->m_blurData.direction = i;
		this->m_blurBuffer.updateBuffer(m_dContextPtr.Get(), &m_blurData);
		this->m_dContextPtr->CSSetConstantBuffers(0, 1, m_blurBuffer.GetAddressOf());

		// Set Rescources
		this->m_dContextPtr->CSSetShaderResources(0, 1, &blurSRVs[this->m_blurData.direction]);
		this->m_dContextPtr->CSSetUnorderedAccessViews(0, 1, &blurUAVs[this->m_blurData.direction], &cOffset);

		// Dispatch Shader
		this->m_dContextPtr->Dispatch(m_settings.width / 16, m_settings.height / 16, 1);

		// Unbind Unordered Access View and Shader Resource View
		this->m_dContextPtr->CSSetShaderResources(0, 1, &this->nullSrv);
		this->m_dContextPtr->CSSetUnorderedAccessViews(0, 1, &this->nullUav, &cOffset);

	}

	m_dContextPtr->IASetVertexBuffers(0, 1, m_renderQuadBuffer.GetAddressOf(), m_renderQuadBuffer.getStridePointer(), &offset);
	m_compiledShaders[ShaderProgramsEnum::BLOOM_COMBINE]->setShaders();
	m_currentSetShaderProg = ShaderProgramsEnum::BLOOM_COMBINE;
	m_dContextPtr->OMSetRenderTargets(1, m_finalRenderTargetViewPtr.GetAddressOf(), m_depthStencilViewPtr.Get());
	m_dContextPtr->PSSetShaderResources(0, 1, m_geometryShaderResourceView.GetAddressOf());
	m_dContextPtr->PSSetShaderResources(1, 1, m_downSampledShaderResourceView.GetAddressOf());
	m_dContextPtr->PSSetShaderResources(2, 1, m_randomVectorsSRV.GetAddressOf()); // TEMPORARY


	//int dimension = 256;
	//for (int i = 0; i < dimension; i++)
	//{
	//	for (int j = 0; j < dimension; j++)
	//	{
	//		m_randomColors[i * dimension + j] = XMVectorSet(randomFloat(), randomFloat(), randomFloat(), 1);
	//		//m_randomColors[i * dimension + j] = XMFLOAT4(1, 1, 0, 1);
	//	}
	//}

	//D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	//ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//m_dContextPtr->Map(m_randomTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	//memcpy(mappedSubResource.pData, m_randomColors, (dimension * dimension * sizeof(XMVECTOR))); // not working
	//m_dContextPtr->Unmap(m_randomTexture.Get(), 0);

	m_dContextPtr->Draw(vertexCount, 0);
	m_dContextPtr->PSSetShaderResources(0, 1, &nullSrv);
	m_dContextPtr->PSSetShaderResources(1, 1, &nullSrv);

}

void Renderer::ssaoBlurPass()
{
	UINT offset = 0;
	int vertexCount = 6;
	UINT cOffset = -1;

	m_dContextPtr->OMSetRenderTargets(1, &nullRtv, NULL);

	this->m_dContextPtr->CSSetUnorderedAccessViews(0, 1, &this->nullUav, &cOffset);

	m_dContextPtr->PSSetShaderResources(0, 1, &nullSrv);
	m_dContextPtr->PSSetShaderResources(1, 1, &nullSrv);
	m_dContextPtr->CSSetShader(m_CSBlurr.Get(), 0, 0);

	ID3D11ShaderResourceView* blurSRVs[] = { m_SSAOShaderResourceViewPtr.Get(), m_SSAOblurSrv.Get() };
	ID3D11UnorderedAccessView* blurUAVs[] = { m_SSAOblurUav.Get(), m_SSAOUnorderedAccessViewPtr.Get() };

	for (UINT i = 0; i < 2; i++)
	{
		// Blur Constant Buffer
		m_ssaoBlurData.direction = i;
		m_blurBuffer.updateBuffer(m_dContextPtr.Get(), &m_ssaoBlurData);
		this->m_dContextPtr->CSSetConstantBuffers(0, 1, m_blurBuffer.GetAddressOf());

		// Set Rescources
		this->m_dContextPtr->CSSetShaderResources(0, 1, &blurSRVs[m_ssaoBlurData.direction]);
		this->m_dContextPtr->CSSetUnorderedAccessViews(0, 1, &blurUAVs[m_ssaoBlurData.direction], &cOffset);

		// Dispatch Shader
		this->m_dContextPtr->Dispatch(m_settings.width / 8, m_settings.height / 8, 1);

		// Unbind Unordered Access View and Shader Resource View
		this->m_dContextPtr->CSSetShaderResources(0, 1, &this->nullSrv);
		this->m_dContextPtr->CSSetUnorderedAccessViews(0, 1, &this->nullUav, &cOffset);
	}

	m_dContextPtr->PSSetShaderResources(0, 1, &nullSrv);
	m_dContextPtr->PSSetShaderResources(1, 1, &nullSrv);

	//ImGui::Begin("someWindow");
	//ImGui::Image(m_SSAOShaderResourceViewPtr.Get(), ImVec2(512, 288));
	//ImGui::Image(m_normalsNDepthSRV.Get(), ImVec2(1024, 576));
	//ImGui::Image(m_randomVectorsSRV.Get(), ImVec2(256, 256));
	//ImGui::Image(m_randomVectorsSRV.Get(), ImVec2(128, 128));
	//ImGui::End();
}

void Renderer::normalsNDepthPass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P)
{
	m_drawn = 0;

	m_dContextPtr->OMSetRenderTargets(1, m_normalsNDepthRenderTargetViewPtr.GetAddressOf(), m_NormalDepthStencilViewPtr.Get());
	m_dContextPtr->ClearRenderTargetView(m_normalsNDepthRenderTargetViewPtr.Get(), m_AOclearColor);
	// Get Entity map from Engine
	std::unordered_map<std::string, Entity*>* entityMap = Engine::get().getEntityMap();

	for (auto& component : *Engine::get().getShadowPassDrawCallsPtr())
	{
		bool isAnim = false;
		XMFLOAT3 min, max;
		bool draw = true;
		Entity* parentEntity;

		if (component->getParentEntityIdentifier() == PLAYER_ENTITY_NAME)
			parentEntity = Engine::get().getPlayerPtr()->getPlayerEntity();
		else if (component->getParentEntityIdentifier() == (const std::string) "3DMarker")
			parentEntity = Engine::get().getPlayerPtr()->get3DMarkerEntity();
		else
			parentEntity = (*entityMap)[component->getParentEntityIdentifier()];


		if (m_camera->frustumCullingOn && parentEntity->m_canCull)
		{
			//Culling
			Vector3 scale = component->getScaling() * parentEntity->getScaling();
			XMVECTOR pos = parentEntity->getTranslation();
			pos += component->getTranslation();
			pos += component->getMeshResourcePtr()->getBoundsCenter() * scale;
			XMFLOAT3 posFloat3;
			XMStoreFloat3(&posFloat3, pos);


			if (frust->Contains(pos) != ContainmentType::CONTAINS)
			{
				component->getMeshResourcePtr()->getMinMax(min, max);

				XMFLOAT3 ext = (max - min) / 2;
				ext = ext * scale;
				XMFLOAT4 rot = parentEntity->getRotation() * component->getRotation();
				BoundingOrientedBox box(posFloat3, ext, rot);

				ContainmentType contType = frust->Contains(box);
				draw = (contType == ContainmentType::INTERSECTS || contType == ContainmentType::CONTAINS);
			}
			else
			{
				draw = true;
			}
		}

		MeshComponent* meshComp = dynamic_cast<MeshComponent*>(component);
		if (draw && meshComp->isVisible())
		{
			m_drawn++;

			perObjectMVP constantBufferPerObjectStruct;
			component->getMeshResourcePtr()->set(m_dContextPtr.Get());
			constantBufferPerObjectStruct.projection = XMMatrixTranspose(m_camera->getProjectionMatrix());
			constantBufferPerObjectStruct.view = XMMatrixTranspose(m_camera->getViewMatrix());
			constantBufferPerObjectStruct.world = XMMatrixTranspose((parentEntity->calculateWorldMatrix() * component->calculateWorldMatrix()));
			constantBufferPerObjectStruct.mvpMatrix = constantBufferPerObjectStruct.projection * constantBufferPerObjectStruct.view * constantBufferPerObjectStruct.world;
			m_perObjectConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferPerObjectStruct);

			XMMATRIX w = parentEntity->calculateWorldMatrix() * component->calculateWorldMatrix();
			w.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

			XMVECTOR det = XMMatrixDeterminant(w);
			w = XMMatrixTranspose(XMMatrixInverse(&det, w));

			m_ssaoData.worldInverseTransposeView = XMMatrixTranspose(w * m_camera->getViewMatrix());

			m_ssaoBuffer.updateBuffer(m_dContextPtr.Get(), &m_ssaoData);

			AnimatedMeshComponent* animMeshComponent = dynamic_cast<AnimatedMeshComponent*>(component);
			ShaderProgramsEnum NormalShaderEnum = ShaderProgramsEnum::NORMALS_DEPTH;

			if (animMeshComponent != nullptr) // ? does this need to be optimised or is it fine to do this for every mesh?
			{
				m_skelAnimationConstantBuffer.updateBuffer(m_dContextPtr.Get(), animMeshComponent->getAllAnimationTransforms());
				NormalShaderEnum = ShaderProgramsEnum::NORMALS_DEPTH_ANIM;
			}

			int materialCount = component->getMeshResourcePtr()->getMaterialCount();

			for (int mat = 0; mat < materialCount; mat++)
			{
				ShaderProgramsEnum meshShaderEnum = component->getShaderProgEnum(mat);
				if (meshShaderEnum != ShaderProgramsEnum::SKYBOX)
				{
					if (m_currentSetShaderProg != NormalShaderEnum)
					{
						m_compiledShaders[NormalShaderEnum]->setShaders();
						m_currentSetShaderProg = NormalShaderEnum;
					}

					Material* meshMatPtr = component->getMaterialPtr(mat);
					if (m_currentSetMaterialId != meshMatPtr->getMaterialId())
					{
						meshMatPtr->setMaterial(m_compiledShaders[meshShaderEnum], m_dContextPtr.Get());
						m_currentSetMaterialId = meshMatPtr->getMaterialId();

						MATERIAL_CONST_BUFFER currentMaterialConstantBufferData;
						currentMaterialConstantBufferData.textured = meshMatPtr->getMaterialParameters().textured;

						m_currentMaterialConstantBuffer.updateBuffer(m_dContextPtr.Get(), &currentMaterialConstantBufferData);
					}

					std::pair<std::uint32_t, std::uint32_t> offsetAndSize = component->getMeshResourcePtr()->getMaterialOffsetAndSize(mat);

					m_dContextPtr->DrawIndexed(offsetAndSize.second, offsetAndSize.first, 0);
				}
			}
		}
	}
}

void Renderer::initRenderQuad()
{
	std::vector<TextureNormalVertex> fullScreenQuad;
	
	fullScreenQuad.push_back({ XMFLOAT3(-1.f, -1.f, 0.f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.f, 1.f) });
	fullScreenQuad.push_back({ XMFLOAT3(-1.f, 1.f, 0.f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(0.f, 0.f) });
	fullScreenQuad.push_back({ XMFLOAT3(1.f, 1.f, 0.f), XMFLOAT3(2.0f, 0.0f, 0.0f), XMFLOAT2(1.f, 0.f) });
	fullScreenQuad.push_back({ XMFLOAT3(-1.f, -1.f, 0.f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.f, 1.f) });
	fullScreenQuad.push_back({ XMFLOAT3(1.f, 1.f, 0.f), XMFLOAT3(2.0f, 0.0f, 0.0f), XMFLOAT2(1.f, 0.f) });
	fullScreenQuad.push_back({ XMFLOAT3(1.f, -1.f, 0.f), XMFLOAT3(3.0f, 0.0f, 0.0f), XMFLOAT2(1.f, 1.f) });

	m_renderQuadBuffer.initializeBuffer(m_devicePtr.Get(), false, D3D11_BIND_VERTEX_BUFFER, fullScreenQuad.data(), (int)fullScreenQuad.size());

}

void Renderer::renderDrawCall(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, DrawCallStruct* drawCallStruct, const bool& useFrustumCullingParam)
{



	// >>>>>>>>>>>>>>>>>>>>> Step 1: Get entity & meshComp to use
	MeshComponent* meshComponent = drawCallStruct->mesh;
	Entity* parentEntity;
	std::unordered_map<std::string, Entity*>* entityMap = Engine::get().getEntityMap();

	if (meshComponent->getParentEntityIdentifier() == PLAYER_ENTITY_NAME)
		parentEntity = Engine::get().getPlayerPtr()->getPlayerEntity();
	else if (meshComponent->getParentEntityIdentifier() == (const std::string) "3DMarker")
		parentEntity = Engine::get().getPlayerPtr()->get3DMarkerEntity();
	else
		parentEntity = (*Engine::get().getEntityMap())[meshComponent->getParentEntityIdentifier()];


	// >>>>>>>>>>>>>>>>>>>>> Step 2: Cull all meshes not seen
	bool meshIsInsideFrustrum = true;
	if (useFrustumCullingParam && m_camera->frustumCullingOn && parentEntity->m_canCull && USE_FRUSTUM_CULLING)
	{
		meshIsInsideFrustrum = preformCullOnMeshComponent(frust, meshComponent, parentEntity);
	}

	// >>>>>>>>>>>>>>>>>>>>> Step 3: Finalize mesh for drawing
	if (meshIsInsideFrustrum && meshComponent->isVisible())
	{
		// ----------------- Step 3.1: Set constant buffers
		perObjectMVP constantBufferPerObjectStruct;
		meshComponent->getMeshResourcePtr()->set(m_dContextPtr.Get());
		constantBufferPerObjectStruct.projection = XMMatrixTranspose(m_switchCamera ? m_testCamera.getProjectionMatrix() : m_camera->getProjectionMatrix());
		constantBufferPerObjectStruct.view = XMMatrixTranspose(m_switchCamera ? m_testCamera.getViewMatrix() : m_camera->getViewMatrix());
		constantBufferPerObjectStruct.world = XMMatrixTranspose(XMMatrixMultiply(meshComponent->calculateWorldMatrix(), parentEntity->calculateWorldMatrix()));
		constantBufferPerObjectStruct.mvpMatrix = constantBufferPerObjectStruct.projection * constantBufferPerObjectStruct.view * constantBufferPerObjectStruct.world;
		m_perObjectConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferPerObjectStruct);

		// ----------------- Step 3.2: Check if the mesh has animations
		AnimatedMeshComponent* animMeshComponent = dynamic_cast<AnimatedMeshComponent*>(meshComponent);
		if (animMeshComponent != nullptr)
		{
			m_skelAnimationConstantBuffer.updateBuffer(m_dContextPtr.Get(), animMeshComponent->getAllAnimationTransforms());
		}

		if (m_currentSetShaderProg != ShaderProgramsEnum(meshComponent->getShaderProgEnum(drawCallStruct->material_IDX)))
		{
			m_currentSetShaderProg = ShaderProgramsEnum(meshComponent->getShaderProgEnum(drawCallStruct->material_IDX));
			m_compiledShaders[m_currentSetShaderProg]->setShaders();
		}
		

		// ----------------- Step 3.3: Set material constant buffers
		int material_ID = drawCallStruct->material_ID;
		int material_IDX = drawCallStruct->material_IDX;
		if (material_ID != m_currentSetMaterialId)
		{
			Material* meshMaterial = meshComponent->getMaterialPtr(material_IDX);
			meshMaterial->setMaterial(m_compiledShaders[m_currentSetShaderProg], m_dContextPtr.Get());
			m_currentSetMaterialId = material_ID;

			MATERIAL_CONST_BUFFER currentMaterialConstantBufferData;
			currentMaterialConstantBufferData.UVScale = meshMaterial->getMaterialParameters().UVScale;
			currentMaterialConstantBufferData.roughness = meshMaterial->getMaterialParameters().roughness;
			currentMaterialConstantBufferData.metallic = meshMaterial->getMaterialParameters().metallic;
			currentMaterialConstantBufferData.textured = meshMaterial->getMaterialParameters().textured;
			currentMaterialConstantBufferData.emissiveStrength = meshMaterial->getMaterialParameters().emissiveStrength;
			m_currentMaterialConstantBuffer.updateBuffer(m_dContextPtr.Get(), &currentMaterialConstantBufferData);
		}

		// ----------------- Step 3.4:
		m_dContextPtr->PSSetShaderResources(7, 1, m_shadowMap->m_depthMapSRV.GetAddressOf());
		m_dContextPtr->PSSetShaderResources(8, 1, m_SSAOShaderResourceViewPtr.GetAddressOf());
		std::pair<std::uint32_t, std::uint32_t> offsetAndSize = meshComponent->getMeshResourcePtr()->getMaterialOffsetAndSize(material_IDX);
		m_drawn++;
		m_dContextPtr->DrawIndexed(offsetAndSize.second, offsetAndSize.first, 0);
	}
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_dContextPtr->PSSetShaderResources(7, 1, nullSRV);
	
}

void Renderer::zPrePassRenderMeshComponent(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, MeshComponent* meshComponent, const bool& useFrustumCullingParam)
{
	bool draw = true;
	bool isAnim = false;
	// Get Entity map from Engine
	std::unordered_map<std::string, Entity*>* entityMap = Engine::get().getEntityMap();
	MeshComponent* meshComp = dynamic_cast<MeshComponent*>(meshComponent);
	XMFLOAT3 min, max;
	Entity* parentEntity;

	if (meshComponent->getParentEntityIdentifier() == PLAYER_ENTITY_NAME)
		parentEntity = Engine::get().getPlayerPtr()->getPlayerEntity();
	else if (meshComponent->getParentEntityIdentifier() == (const std::string) "3DMarker")
		parentEntity = Engine::get().getPlayerPtr()->get3DMarkerEntity();
	else
		parentEntity = (*entityMap)[meshComponent->getParentEntityIdentifier()];


	if (m_camera->frustumCullingOn && parentEntity->m_canCull && USE_FRUSTUM_CULLING && useFrustumCullingParam)
	{
		draw = preformCullOnMeshComponent(frust, meshComponent, parentEntity);
	}

	if (draw && meshComp->isVisible())
	{

		perObjectMVP constantBufferPerObjectStruct;
		meshComponent->getMeshResourcePtr()->set(m_dContextPtr.Get());
		constantBufferPerObjectStruct.projection = XMMatrixTranspose(m_switchCamera ? m_testCamera.getProjectionMatrix() : m_camera->getProjectionMatrix());
		constantBufferPerObjectStruct.view = XMMatrixTranspose(m_switchCamera ? m_testCamera.getViewMatrix() : m_camera->getViewMatrix());
		constantBufferPerObjectStruct.world = XMMatrixTranspose(XMMatrixMultiply(meshComponent->calculateWorldMatrix(), parentEntity->calculateWorldMatrix()));
		constantBufferPerObjectStruct.mvpMatrix = constantBufferPerObjectStruct.projection * constantBufferPerObjectStruct.view * constantBufferPerObjectStruct.world;

		m_perObjectConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferPerObjectStruct);

		AnimatedMeshComponent* animMeshComponent = nullptr;
		if (meshComponent->getType() == ComponentType::ANIM_MESH)
			animMeshComponent = static_cast<AnimatedMeshComponent*>(meshComponent);


		if (animMeshComponent != nullptr) // ? does this need to be optimised or is it fine to do this for every mesh?
		{
			m_skelAnimationConstantBuffer.updateBuffer(m_dContextPtr.Get(), animMeshComponent->getAllAnimationTransforms());
			isAnim = true;
		}

		int materialCount = meshComponent->getMeshResourcePtr()->getMaterialCount();

		for (int mat = 0; mat < materialCount; mat++)
		{
			ShaderProgramsEnum meshShaderEnum = isAnim ? ShaderProgramsEnum::Z_PRE_PASS_ANIM : ShaderProgramsEnum::Z_PRE_PASS;
			ShaderProgramsEnum shaderProgEnum = meshComponent->getShaderProgEnum(mat);
			if (shaderProgEnum == ShaderProgramsEnum::SKYBOX || shaderProgEnum == ShaderProgramsEnum::CLOUD || shaderProgEnum == ShaderProgramsEnum::LUCY_FACE || shaderProgEnum == ShaderProgramsEnum::RAINBOW)
				continue;

			if (m_currentSetShaderProg != meshShaderEnum)
			{
				m_compiledShaders[meshShaderEnum]->setShaders();
				m_currentSetShaderProg = meshShaderEnum;
			}

			Material* meshMatPtr = meshComponent->getMaterialPtr(mat);
			if (m_currentSetMaterialId != meshMatPtr->getMaterialId())
			{
				meshMatPtr->setMaterial(m_compiledShaders[meshShaderEnum], m_dContextPtr.Get());
				m_currentSetMaterialId = meshMatPtr->getMaterialId();

				std::pair<std::uint32_t, std::uint32_t> offsetAndSize = meshComponent->getMeshResourcePtr()->getMaterialOffsetAndSize(mat);

				m_dContextPtr->DrawIndexed(offsetAndSize.second, offsetAndSize.first, 0);

			}
		}
	}
}

void Renderer::zPrePass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, std::vector<MeshComponent*> &meshComponentsFromQuadTree)
{

	//for (auto& component : *Engine::get().getMeshComponentMap())
	//{
	//	this->zPrePassRenderMeshComponent(frust, wvp, V, P, component.second, true);
	//}

	for (auto& meshComponent : *Engine::get().getShadowPassDrawCallsPtr())
	{

		this->zPrePassRenderMeshComponent(frust, wvp, V, P, meshComponent, true);
		
	}

	for (auto& meshComponent : meshComponentsFromQuadTree)
	{
		this->zPrePassRenderMeshComponent(frust, wvp, V, P, meshComponent, false);
	}

}

void Renderer::renderMeshComponent(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, MeshComponent* meshComponent, const bool& useFrustumCullingParam)
{
	// Get Entity map from Engine
	std::unordered_map<std::string, Entity*>* entityMap = Engine::get().getEntityMap();
	XMFLOAT3 min, max;
	bool draw = true;
	Entity* parentEntity;

	if (meshComponent->getParentEntityIdentifier() == PLAYER_ENTITY_NAME)
		parentEntity = Engine::get().getPlayerPtr()->getPlayerEntity();
	else if (meshComponent->getParentEntityIdentifier() == (const std::string) "3DMarker")
		parentEntity = Engine::get().getPlayerPtr()->get3DMarkerEntity();
	else
		parentEntity = (*entityMap)[meshComponent->getParentEntityIdentifier()];



	if (m_camera->frustumCullingOn && parentEntity->m_canCull && USE_FRUSTUM_CULLING && useFrustumCullingParam)
	{
		draw = preformCullOnMeshComponent(frust, meshComponent, parentEntity);
	}

	MeshComponent* meshComp = dynamic_cast<MeshComponent*>(meshComponent);
	if (draw && meshComp->isVisible())
	{
		perObjectMVP constantBufferPerObjectStruct;
		meshComponent->getMeshResourcePtr()->set(m_dContextPtr.Get());
		constantBufferPerObjectStruct.projection = XMMatrixTranspose(m_switchCamera ? m_testCamera.getProjectionMatrix() : m_camera->getProjectionMatrix());
		constantBufferPerObjectStruct.view = XMMatrixTranspose(m_switchCamera ? m_testCamera.getViewMatrix() : m_camera->getViewMatrix());
		constantBufferPerObjectStruct.world = XMMatrixTranspose(XMMatrixMultiply(meshComponent->calculateWorldMatrix(), parentEntity->calculateWorldMatrix()));
		constantBufferPerObjectStruct.mvpMatrix = constantBufferPerObjectStruct.projection * constantBufferPerObjectStruct.view * constantBufferPerObjectStruct.world;

		m_perObjectConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferPerObjectStruct);

		AnimatedMeshComponent* animMeshComponent = dynamic_cast<AnimatedMeshComponent*>(meshComponent);

		if (animMeshComponent != nullptr) // ? does this need to be optimised or is it fine to do this for every mesh?
		{
			m_skelAnimationConstantBuffer.updateBuffer(m_dContextPtr.Get(), animMeshComponent->getAllAnimationTransforms());
		}

		int materialCount = meshComponent->getMeshResourcePtr()->getMaterialCount();

		for (int mat = 0; mat < materialCount; mat++)
		{
			ShaderProgramsEnum meshShaderEnum = meshComponent->getShaderProgEnum(mat);
			if (m_currentSetShaderProg != meshShaderEnum)
			{
				m_compiledShaders[meshShaderEnum]->setShaders();
				m_currentSetShaderProg = meshShaderEnum;
			}

			Material* meshMatPtr = meshComponent->getMaterialPtr(mat);
			if (m_currentSetMaterialId != meshMatPtr->getMaterialId())
			{
				meshMatPtr->setMaterial(m_compiledShaders[meshShaderEnum], m_dContextPtr.Get());
				m_currentSetMaterialId = meshMatPtr->getMaterialId();

				if (m_currentSetShaderProg != meshShaderEnum)
				{
					m_compiledShaders[meshShaderEnum]->setShaders();
					m_currentSetShaderProg = meshShaderEnum;
				}
				
				Material* meshMatPtr = meshComponent->getMaterialPtr(mat);
				if (m_currentSetMaterialId != meshMatPtr->getMaterialId())
				{
					meshMatPtr->setMaterial(m_compiledShaders[meshShaderEnum], m_dContextPtr.Get());
					m_currentSetMaterialId = meshMatPtr->getMaterialId();
					
					MATERIAL_CONST_BUFFER currentMaterialConstantBufferData;
					currentMaterialConstantBufferData.UVScale = meshMatPtr->getMaterialParameters().UVScale;
					currentMaterialConstantBufferData.roughness = meshMatPtr->getMaterialParameters().roughness;
					currentMaterialConstantBufferData.metallic = meshMatPtr->getMaterialParameters().metallic;
					currentMaterialConstantBufferData.textured = meshMatPtr->getMaterialParameters().textured;
					currentMaterialConstantBufferData.emissiveStrength = meshMatPtr->getMaterialParameters().emissiveStrength;

					m_currentMaterialConstantBuffer.updateBuffer(m_dContextPtr.Get(), &currentMaterialConstantBufferData);
				}
				m_dContextPtr->PSSetShaderResources(7, 1, m_shadowMap->m_depthMapSRV.GetAddressOf());
				m_dContextPtr->PSSetShaderResources(8, 1, m_SSAOShaderResourceViewPtr.GetAddressOf());

				std::pair<std::uint32_t, std::uint32_t> offsetAndSize = meshComponent->getMeshResourcePtr()->getMaterialOffsetAndSize(mat);
				
				m_dContextPtr->DrawIndexed(offsetAndSize.second, offsetAndSize.first, 0);
			}
			m_dContextPtr->PSSetShaderResources(7, 1, m_shadowMap->m_depthMapSRV.GetAddressOf());

			std::pair<std::uint32_t, std::uint32_t> offsetAndSize = meshComponent->getMeshResourcePtr()->getMaterialOffsetAndSize(mat);

			m_dContextPtr->DrawIndexed(offsetAndSize.second, offsetAndSize.first, 0);
		}
			m_dContextPtr->PSSetShaderResources(7, 1, m_shadowMap->m_depthMapSRV.GetAddressOf());
	}
}

void Renderer::renderScene(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, std::vector<MeshComponent*> &meshComponentsFromQuadTree)
{
	m_drawn = 0;

	for (auto& meshComponent : *Engine::get().getShadowPassDrawCallsPtr())
	{

		this->renderMeshComponent(frust, wvp, V, P, meshComponent, true);

	}

	for (auto& meshComponent : meshComponentsFromQuadTree)
	{
		renderMeshComponent(frust, wvp, V, P, meshComponent, false);
	}
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_dContextPtr->PSSetShaderResources(7, 1, nullSRV);
}

void Renderer::renderSceneWithExperimentalSorting(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P)
{
	m_drawn = 0;


	for (auto& drawCall : m_drawCallVector)
	{
		renderDrawCall(frust, wvp, V, P, drawCall.second, false);
		delete drawCall.second;
	}
		
	m_drawCallVector.clear();
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_dContextPtr->PSSetShaderResources(7, 1, nullSRV);
}

void Renderer::renderShadowPass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P)
{
	std::vector<MeshComponent*>* shadowPassDrawCalls = Engine::get().getShadowPassDrawCallsPtr();
	
	ID3D11ShaderResourceView* emptySRV[1] = { nullptr };
	m_dContextPtr->PSSetShaderResources(2, 1, emptySRV);

	//Shadow
	m_shadowMap->bindResourcesAndSetNullRTV(m_dContextPtr.Get());
	m_shadowMap->computeShadowMatrix(m_camera->getPosition());

	shadowBuffer shadowBufferStruct;
	shadowBufferStruct.lightProjMatrix = XMMatrixTranspose(m_shadowMap->m_lightProjMatrix);
	shadowBufferStruct.lightViewMatrix = XMMatrixTranspose(m_shadowMap->m_lightViewMatrix);
	shadowBufferStruct.shadowMatrix = XMMatrixTranspose(m_shadowMap->m_shadowTransform);
	m_shadowConstantBuffer.updateBuffer(m_dContextPtr.Get(), &shadowBufferStruct);

	//for (auto& drawCallVector : *Engine::get().getDrawCallsPtr())
	{
		//for (auto& drawCallStruct : drawCallVector)
		for (int i = 0; i < shadowPassDrawCalls->size(); i++)
		{
			//MeshComponent* component = drawCallStruct.mesh;
 			MeshComponent* component = shadowPassDrawCalls->at(i);

			if (component->getShaderProgEnum(0) != ShaderProgramsEnum::SKEL_PBR && component->getShaderProgEnum(0) != ShaderProgramsEnum::LUCY_FACE && component->getShaderProgEnum(0) != ShaderProgramsEnum::SKEL_ANIM)
			{
				ShaderProgramsEnum meshShaderEnum = ShaderProgramsEnum::SHADOW_DEPTH;
				m_compiledShaders[meshShaderEnum]->setShaders();
				m_currentSetShaderProg = meshShaderEnum;
			}
			else
			{
				ShaderProgramsEnum meshShaderEnum = ShaderProgramsEnum::SHADOW_DEPTH_ANIM;
				m_compiledShaders[meshShaderEnum]->setShaders();
				m_currentSetShaderProg = meshShaderEnum;
			}

			// Get Entity map from Engine
			std::unordered_map<std::string, Entity*>* entityMap = Engine::get().getEntityMap();
			
			Entity* parentEntity;
			parentEntity = (*entityMap)[component->getParentEntityIdentifier()];

			//MeshComponent* comp = dynamic_cast<MeshComponent*>(component);
			if (component->castsShadow())
			{
				perObjectMVP constantBufferPerObjectStruct;
				component->getMeshResourcePtr()->set(m_dContextPtr.Get());
				constantBufferPerObjectStruct.projection = XMMatrixTranspose(m_shadowMap->m_lightProjMatrix);//XMMatrixTranspose(m_camera->getProjectionMatrix());
				constantBufferPerObjectStruct.view = XMMatrixTranspose(m_shadowMap->m_lightViewMatrix);//XMMatrixTranspose(m_camera->getViewMatrix());
				constantBufferPerObjectStruct.world = XMMatrixTranspose(XMMatrixMultiply(component->calculateWorldMatrix(), parentEntity->calculateWorldMatrix()));
				constantBufferPerObjectStruct.mvpMatrix = constantBufferPerObjectStruct.projection * constantBufferPerObjectStruct.view * constantBufferPerObjectStruct.world;
				m_perObjectConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferPerObjectStruct);

				if (component->getType() == ComponentType::ANIM_MESH)
				{
					AnimatedMeshComponent* animMeshComponent = dynamic_cast<AnimatedMeshComponent*>(component);
					assert(animMeshComponent);
					m_skelAnimationConstantBuffer.updateBuffer(m_dContextPtr.Get(), animMeshComponent->getAllAnimationTransforms());// ? does this need to be optimised or is it fine to do this for every mesh?
				}

				//std::pair<std::uint32_t, std::uint32_t> offsetAndSize = component->getMeshResourcePtr()->getMaterialOffsetAndSize(drawCallStruct.material_IDX);

				//m_dContextPtr->DrawIndexed(offsetAndSize.second, offsetAndSize.first, 0);
				m_dContextPtr->DrawIndexed(component->getMeshResourcePtr()->getIndexBuffer().getSize(), 0, 0);
				//m_dContextPtr->DrawIndexed(component->getMeshResourcePtr()->getIndexBuffer().getSize(), 0, 0);
			}
		}
	}
}

void Renderer::resizeBackbuff(int x, int y)
{
	//m_swapChainPtr.
	
	/*ID3D11Texture2D* swapChainBufferPtr;
	HRESULT hr = m_swapChainPtr->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&swapChainBufferPtr);
	int succInt = swapChainBufferPtr->Release();

	m_dContextPtr->ClearState();
	HRESULT succ = m_swapChainPtr->ResizeBuffers(0, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 0);
	assert(SUCCEEDED(succ));*/
	//m_swapChainPtr->SetFullscreenState(TRUE, nullptr);
}

void Renderer::rasterizerSetup()
{
	HRESULT hr = 0;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;

	hr = m_devicePtr->CreateRasterizerState(&rasterizerDesc, m_rasterizerStatePtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error creating rasterizerState");



	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

	hr = m_devicePtr->CreateRasterizerState(&rasterizerDesc, m_particleRasterizerStatePtr.GetAddressOf());
	assert(SUCCEEDED(hr) && "Error creating particleRasterizerState");
}

void Renderer::update(const float& dt)
{
	if (DEBUGMODE)
	{
		if (ImGui::Button("Toggle FrustumCulling"))
		{
			m_camera->frustumCullingOn = !m_camera->frustumCullingOn;
		}

		if (ImGui::Button("Switch Camera"))
		{
			m_switchCamera = !m_switchCamera;
		}

		if (ImGui::Button("Flying camera"))
		{
			m_useFlyingCamera = !m_useFlyingCamera;
			Engine::get().getPlayerPtr()->m_ignoreInput = m_useFlyingCamera;

			m_camera = &m_flyingCamera;
			m_camera->frustumCullingOn = false;
			m_camera->setPosition(Engine::get().getPlayerPtr()->getPlayerEntity()->getTranslation());
		}
	}

	if (m_useFlyingCamera)
	{
		
		m_camera->update(dt);
		m_camera->setProjectionMatrix(m_camera->fovAmount, (float)m_settings.width / (float)m_settings.height, 0.01f, 1000.0f);
		
		
	}
	else
	{
		m_camera = Engine::get().getCameraPtr();

	}


	// Constant buffer updates and settings
	static globalConstBuffer globalConstBufferTemp;
	static atmosphericFogConstBuffer fogConstBufferTemp;
	static lightBufferStruct lightBufferTemp;

	static float tempSunDirectionX = 0.0f;
	static float tempSunDirectionY = -1.0f;
	static float tempSunDirectionZ = 1.0f;

	if (DEBUGMODE)
	{
		ImGui::SetNextWindowPos(ImVec2(1300.0f, 200.0f));
		ImGui::SetNextWindowSize(ImVec2(550.0f, 500.0f));
		ImGui::Begin("Global Shader Settings");

		ImGui::Text("");
		ImGui::Text("Directional Light");
		ImGui::SliderFloat("Intensity", (float*)&lightBufferTemp.skyLight.brightness, 0.0f, 100.0f);
		ImGui::SliderFloat("Sun direction X", (float*)&tempSunDirectionX, -1.0f, 1.0f);
		ImGui::SliderFloat("Sun direction Y", (float*)&tempSunDirectionY, -1.0f, 1.0f);
		ImGui::SliderFloat("Sun direction Z", (float*)&tempSunDirectionZ, -1.0f, 1.0f);
		ImGui::ColorEdit3("Color:", (float*)&lightBufferTemp.skyLight.color);

		ImGui::Text("");
		ImGui::Text("Atmospheric Fog Settings");
		ImGui::ColorEdit3("Color", (float*)&fogConstBufferTemp.FogColor);
		ImGui::SliderFloat("Start depth", (float*)&fogConstBufferTemp.FogStartDepth, 0.0f, 100.0f);
		ImGui::SliderFloat("Start depth (Skybox)", (float*)&fogConstBufferTemp.FogStartDepthSkybox, 0.0f, 100.0f);
		ImGui::ColorEdit3("Highlight color", (float*)&fogConstBufferTemp.FogHighlightColor);
		ImGui::SliderFloat("Global Density", (float*)&fogConstBufferTemp.FogGlobalDensity, 0.0f, 0.5f);
		ImGui::SliderFloat("Height falloff", (float*)&fogConstBufferTemp.FogHeightFalloff, 0.0f, 100.0f);
		ImGui::Text("Cloud Fog");
		ImGui::SliderFloat("Start", (float*)&fogConstBufferTemp.cloudFogHeightStart, -100.0f, 100.0f);
		ImGui::SliderFloat("End", (float*)&fogConstBufferTemp.cloudFogHeightEnd, -100.0f, 100.0f);
		ImGui::SliderFloat("Strength", (float*)&fogConstBufferTemp.cloudFogStrength, 0.0f, 1.0f);
		ImGui::ColorEdit3("Cloud Fog Color", (float*)&fogConstBufferTemp.cloudFogColor);

		ImGui::End();
	}


	// Misc updates
	lightBufferTemp.skyLight.direction = { tempSunDirectionX, tempSunDirectionY, tempSunDirectionZ };
	fogConstBufferTemp.FogSunDir = lightBufferTemp.skyLight.direction;
	globalConstBufferTemp.time += dt;

	m_globalConstBuffer.updateBuffer(m_dContextPtr.Get(), &globalConstBufferTemp);
	m_atmosphericFogConstBuffer.updateBuffer(m_dContextPtr.Get(), &fogConstBufferTemp);
	m_lightBuffer.updateBuffer(m_dContextPtr.Get(), &lightBufferTemp);

	Engine::get().setSkyLightDir({ tempSunDirectionX, tempSunDirectionY, tempSunDirectionZ, 0 });
}

void Renderer::setPipelineShaders(ID3D11VertexShader* vsPtr, ID3D11HullShader* hsPtr, ID3D11DomainShader* dsPtr, ID3D11GeometryShader* gsPtr, ID3D11PixelShader* psPtr)
{
	this->m_dContextPtr->VSSetShader(vsPtr, nullptr, 0);
	this->m_dContextPtr->HSSetShader(hsPtr, nullptr, 0);
	this->m_dContextPtr->DSSetShader(dsPtr, nullptr, 0);
	this->m_dContextPtr->GSSetShader(gsPtr, nullptr, 0);
	this->m_dContextPtr->PSSetShader(psPtr, nullptr, 0);
}

void Renderer::render()
{



	//m_deferredContext->ExecuteCommandList();
	m_dContextPtr->ExecuteCommandList(ResourceHandler::get().m_commandList, TRUE);

	//Update camera position for pixel shader buffer
	cameraBufferStruct cameraStruct;
	//if (!m_switchCamera)
		cameraStruct = cameraBufferStruct{ m_camera->getPosition() };
	//else
		//cameraStruct = cameraBufferStruct{ m_testCamera.getPosition() };

	m_cameraBuffer.updateBuffer(m_dContextPtr.Get(), &cameraStruct);

	m_dContextPtr->OMSetBlendState(m_blendStateNoBlendPtr, m_blendFactor, m_sampleMask);

	m_dContextPtr->ClearRenderTargetView(m_geometryRenderTargetViewPtr.Get(), m_clearColor);
	m_dContextPtr->ClearRenderTargetView(m_finalRenderTargetViewPtr.Get(), m_clearColor);
	m_dContextPtr->ClearRenderTargetView(m_glowMapRenderTargetViewPtr.Get(), m_blackClearColor);
	m_dContextPtr->ClearRenderTargetView(m_normalsNDepthRenderTargetViewPtr.Get(), m_clearColor);

	if (m_depthStencilViewPtr)
	{
		m_dContextPtr->ClearDepthStencilView(m_depthStencilViewPtr.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
		m_dContextPtr->ClearDepthStencilView(m_NormalDepthStencilViewPtr.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	//NormalPass
	m_dContextPtr->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT offset = 0;

	m_dContextPtr->RSSetViewports(1, &m_defaultViewport); //Set default viewport
	//m_rTargetViewsArray[0] = m_finalRenderTargetViewPtr.Get();
	m_dContextPtr->RSSetState(m_rasterizerStatePtr.Get());

	
	

	//End of particles
	// Skybox constant buffer:
	m_dContextPtr->OMSetDepthStencilState(skyboxDSSPtr, 0);
	skyboxMVP constantBufferSkyboxStruct;
	XMMATRIX W;
	XMMATRIX V;

	XMMATRIX P;
	if (!m_switchCamera)
	{
		W = XMMatrixTranslation(XMVectorGetX(m_camera->getPosition()), XMVectorGetY(m_camera->getPosition()), XMVectorGetZ(m_camera->getPosition()));
		P = m_camera->getProjectionMatrix();
		V = m_camera->getViewMatrix();
	}
	else
	{
		W = XMMatrixTranslation(XMVectorGetX(m_testCamera.getPosition()), XMVectorGetY(m_testCamera.getPosition()), XMVectorGetZ(m_testCamera.getPosition()));
		P = m_testCamera.getProjectionMatrix();
		V = m_testCamera.getViewMatrix();
	}
	constantBufferSkyboxStruct.mvpMatrix = XMMatrixTranspose(W * V * P);
	m_skyboxConstantBuffer.updateBuffer(m_dContextPtr.Get(), &constantBufferSkyboxStruct);


	// Mesh WVP buffer, needs to be set every frame bacause of SpriteBatch(GUIHandler)
	m_dContextPtr->VSSetConstantBuffers(0, 1, m_perObjectConstantBuffer.GetAddressOf());


	//Calculate the frumstum required
	BoundingFrustum frust = m_camera->getFrustum();
	frust.Origin = Vector3(m_camera->getPosition());
	frust.Orientation = m_camera->getFloat4Rotation();

	
	QuadTree* quadTree = Engine::get().getQuadTreePtr();
	std::unordered_map<std::string, Entity*>* entityMapPtr = Engine::get().getEntityMap();
//	std::vector<DrawCallStruct*> drawCalls;
	std::vector<MeshComponent*> meshCompVec;
	if (quadTree)
	{
		quadTree->getRenderList(&frust, meshCompVec, m_boundingVolumes);
		if (USE_EXPERIMENTAL_SORTING)
		{
			for (int i = 0; i < (int)meshCompVec.size(); i++)
			{
				MeshComponent* mComp = meshCompVec.at(i);
				Entity* entity = entityMapPtr->at(mComp->getParentEntityIdentifier());
				int nrOfMaterials = mComp->getMeshResourcePtr()->getMaterialCount();
				for (int j = 0; j < nrOfMaterials; j++)
				{
					DrawCallStruct* dStruct = new DrawCallStruct(mComp, j, entity);
					Vector3 meshPos = entity->getTranslation();
					Vector3 dist = meshPos - Vector3(m_camera->getPosition());
					unsigned int val = 0;
					drawInt dInt;
					dInt.dist = dist.Length();
					dInt.id = (unsigned int)mComp->getShaderProgEnum(j);
					dInt.translucency = (dInt.id == (int)ShaderProgramsEnum::LUCY_FACE);
					m_drawCallVector.push_back(std::make_pair((UINT16)(dInt.translucency << 15 | dInt.dist << 4 | dInt.id), dStruct));
				}
				
			}

			for (auto mComp : *Engine::get().getShadowPassDrawCallsPtr())
			{
				Entity* entity = entityMapPtr->at(mComp->getParentEntityIdentifier());
				bool draw = true;
				if (entity->m_canCull)
				{
					draw = preformCullOnMeshComponent(&frust, mComp, entity);
				}
				if (!draw)
					continue;

				int nrOfMaterials = mComp->getMeshResourcePtr()->getMaterialCount();
				for (int j = 0; j < nrOfMaterials; j++)
				{
					DrawCallStruct* dStruct = new DrawCallStruct(mComp, j, entity);
					Vector3 meshPos = entity->getTranslation();
					Vector3 dist = meshPos - Vector3(m_camera->getPosition());
					unsigned int val = 0;
					drawInt dInt;
					dInt.dist = dist.Length() + j;
					dInt.id = (unsigned int)mComp->getShaderProgEnum(j);
					dInt.translucency = (dInt.id == (int)ShaderProgramsEnum::LUCY_FACE);
					m_drawCallVector.push_back(std::make_pair((UINT16)(dInt.translucency << 15 | dInt.dist << 4 | dInt.id), dStruct));
				}
			}

			std::sort(m_drawCallVector.begin(), m_drawCallVector.end());
		}
		
	}





	//Run the shadow pass before everything else
	m_dContextPtr->VSSetConstantBuffers(3, 1, m_shadowConstantBuffer.GetAddressOf());
	
	m_shadowMap->setLightDir(Engine::get().getSkyLightDir());
	renderShadowPass(&frust, &W, &V, &P);
	
	
	//Set to null, otherwise we get error saying it's still bound to input.
	ID3D11RenderTargetView* nullRenderTargets[1] = { 0 };
	m_dContextPtr->OMSetRenderTargets(1, nullRenderTargets, nullptr);

	//Run ZPreePass
	m_dContextPtr->RSSetState(m_rasterizerStatePtr.Get());
	m_dContextPtr->RSSetViewports(1, &m_defaultViewport); //Set default viewport
	m_dContextPtr->PSSetSamplers(0, 1, m_psSamplerState.GetAddressOf());


	if(USE_Z_PRE_PASS)
	{
		this->m_dContextPtr->OMSetDepthStencilState(m_depthStencilStateCompLessPtr.Get(), NULL);
		ID3D11RenderTargetView* nullRenderTargetsTwo[2] = { 0 };
		m_dContextPtr->OMSetRenderTargets(2, nullRenderTargetsTwo, m_depthStencilViewPtr.Get());
		this->zPrePass(&frust, &W, &V, &P, meshCompVec);
	}


	// Normals & Depth pass
	normalsNDepthPass(&frust, &W, &V, &P);

	// SSAO
	computeSSAOPass();

	// SSAO Blur
	ssaoBlurPass();

	//Run ordinary pass
	this->m_dContextPtr->OMSetDepthStencilState(m_depthStencilStatePtr.Get(), NULL);
	m_dContextPtr->OMSetRenderTargets(2, m_geometryPassRTVs, m_depthStencilViewPtr.Get());






	m_dContextPtr->OMSetBlendState(m_blendStateWithBlendPtr, m_blendFactor, m_sampleMask);
	if (!USE_EXPERIMENTAL_SORTING)
	USE_QUADTREE ? renderScene(&frust, &W, &V, &P, meshCompVec) : renderSortedScene(&frust, &W, &V, &P); //If we use quadTree we use render scene otherwise we use sorted scene.
	else
		renderSceneWithExperimentalSorting(&frust, &W, &V, &P);
	//renderScene(&frust, &wvp, &V, &P);
	m_dContextPtr->OMSetBlendState(m_blendStateNoBlendPtr, m_blendFactor, m_sampleMask);


	ID3D11ShaderResourceView* srv[1] = { 0 };
	m_dContextPtr->PSSetShaderResources(0, 1, srv);


	//Particles
	m_dContextPtr->RSSetState(m_particleRasterizerStatePtr.Get());
	this->setPipelineShaders(nullptr, nullptr, nullptr, nullptr, nullptr);
	//Draw all particles here
	//this->particle.draw(this->m_dContextPtr.Get());

	for (auto& entity : *Engine::get().getEntityMap())
	{
		std::vector<Component*> vec;
		entity.second->getComponentsOfType(vec, ComponentType::PARTICLE);

		for (int i = 0; i < vec.size(); i++)
		{
			static_cast<ParticleComponent*>(vec[i])->draw(m_dContextPtr.Get());
		}
	}
	m_dContextPtr->RSSetState(m_rasterizerStatePtr.Get());
	this->setPipelineShaders(nullptr, nullptr, nullptr, nullptr, nullptr);
	this->m_dContextPtr->OMSetDepthStencilState(this->m_depthStencilStatePtr.Get(), 0);

	if (DEBUGMODE)
	{
		ImGui::Begin("DrawCall", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
		ImGui::Text("Nr of draw calls per frame: %d .", (int)m_drawn);
		ImGui::End();
	}

	// Bloom Filter
	downSamplePass();
	blurPass();

	drawBoundingVolumes();

	// GUI
	GUIHandler::get().render();

	// Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


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

ID3D11DeviceContext* Renderer::getDeferredDContext()
{
	return m_deferredContext.Get();
}

ID3D11DepthStencilView* Renderer::getDepthStencilView()
{
	return m_depthStencilViewPtr.Get();
}

void Renderer::printLiveObject()
{
#if defined( DEBUG ) || defined( _DEBUG )
	HRESULT hr = m_debugPtr->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	assert(SUCCEEDED(hr));
#endif
}

/*
void Renderer::initializeDrawCallList()
{
	drawCalls.clear();
	//drawCalls.resize(NR_OF_SHADER_PROGRAM_ENUMS);
	//for (auto& component : *Engine::get().getMeshComponentMap()) // Loop over all meshes in the scene
	//{
	//	MeshComponent* meshComp = dynamic_cast<MeshComponent*>(component.second);
	//	for (int i = 0; i < meshComp->getMeshResourcePtr()->getMaterialCount(); i++) // Loop over all materials on the mesh
	//	{
	//		drawCalls.at(meshComp->getShaderProgEnum(i)).push_back(
	//			drawCallStruct(meshComp,													// mesh
	//						   meshComp->getShaderProgEnum(i),								// shaderEnu
	//						   meshComp->getMaterialPtr(i)->getMaterialId(),				// matID
	//						   i,															// matIDX
	//						   meshComp->getFilePath()));
	//	}
	//}
	//sortDrawCallList();
	//int stop = 0;
}
*/
