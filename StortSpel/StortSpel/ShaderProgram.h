#pragma once
#include "VertexStructs.h"
#include "Layouts.h"

//#include "ResourceHandler.h"

enum ShaderType
{
	Vertex = 0,
	Hull = 1,
	Domain = 2,
	Geometry = 3,
	Pixel = 4,
	Compute = 5
};

static LPCWSTR setShaderFiles[5];

class ShaderProgram
{
private:

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	 m_VS = NULL;
	Microsoft::WRL::ComPtr<ID3D11HullShader>	 m_HS = NULL;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>	 m_DS = NULL;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GS = NULL;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	 m_PS = NULL;

	// Labels to identify which file the shaders are from, use the enum ShaderType
	LPCWSTR m_shaderFiles[5];

	int m_nrOfPSResources;

	D3D_PRIMITIVE_TOPOLOGY m_topologyType;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout = NULL;

	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_renderTargets;
	bool m_usesDepthStencilView;

	bool m_shaderNeedsResource[5];
	bool m_shaderNeedsCBuffer[5];

	ID3D11Device* m_devicePtr;
	ID3D11DeviceContext* m_dContextPtr;
	ID3D11DepthStencilView* m_depthStencilPtr;

public:
	ShaderProgram
	(
		std::initializer_list <LPCWSTR> shaderFiles,
		D3D_PRIMITIVE_TOPOLOGY topologyType,
		VertexLayoutType inputLayoutType,
		ID3D11Device* devicePtr,
		ID3D11DeviceContext* dContextPtr,
		ID3D11DepthStencilView* depthStencilPtr
	);
	~ShaderProgram();

	void compileShaders(VertexLayoutType inputLayoutType);

	void addRenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv);
	//void addRenderTarget(ResourceHandler& rh, const wchar_t* label);
	void setUsesDepthStencilView(bool state);
	void clearRenderTargets();

	void setShaderNeedsResource(ShaderType shader, bool state);
	void setShadersNeedsResource(bool vState, bool hState, bool dState, bool gState, bool pState);
	bool* getShadersNeedsResource();
	void setShaderNeedsCBuffer(ShaderType shader, bool state);
	void setShadersNeedsCBuffer(bool vState, bool hState, bool dState, bool gState, bool pState);
	bool* getShaderNeedsCBuffer();

	void setShaders();

private:
	void errorBlobCheck(Microsoft::WRL::ComPtr<ID3DBlob> errorBlob);
	void inputLayoutSetup(VertexLayoutType inputLayoutType, Microsoft::WRL::ComPtr<ID3DBlob> VSBlob);
	bool checkSetShaderFile(ShaderType s, LPCWSTR file);
};

