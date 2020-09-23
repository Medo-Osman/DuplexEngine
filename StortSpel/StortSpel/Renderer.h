#pragma once
#include "VertexStructs.h"
#include "ConstantBufferTypes.h"
#include "Camera.h"
#include "Layouts.h"
#include "CompileShaderUtility.h"
#include "Buffer.h"
#include "Entity.h"
#include "Player.h"
class MeshComponent;

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

	//Input Assembler
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_vertexLayoutPtr = NULL;

	//Vertex Shader
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShaderPtr = NULL;
	Microsoft::WRL::ComPtr<ID3DBlob> m_vertexShaderBufferPtr = NULL;
	//Buffer<cbVSWVPMatrix> m_vertexShaderConstantBuffer;
	Buffer<perObjectMVP> m_perObjectConstantBuffer;

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
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerStatePtr = NULL;
	D3D11_VIEWPORT m_defaultViewport;

	//Pixel Shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShaderPtr = NULL;
	Microsoft::WRL::ComPtr<ID3DBlob> m_pixelBufferPtr = NULL;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_psSamplerState = NULL;


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
	
	unsigned int long m_MeshCount = 0;

	//Functions
	HRESULT createDeviceAndSwapChain();
	HRESULT createDepthStencil();
	HRESULT setUpInputAssembler();
	void createAndSetShaders();
	void rasterizerSetup();

	void createViewPort(D3D11_VIEWPORT& viewPort, const int& width, const int& height) const;
	void setPipelineShaders(ID3D11VertexShader* vsPtr, ID3D11HullShader* hsPtr, ID3D11DomainShader* dsPtr, ID3D11GeometryShader* gsPtr, ID3D11PixelShader* psPtr);
	Renderer(); //{};

	std::map<unsigned int long, MeshComponent*> meshComponentMap;


public:
	Renderer(const Renderer&) = delete;
	void operator=(Renderer const&) = delete;

	static Renderer& get()
	{
		static Renderer instance;
		return instance;
	}

	void addMeshComponent(MeshComponent* component);

	HRESULT initialize(const HWND& window);
	void release();


	void handleInput(Mouse* mousePtr, Keyboard* keyboardPtr, const float& dt); //Move to game when any sort of gameLayer exists
	void update(const float& dt);
	void render();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDContext();

};