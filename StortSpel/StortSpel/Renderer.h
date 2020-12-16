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

class Renderer : public InputObserver
{

private:
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

	// SSAO stuff

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_NormalDepthStencilViewPtr = NULL;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalsNDepthSRV = NULL;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_normalsNDepthRenderTargetViewPtr = NULL;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_randomVectorsSRV = NULL;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SSAOShaderResourceViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_SSAORenderTargetViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_SSAOUnorderedAccessViewPtr = NULL;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SSAOblurSrv = NULL;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_SSAOblurUav = NULL;


	Microsoft::WRL::ComPtr <ID3D11Texture2D> m_randomTexture = NULL;
	XMVECTOR* m_randomColors;
	Vector4 m_offsetVectors[14];
	Vector4 m_frustumFarCorners[4];

	D3D11_VIEWPORT m_SSAOViewport;

	// SSAO Buffer
	SSAO_BUFFER m_ssaoData;
	Buffer<SSAO_BUFFER> m_ssaoBuffer;
	CS_BLUR_CBUFFER m_ssaoBlurData;
	float m_ssaoSigma = 5.f;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SSAOSamplerStateNRM = NULL;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_SSAOSamplerStateRAND = NULL;


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
	Buffer<TextureNormalVertex> m_renderQuadBuffer;

	CS_BLUR_CBUFFER m_blurData;
	float m_bloomSigma = 5.f;

	

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
	float m_AOclearColor[4] = { 0.0f, 0.0f, -1.0f, 1e5f };

	std::vector<std::pair<uint_fast16_t, DrawCallStruct*>> m_drawCallVector;
	
	std::unordered_map<ShaderProgramsEnum, ShaderProgram*> m_compiledShaders;
	ShaderProgramsEnum m_currentSetShaderProg = ShaderProgramsEnum::NONE;
	unsigned int long m_currentSetMaterialId = -1;

	// Blendstate
	ID3D11BlendState* m_blendStateNoBlendPtr = NULL;
	ID3D11BlendState* m_blendStateWithBlendPtr = NULL;
	const float m_blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const UINT m_sampleMask = 0xffffffff;

	//Functions
	HRESULT createDeviceAndSwapChain();
	HRESULT createDepthStencil();
	void rasterizerSetup();
	
	void createViewPort(D3D11_VIEWPORT& viewPort, const int& width, const int& height) const;

	HRESULT buildRandomVectorTexture();
	void buildOffsetVectors();
	void buildFrustumFarCorners(float fovY, float farZ);
	void computeSSAOPass();

	static float randomFloat(float a, float b)
	{
		return a + ((float)(rand()) / (float)RAND_MAX) * (b - a);
	}
	static float randomFloat()
	{
		return ((float)(rand()) / (float)RAND_MAX);
	}
	HRESULT initializeBloomFilter();
	void calculateBlurWeights(CS_BLUR_CBUFFER& bufferData, int radius = BLUR_RADIUS, float sigma = 5.f);
	void downSamplePass();
	void blurPass();
	void ssaoBlurPass();
	void normalsNDepthPass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P);
	void initRenderQuad();

	void zPrePassRenderMeshComponent(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, MeshComponent* meshComponent, const bool& useFrustumCullingParam = false);
	void zPrePass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, std::vector<MeshComponent*>& meshComponentsFromQuadTree);
	void renderDrawCall(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, DrawCallStruct* drawCallStruct, const bool& useFrustumCullingParam);
	void renderMeshComponent(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, MeshComponent* meshComponent, const bool& useFrustumCullingParam = false);
	void renderScene(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, std::vector<MeshComponent*>& meshComponentsFromQuadTree);
	void renderSceneWithExperimentalSorting(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P);
	void renderShadowPass(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P);
	void renderShadowPassByMeshComponent(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P, MeshComponent* meshComponent);
	void toggleFlyingCamera();
	Renderer(); //{};

	Camera m_testCamera;
	Camera m_flyingCamera;

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


	bool m_useFlyingCamera = false;
	bool m_switchCamera = false;

	// Sorting
	void renderSortedScene(BoundingFrustum* frust, XMMATRIX* wvp, XMMATRIX* V, XMMATRIX* P);

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


	// Inherited via InputObserver
	virtual void inputUpdate(InputData& inputData) override;

};
