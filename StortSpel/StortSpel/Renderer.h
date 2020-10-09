#pragma once

#include "ConstantBufferTypes.h"
#include "ShaderProgram.h"
#include "ShaderEnums.h"
#include "Engine.h"
//#include "LightComponent.h"



class Renderer
{
private:
	//Pointers
	ID3D11RenderTargetView** m_rTargetViewsArray;
	Microsoft::WRL::ComPtr<ID3D11Device> m_devicePtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_dContextPtr = NULL;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChainPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11Debug> m_debugPtr = NULL;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rTargetViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_swapChainBufferPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBufferPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilViewPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStatePtr = NULL;

	

	//Buffer<cbVSWVPMatrix> m_vertexShaderConstantBuffer;
	Buffer<perObjectMVP> m_perObjectConstantBuffer;
	Buffer<lightBufferStruct> m_lightBuffer;
	Buffer<cameraBufferStruct> m_cameraBuffer;
	Buffer<skyboxMVP> m_skyboxConstantBuffer;

	//Rasterizer
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStatePtr = NULL;

	D3D11_VIEWPORT m_defaultViewport;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_psSamplerState = NULL;


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

	
	std::unordered_map<ShaderProgramsEnum, ShaderProgram*> m_compiledShaders;
	ShaderProgramsEnum m_currentSetShaderProg = ShaderProgramsEnum::NONE;
	unsigned int long m_currentSetMaterialId = 1000;

	//Functions
	HRESULT createDeviceAndSwapChain();
	HRESULT createDepthStencil();
	void rasterizerSetup();

	void createViewPort(D3D11_VIEWPORT& viewPort, const int& width, const int& height) const;
	Renderer(); //{};

public:
	Renderer(const Renderer&) = delete;
	void operator=(Renderer const&) = delete;
	void setPointLightRenderStruct(lightBufferStruct& buffer);

	static Renderer& get()
	{
		static Renderer instance;
		return instance;
	}
	~Renderer();

	HRESULT initialize(const HWND& window);
	void release();
	void update(const float& dt);
	void render();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDContext();
	ID3D11DepthStencilView* getDepthStencilView();

};
