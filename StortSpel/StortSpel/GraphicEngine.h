#pragma once
#include"VertexStructs.h"
#include"Model.h"
#include"ConstantBufferTypes.h"
#include"Camera.h"
#include"Layouts.h"
#include"CompileShaderUtility.h"
#include"Buffer.h"

class GraphicEngine
{
private:
	//Pointers
	ID3D11RenderTargetView** m_rTargetViewsArray;
	Microsoft::WRL::ComPtr<ID3D11Device> m_devicePtr;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_dContextPtr;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChainPtr;
	Microsoft::WRL::ComPtr<ID3D11Debug> m_debugPtr;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rTargetViewPtr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_swapChainBufferPtr;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBufferPtr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilViewPtr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStatePtr;

	//Input Assembler
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_vertexLayoutPtr;
	Buffer<ColorVertex> m_vertexBuffer;


	//Vertex Shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShaderPtr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShaderBufferPtr;
	Buffer<cbVSWVPMatrix> m_vertexShaderConstantBuffer;

	//HullShader
	//Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShaderDMPtr;
	//Microsoft::WRL::ComPtr<ID3DBlob> m_hullShaderBufferPtr;

	//DomainShader
	//Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShaderDMPtr;
	//Microsoft::WRL::ComPtr<ID3DBlob> m_domainShaderBufferPtr;

	//GeometryShader
	//Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShaderPtr;
	//Microsoft::WRL::ComPtr<ID3DBlob> m_geometryShaderBufferPtr;

	//Rasterizer
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStatePtr;
	D3D11_VIEWPORT m_defaultViewport; 

	//Pixel Shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShaderPtr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pixelBufferPtr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_psSamplerState;


	//Variables Config
	static const UINT nrOfFeatureLevels = 2;
	const D3D_FEATURE_LEVEL featureLevelArray[nrOfFeatureLevels]
	{
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
	};
	static const int m_startHeight = 600;
	static const int m_startWidth = 600;

	//Variables
	D3D_FEATURE_LEVEL m_fLevel;
	HWND m_window;
	int m_width;
	int m_height;
	float m_clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.f };
	Camera m_camera;

	//Functions
	HRESULT createDeviceAndSwapChain();
	HRESULT createDepthStencil();
	HRESULT setUpInputAssembler();
	void createAndSetShaders();
	void rasterizerSetup();

	void createViewPort(D3D11_VIEWPORT& viewPort, const int& width, const int& height) const;
	void setPipelineShaders(ID3D11VertexShader* vsPtr, ID3D11HullShader* hsPtr, ID3D11DomainShader* dsPtr, ID3D11GeometryShader* gsPtr, ID3D11PixelShader* psPtr);

public:
	GraphicEngine();
	~GraphicEngine();

	HRESULT initialize(const HWND& window);
	void release();


	void handleInput(Mouse* mousePtr, Keyboard* keyboardPtr, const float& dt); //Move to game when any sort of gameLayer exists
	void update(const float &dt);
	void render();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDContext();

};