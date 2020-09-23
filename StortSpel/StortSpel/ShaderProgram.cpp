#include "3DPCH.h"

#include "ShaderProgram.h"



ShaderProgram::ShaderProgram(std::initializer_list <LPCWSTR> shaderFiles, D3D_PRIMITIVE_TOPOLOGY topologyType, std::string input_layout_type)
{
	this->topologyType = topologyType;

	int i = 0;
	for (auto file : shaderFiles)
	{
		this->shaderFiles[i] = file;
		i++;
	}

	for (int i = 0; i < 4; i++)
		this->shaderNeedsResource[i] = false;

	this->shaderNeedsResource[4] = true;

	for (int i = 0; i < 4; i++)
		this->shaderNeedsCBuffer[i] = false;

	this->shaderNeedsCBuffer[4] = true;

	compileShaders(input_layout_type);
}

ShaderProgram::~ShaderProgram()
{
	VS = nullptr;
	HS = nullptr;
	DS = nullptr;
	GS = nullptr;
	PS = nullptr;

	input_layout = nullptr;

	for (int i = 0; i < renderTargets.size(); i++)
	{
		renderTargets.at(i) = nullptr;
	}
}

void ShaderProgram::compileShaders(std::string input_layout_type)
{
	// Compile and create all the shaders
	// For now let each Shader object have it's own comptr even if a diffrent Shader uses the same shader-stage. TODO: Consider adding a resource manager at a diffrent time
	Microsoft::WRL::ComPtr<ID3DBlob> Blob = NULL;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = NULL;

	HRESULT hr = NULL;

	// Make a handy pointer to GraphicsEngine for use in the function
	Renderer* rend = &Renderer::get();

	if (this->shaderFiles[ShaderType::Vertex] != L"null" && this->shaderFiles[ShaderType::Vertex] != L"")
	{
		hr = D3DCompileFromFile(this->shaderFiles[ShaderType::Vertex], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", 0, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			this->errorBlobCheck(errorBlob);

		hr = rend->getDevice()->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, this->VS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	//this->inputLayoutSetup(input_layout_type, Blob);

	Blob = nullptr;

	if (this->shaderFiles[ShaderType::Hull] != L"null" && this->shaderFiles[ShaderType::Hull] != L"")
	{
		hr = D3DCompileFromFile(this->shaderFiles[ShaderType::Hull], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "HSMain", "hs_5_0", 0, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			this->errorBlobCheck(errorBlob);

		hr = rend->getDevice()->CreateHullShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, this->HS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	Blob = nullptr;

	if (this->shaderFiles[ShaderType::Domain] != L"null" && this->shaderFiles[ShaderType::Domain] != L"")
	{
		hr = D3DCompileFromFile(this->shaderFiles[ShaderType::Domain], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "DSMain", "ds_5_0", 0, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			this->errorBlobCheck(errorBlob);

		hr = rend->getDevice()->CreateDomainShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, this->DS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	Blob = nullptr;

	if (this->shaderFiles[ShaderType::Geometry] != L"null" && this->shaderFiles[ShaderType::Geometry] != L"")
	{
		hr = D3DCompileFromFile(this->shaderFiles[ShaderType::Geometry], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "GSMain", "gs_5_0", 0, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			this->errorBlobCheck(errorBlob);

		hr = rend->getDevice()->CreateGeometryShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, this->GS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	Blob = nullptr;

	if (this->shaderFiles[ShaderType::Pixel] != L"null" && this->shaderFiles[ShaderType::Pixel] != L"")
	{
		hr = D3DCompileFromFile(this->shaderFiles[ShaderType::Pixel], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", 0, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			this->errorBlobCheck(errorBlob);

		hr = rend->getDevice()->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, this->PS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

void ShaderProgram::addRenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv)
{
	this->renderTargets.push_back(rtv);
}

void ShaderProgram::setUsesDepthStencilView(bool state)
{
	this->usesDepthStencilView = state;
}

void ShaderProgram::clearRenderTargets()
{
	float colorBlack[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	for (int i = 0; i < this->renderTargets.size(); i++)
	{
		Renderer::get().getDContext()->ClearRenderTargetView(this->renderTargets.at(i).Get(), colorBlack);
	}
}

void ShaderProgram::setShaderNeedsResource(ShaderType shader, bool state)
{
	this->shaderNeedsResource[shader] = state;
}

void ShaderProgram::setShadersNeedsResource(bool vState, bool hState, bool dState, bool gState, bool pState)
{
	this->shaderNeedsResource[ShaderType::Vertex] = vState;
	this->shaderNeedsResource[ShaderType::Hull] = hState;
	this->shaderNeedsResource[ShaderType::Domain] = dState;
	this->shaderNeedsResource[ShaderType::Geometry] = gState;
	this->shaderNeedsResource[ShaderType::Pixel] = pState;
}

void ShaderProgram::setShaderNeedsCBuffer(ShaderType shader, bool state)
{
	this->shaderNeedsCBuffer[shader] = state;
}

void ShaderProgram::setShadersNeedsCBuffer(bool vState, bool hState, bool dState, bool gState, bool pState)
{
	this->shaderNeedsCBuffer[ShaderType::Vertex] = vState;
	this->shaderNeedsCBuffer[ShaderType::Hull] = hState;
	this->shaderNeedsCBuffer[ShaderType::Domain] = dState;
	this->shaderNeedsCBuffer[ShaderType::Geometry] = gState;
	this->shaderNeedsCBuffer[ShaderType::Pixel] = pState;
}

void ShaderProgram::setShaders()
{
	// Make a handy pointer to GraphicsEngine for use in the function
	Renderer* rend = &Renderer::get();

	rend->getDContext()->IASetPrimitiveTopology(this->topologyType);

	rend->getDContext()->IASetInputLayout(this->input_layout.Get());

	if (this->renderTargets.size() > 0) // TODO: this needs a check to see if they are already set
	{
		if (this->usesDepthStencilView)
			rend->getDContext()->OMSetRenderTargets((UINT)this->renderTargets.size(), this->renderTargets[0].GetAddressOf(), rend->getDepthStencilView());
		else
			rend->getDContext()->OMSetRenderTargets((UINT)this->renderTargets.size(), this->renderTargets[0].GetAddressOf(), NULL);
	}

	// ---Vertex
	if (this->shaderFiles[ShaderType::Vertex] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (this->shaderFiles[ShaderType::Vertex] == L"null")
		{
			if (!rend->checkSetShaderFile(ShaderType::Vertex, L"null")) // If the file is null, check if rend's file is null and
				rend->getDContext()->VSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!rend->checkSetShaderFile(ShaderType::Vertex, this->shaderFiles[ShaderType::Vertex])) //Check if shader is currently set
				rend->getDContext()->VSSetShader(this->VS.Get(), 0, 0);	// If not then set the shader
		}
	}

	// ---Hull
	if (this->shaderFiles[ShaderType::Hull] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (this->shaderFiles[ShaderType::Hull] == L"null")
		{
			if (!rend->checkSetShaderFile(ShaderType::Hull, L"null")) // If the file is null, check if rend's file is null and
				rend->getDContext()->HSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!rend->checkSetShaderFile(ShaderType::Hull, this->shaderFiles[ShaderType::Hull])) //Check if shader is currently set
				rend->getDContext()->HSSetShader(this->HS.Get(), 0, 0);	// If not then set the shader
		}
	}

	// ---Domain
	if (this->shaderFiles[ShaderType::Domain] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (this->shaderFiles[ShaderType::Domain] == L"null")
		{
			if (!rend->checkSetShaderFile(ShaderType::Domain, L"null")) // If the file is null, check if rend's file is null and
				rend->getDContext()->DSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!rend->checkSetShaderFile(ShaderType::Domain, this->shaderFiles[ShaderType::Domain])) //Check if shader is currently set
				rend->getDContext()->DSSetShader(this->DS.Get(), 0, 0);	// If not then set the shader
		}
	}

	// ---Geo
	if (this->shaderFiles[ShaderType::Geometry] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (this->shaderFiles[ShaderType::Geometry] == L"null")
		{
			if (!rend->checkSetShaderFile(ShaderType::Geometry, L"null")) // If the file is null, check if rend's file is null and
				rend->getDContext()->GSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!rend->checkSetShaderFile(ShaderType::Geometry, this->shaderFiles[ShaderType::Geometry])) //Check if shader is currently set
				rend->getDContext()->GSSetShader(this->GS.Get(), 0, 0);	// If not then set the shader
		}
	}

	// ---Pixel
	if (this->shaderFiles[ShaderType::Pixel] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (this->shaderFiles[ShaderType::Pixel] == L"null")
		{
			if (!rend->checkSetShaderFile(ShaderType::Pixel, L"null")) // If the file is null, check if rend's file is null and
				rend->getDContext()->PSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!rend->checkSetShaderFile(ShaderType::Pixel, this->shaderFiles[ShaderType::Pixel])) //Check if shader is currently set
				rend->getDContext()->PSSetShader(this->PS.Get(), 0, 0);	// If not then set the shader
		}
	}
}

void ShaderProgram::errorBlobCheck(Microsoft::WRL::ComPtr<ID3DBlob> errorBlob)
{
	if (errorBlob.Get())
	{
		//ErrorLogger::logError(errorBlob->GetBufferPointer()); TODO: fix this
	}
	assert(false);
}

//void ShaderProgram::inputLayoutSetup(std::string input_layout_type, Microsoft::WRL::ComPtr<ID3DBlob> VSBlob)
//{
//	if (input_layout_type == "null") //These if statements could be replaced with resource handler functionality or perhaps something more dynamic.
//	{
//		this->input_layout = nullptr;
//	}
//	else if (input_layout_type == "pos_uv_n_col")
//	{
//		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
//		{
//			//|Sem. name| |Sem. Index|	|Format|						|Input slot|	|Aligned byte offset|								|Input slot class|				|Instance data step rate|
//			{ "POSITION",	0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				0,													D3D11_INPUT_PER_VERTEX_DATA,	0 },
//			{ "TEX_COORD",	0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				sizeof(float3),										D3D11_INPUT_PER_VERTEX_DATA,	0 },
//			{ "NORMAL",		0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				sizeof(float2) + sizeof(float3),					D3D11_INPUT_PER_VERTEX_DATA,	0 },
//			{ "COLOR",		0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				sizeof(float3) + sizeof(float2) + sizeof(float3),	D3D11_INPUT_PER_VERTEX_DATA,	0 }
//		};
//
//		HRESULT hr = GraphicsEngine::Get()->getDevice()->CreateInputLayout(
//			input_element_desc,
//			ARRAYSIZE(input_element_desc),
//			VSBlob->GetBufferPointer(),
//			VSBlob->GetBufferSize(),
//			this->input_layout.GetAddressOf()
//		);
//		ASSERT;
//	}
//	else if (input_layout_type == "ieu_layout")
//	{
//		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
//		{
//			//|Sem. name| |Sem. Index|	|Format|						|Input slot|	|Aligned byte offset|												|Input slot class|		|Instance data step rate|
//			{ "POSITION",	0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				0,																	D3D11_INPUT_PER_VERTEX_DATA,	0 },
//			{ "TEX_COORD",	0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				sizeof(float3),														D3D11_INPUT_PER_VERTEX_DATA,	0 },
//			{ "NORMAL",		0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				sizeof(float2) + sizeof(float3),									D3D11_INPUT_PER_VERTEX_DATA,	0 },
//			{ "TANGENT",	0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				sizeof(float3) + sizeof(float2) + sizeof(float3),					D3D11_INPUT_PER_VERTEX_DATA,	0 },
//			{ "BINORMAL",	0,			DXGI_FORMAT_R32G32B32_FLOAT,	0,				sizeof(float3) + sizeof(float2) + sizeof(float3) + sizeof(float3),	D3D11_INPUT_PER_VERTEX_DATA,	0 }
//		};
//
//		HRESULT hr = GraphicsEngine::Get()->getDevice()->CreateInputLayout(
//			input_element_desc,
//			ARRAYSIZE(input_element_desc),
//			VSBlob->GetBufferPointer(),
//			VSBlob->GetBufferSize(),
//			this->input_layout.GetAddressOf()
//		);
//		ASSERT;
//	}
//}