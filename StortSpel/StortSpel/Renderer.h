#pragma once

#include "ConstantBufferTypes.h"
#include "ShaderProgram.h"
#include "ShaderEnums.h"
#include "Engine.h"
#include "GUIHandler.h"
#include "Scene.h"
#include "ShadowMap.h"
//#include "LightComponent.h"
#include"Particles\Particle.h"
#include"Particles\RainingDogsParticle.h"
#include"Particles\ScorePickupParticle.h"
#include"DebugDraw.h"
#include"BoundingVolumeHolder.h"

class Renderer
{

private:
	static const bool USE_Z_PRE_PASS;
	//Pointers
	//ID3D11RenderTargetView** m_rTargetViewsArray;
	Microsoft::WRL::ComPtr<ID3D11Device> m_devicePtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_dContextPtr = NULL;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChainPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11Debug> m_debugPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deferredContext = nullptr; //For multithreading

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_geometryRenderTargetViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_finalRenderTargetViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStatePtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateCompLessPtr = NULL;

	// Bloom stuff
	ID3D11RenderTargetView* m_geometryPassRTVs[2];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_glowMapRenderTargetViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_glowMapShaderResourceView = NULL;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_downSampledShaderResourceView = NULL;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_downSampledUnorderedAccessView = NULL;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_geometryShaderResourceView = NULL;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_geometryUnorderedAccessView = NULL;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_blurShaderResourceView = NULL;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_blurUnorderedAccessView = NULL;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_CSDownSample = NULL;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_CSBlurr = NULL;
	
	Microsoft::WRL::ComPtr<ID3DBlob> Blob = NULL;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = NULL;

	ID3D11RenderTargetView* nullRtv = nullptr;
	ID3D11ShaderResourceView* nullSrv = nullptr;
	ID3D11UnorderedAccessView* nullUav = nullptr;

	//Buffer<cbVSWVPMatrix> m_vertexShaderConstantBuffer;
	Buffer<perObjectMVP> m_perObjectConstantBuffer;
	Buffer<lightBufferStruct> m_lightBuffer;
	Buffer<cameraBufferStruct> m_cameraBuffer;
	Buffer<skyboxMVP> m_skyboxConstantBuffer;
	Buffer<shadowBuffer> m_shadowConstantBuffer;
	
	Buffer<skeletonAnimationCBuffer> m_skelAnimationConstantBuffer;
	Buffer<MATERIAL_CONST_BUFFER> m_currentMaterialConstantBuffer;
	Buffer<globalConstBuffer> m_globalConstBuffer;
	Buffer<atmosphericFogConstBuffer> m_atmosphericFogConstBuffer;

	// Blur stuff
	Buffer<CS_BLUR_CBUFFER> m_blurBuffer;
	Buffer<PositionTextureVertex> m_renderQuadBuffer;

	CS_BLUR_CBUFFER m_blurData;
	float m_weightSigma = 5.f;

	//Rasterizer
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStatePtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_particleRasterizerStatePtr = NULL;

	D3D11_VIEWPORT m_defaultViewport;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_psSamplerState = NULL;

	//Shadowmap
	ShadowMap* m_shadowMap;

	//Variables Config
	static const UINT nrOfFeatureLevels = 2;
	const D3D_FEATURE_LEVEL featureLevelArray[nrOfFeatureLevels]
	{
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
	};

	//Variables
	ID3D11DepthStencilState* skyboxDSSPtr;

	D3D_FEATURE_LEVEL m_fLevel;
	HWND m_window;
	Settings m_settings;
	Camera* m_camera = nullptr;
	
	float m_clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.f };
	float m_blackClearColor[4] = { 0.f, 0.f, 0.f, 1.f };

	
	
	std::unordered_map<ShaderProgramsEnum, ShaderProgram*> m_compiledShaders;
	ShaderProgramsEnum m_currentSetShaderProg = ShaderProgramsEnum::NONE;
	unsigned int long m_currentSetMaterialId = 1000;

	//Functions
	HRESULT createDeviceAndSwapChain();
	HRESULT createDepthStencil();
	void rasterizerSetup();
	
	void createViewPort(D3D11_VIEWPORT& viewPort, const int& width, const int& height) const;
	HRESULT initializeBloomFilter();
	void calculateBloomWeights();
	void downSamplePass();
	void blurPass();
	void initRenderQuad();
	void zPrePass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P);
	void renderScene(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P);
	void renderShadowPass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P);
	Renderer(); //{};

	Camera m_testCamera;

	using VertexType = DirectX::VertexPositionColor;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<VertexType>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//BoundingVolume DrawStuff
	std::vector<BoundingVolumeHolder> m_boundingVolumes;
	void drawBoundingVolumes();

	int m_drawn = 0;
	bool m_isFullscreen = false;

	bool m_switchCamera = false;
public:
	Renderer(const Renderer&) = delete;
	void operator=(Renderer const&) = delete;
	void setPointLightRenderStruct(lightBufferStruct& buffer);

	void setFullScreen(BOOL val);
	bool isFullscreen();

	void resizeBackbuff(int x, int y);
	static Renderer& get()
	{
		static Renderer instance;
		return instance;
	}
	~Renderer();

	HRESULT initialize(const HWND& window);
	void release();
	void update(const float& dt);
	void setPipelineShaders(ID3D11VertexShader* vsPtr, ID3D11HullShader* hsPtr, ID3D11DomainShader* dsPtr, ID3D11GeometryShader* gsPtr, ID3D11PixelShader* psPtr);
	void render();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDContext();
	ID3D11DeviceContext* getDeferredDContext();
	ID3D11DepthStencilView* getDepthStencilView();
	void printLiveObject();


	void addPrimitiveToDraw();

};
