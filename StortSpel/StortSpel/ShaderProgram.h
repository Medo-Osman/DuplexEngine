#pragma once
#include "VertexStructs.h"
#include "ShaderEnums.h"
#include "Renderer.h"

//#include "ResourceHandler.h"



class ShaderProgram
{
private:

	Microsoft::WRL::ComPtr<ID3D11VertexShader>	 VS = NULL;
	Microsoft::WRL::ComPtr<ID3D11HullShader>	 HS = NULL;
	Microsoft::WRL::ComPtr<ID3D11DomainShader>	 DS = NULL;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> GS = NULL;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	 PS = NULL;

	// Labels to identify which file the shaders are from, use the enum ShaderType
	LPCWSTR shaderFiles[5];

	int nrOfPSResources;

	D3D_PRIMITIVE_TOPOLOGY topologyType;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout = NULL;

	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> renderTargets;
	bool usesDepthStencilView;

	bool shaderNeedsResource[5];
	bool shaderNeedsCBuffer[5];

public:
	ShaderProgram(std::initializer_list <LPCWSTR> shaderFiles, D3D_PRIMITIVE_TOPOLOGY topologyType, std::string input_layout_type);
	~ShaderProgram();

	void compileShaders(std::string input_layout_type);

	void addRenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv);
	//void addRenderTarget(ResourceHandler& rh, const wchar_t* label);
	void setUsesDepthStencilView(bool state);
	void clearRenderTargets();

	void setShaderNeedsResource(ShaderType shader, bool state);
	void setShadersNeedsResource(bool vState, bool hState, bool dState, bool gState, bool pState);
	void setShaderNeedsCBuffer(ShaderType shader, bool state);
	void setShadersNeedsCBuffer(bool vState, bool hState, bool dState, bool gState, bool pState);

	void setShaders();

private:
	void errorBlobCheck(Microsoft::WRL::ComPtr<ID3DBlob> errorBlob);
	//void inputLayoutSetup(std::string input_layout_type, Microsoft::WRL::ComPtr<ID3DBlob> VSBlob);
};

