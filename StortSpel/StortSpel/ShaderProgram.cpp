#include "3DPCH.h"
#include "ShaderProgram.h"



ShaderProgram::ShaderProgram(std::initializer_list <LPCWSTR> shaderFiles, D3D_PRIMITIVE_TOPOLOGY topologyType, VertexLayoutType inputLayoutType, ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr, ID3D11DepthStencilView* depthStencilPtr)
	: m_topologyType(topologyType), m_devicePtr(devicePtr), m_dContextPtr(dContextPtr), m_depthStencilPtr(depthStencilPtr)
{
	int i = 0;
	for (auto file : shaderFiles)
	{
		m_shaderFiles[i] = file;
		i++;
	}

	for (int i = 0; i < 4; i++)
		m_shaderNeedsResource[i] = false;

	m_shaderNeedsResource[4] = true;

	for (int i = 0; i < 4; i++)
		m_shaderNeedsCBuffer[i] = false;

	m_shaderNeedsCBuffer[4] = true;

	compileShaders(inputLayoutType);
}

ShaderProgram::~ShaderProgram()
{
	m_VS = nullptr;
	m_HS = nullptr;
	m_DS = nullptr;
	m_GS = nullptr;
	m_PS = nullptr;

	m_inputLayout = nullptr;

	for (int i = 0; i < m_renderTargets.size(); i++)
	{
		m_renderTargets.at(i) = nullptr;
	}
}

void ShaderProgram::compileShaders(VertexLayoutType inputLayoutType)
{
	// Compile and create all the shaders
	// For now let each Shader object have it's own comptr even if a diffrent Shader uses the same shader-stage.
	Microsoft::WRL::ComPtr<ID3DBlob> Blob = NULL;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = NULL;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = NULL;

	if (m_shaderFiles[ShaderType::Vertex] != L"null" && m_shaderFiles[ShaderType::Vertex] != L"")
	{
		hr = D3DCompileFromFile(m_shaderFiles[ShaderType::Vertex], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			errorBlobCheck(errorBlob);

		hr = m_devicePtr->CreateVertexShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, m_VS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}

	inputLayoutSetup(inputLayoutType, Blob);

	Blob = nullptr;

	if (m_shaderFiles[ShaderType::Hull] != L"null" && m_shaderFiles[ShaderType::Hull] != L"")
	{
		hr = D3DCompileFromFile(m_shaderFiles[ShaderType::Hull], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "hs_5_0", flags, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			errorBlobCheck(errorBlob);

		hr = m_devicePtr->CreateHullShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, m_HS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	Blob = nullptr;

	if (m_shaderFiles[ShaderType::Domain] != L"null" && m_shaderFiles[ShaderType::Domain] != L"")
	{
		hr = D3DCompileFromFile(m_shaderFiles[ShaderType::Domain], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ds_5_0", flags, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			errorBlobCheck(errorBlob);

		hr = m_devicePtr->CreateDomainShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, m_DS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	Blob = nullptr;

	if (m_shaderFiles[ShaderType::Geometry] != L"null" && m_shaderFiles[ShaderType::Geometry] != L"")
	{
		hr = D3DCompileFromFile(m_shaderFiles[ShaderType::Geometry], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "gs_5_0", flags, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			errorBlobCheck(errorBlob);

		hr = m_devicePtr->CreateGeometryShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, m_GS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
	Blob = nullptr;

	if (m_shaderFiles[ShaderType::Pixel] != L"null" && m_shaderFiles[ShaderType::Pixel] != L"")
	{
		hr = D3DCompileFromFile(m_shaderFiles[ShaderType::Pixel], nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "ps_5_0", flags, 0, Blob.GetAddressOf(), errorBlob.GetAddressOf());

		if (FAILED(hr))
			errorBlobCheck(errorBlob);

		hr = m_devicePtr->CreatePixelShader(Blob->GetBufferPointer(), Blob->GetBufferSize(), NULL, m_PS.GetAddressOf());
		assert(SUCCEEDED(hr));
	}
}

void ShaderProgram::addRenderTarget(Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv)
{
	m_renderTargets.push_back(rtv);
}

void ShaderProgram::setUsesDepthStencilView(bool state)
{
	m_usesDepthStencilView = state;
}

void ShaderProgram::clearRenderTargets()
{
	float colorBlack[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	for (int i = 0; i < m_renderTargets.size(); i++)
	{
		m_dContextPtr->ClearRenderTargetView(m_renderTargets.at(i).Get(), colorBlack);
	}
}

void ShaderProgram::setShaderNeedsResource(ShaderType shader, bool state)
{
	m_shaderNeedsResource[shader] = state;
}

void ShaderProgram::setShadersNeedsResource(bool vState, bool hState, bool dState, bool gState, bool pState)
{
	m_shaderNeedsResource[ShaderType::Vertex] = vState;
	m_shaderNeedsResource[ShaderType::Hull] = hState;
	m_shaderNeedsResource[ShaderType::Domain] = dState;
	m_shaderNeedsResource[ShaderType::Geometry] = gState;
	m_shaderNeedsResource[ShaderType::Pixel] = pState;
}

bool* ShaderProgram::getShadersNeedsResource()
{
	bool* shaderNeedsResourcePtr = m_shaderNeedsResource;
	return shaderNeedsResourcePtr;
}

void ShaderProgram::setShaderNeedsCBuffer(ShaderType shader, bool state)
{
	m_shaderNeedsCBuffer[shader] = state;
}

void ShaderProgram::setShadersNeedsCBuffer(bool vState, bool hState, bool dState, bool gState, bool pState)
{
	m_shaderNeedsCBuffer[ShaderType::Vertex] = vState;
	m_shaderNeedsCBuffer[ShaderType::Hull] = hState;
	m_shaderNeedsCBuffer[ShaderType::Domain] = dState;
	m_shaderNeedsCBuffer[ShaderType::Geometry] = gState;
	m_shaderNeedsCBuffer[ShaderType::Pixel] = pState;
}

bool* ShaderProgram::getShaderNeedsCBuffer()
{
	bool* shaderNeedsCBufferPtr = m_shaderNeedsCBuffer;
	return shaderNeedsCBufferPtr;
}

void ShaderProgram::setShaders()
{

	m_dContextPtr->IASetPrimitiveTopology(m_topologyType);

	m_dContextPtr->IASetInputLayout(m_inputLayout.Get());

	if (m_renderTargets.size() > 0) // TODO: this might need a check to see if they are already set
	{
		if (m_usesDepthStencilView)
			m_dContextPtr->OMSetRenderTargets((UINT)m_renderTargets.size(), m_renderTargets[0].GetAddressOf(), m_depthStencilPtr);
		else
			m_dContextPtr->OMSetRenderTargets((UINT)m_renderTargets.size(), m_renderTargets[0].GetAddressOf(), NULL);
	}

	// ---Vertex
	if (m_shaderFiles[ShaderType::Vertex] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (m_shaderFiles[ShaderType::Vertex] == L"null")
		{
			if (!checkSetShaderFile(ShaderType::Vertex, L"null")) // If the file is null, check if rend's file is null and
				m_dContextPtr->VSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!checkSetShaderFile(ShaderType::Vertex, m_shaderFiles[ShaderType::Vertex])) //Check if shader is currently set
				m_dContextPtr->VSSetShader(m_VS.Get(), 0, 0);	// If not then set the shader
		}
	}

	// ---Hull
	if (m_shaderFiles[ShaderType::Hull] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (m_shaderFiles[ShaderType::Hull] == L"null")
		{
			if (!checkSetShaderFile(ShaderType::Hull, L"null")) // If the file is null, check if rend's file is null and
				m_dContextPtr->HSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!checkSetShaderFile(ShaderType::Hull, m_shaderFiles[ShaderType::Hull])) //Check if shader is currently set
				m_dContextPtr->HSSetShader(m_HS.Get(), 0, 0);	// If not then set the shader
		}
	}

	// ---Domain
	if (m_shaderFiles[ShaderType::Domain] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (m_shaderFiles[ShaderType::Domain] == L"null")
		{
			if (!checkSetShaderFile(ShaderType::Domain, L"null")) // If the file is null, check if rend's file is null and
				m_dContextPtr->DSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!checkSetShaderFile(ShaderType::Domain, m_shaderFiles[ShaderType::Domain])) //Check if shader is currently set
				m_dContextPtr->DSSetShader(m_DS.Get(), 0, 0);	// If not then set the shader
		}
	}

	// ---Geo
	if (m_shaderFiles[ShaderType::Geometry] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (m_shaderFiles[ShaderType::Geometry] == L"null")
		{
			if (!checkSetShaderFile(ShaderType::Geometry, L"null")) // If the file is null, check if rend's file is null and
				m_dContextPtr->GSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!checkSetShaderFile(ShaderType::Geometry, m_shaderFiles[ShaderType::Geometry])) //Check if shader is currently set
				m_dContextPtr->GSSetShader(m_GS.Get(), 0, 0);	// If not then set the shader
		}
	}
	
	// ---Pixel
	if (m_shaderFiles[ShaderType::Pixel] != L"") // Only set if the file isn't "", if it is it means it doesn't matter
	{
		if (m_shaderFiles[ShaderType::Pixel] == L"null")
		{
			if (!checkSetShaderFile(ShaderType::Pixel, L"null")) // If the file is null, check if rend's file is null and
				m_dContextPtr->PSSetShader(NULL, 0, 0);				// if not then set null
		}
		else
		{
			if (!checkSetShaderFile(ShaderType::Pixel, m_shaderFiles[ShaderType::Pixel])) //Check if shader is currently set
				m_dContextPtr->PSSetShader(m_PS.Get(), 0, 0);	// If not then set the shader
		}
	}
}

void ShaderProgram::errorBlobCheck(Microsoft::WRL::ComPtr<ID3DBlob> errorBlob)
{
	if (errorBlob.Get())
	{
		ErrorLogger::get().logError((char*)errorBlob->GetBufferPointer());
	}
	assert(false);
}

void ShaderProgram::inputLayoutSetup(VertexLayoutType inputLayoutType, Microsoft::WRL::ComPtr<ID3DBlob> VSBlob)
{
	const D3D11_INPUT_ELEMENT_DESC* input_element_desc; // This solution of making a pointer here needs to be tested
	unsigned long long layoutArraySize = 0;

	switch (inputLayoutType)
	{
	case VertexLayoutType::none:
		m_inputLayout = nullptr;
		return;
	case VertexLayoutType::vertexLayout:
		input_element_desc = Layouts::vertexLayout;
		layoutArraySize = ARRAYSIZE(Layouts::vertexLayout);
		break;
	case VertexLayoutType::colorVertexLayout:
		input_element_desc = Layouts::colorVertexLayout;
		layoutArraySize = ARRAYSIZE(Layouts::colorVertexLayout);
		break;
	case VertexLayoutType::LRMVertexLayout:
		input_element_desc = Layouts::LRMVertexLayout;
		layoutArraySize = ARRAYSIZE(Layouts::LRMVertexLayout);
		break;
	default:
		m_inputLayout = nullptr;
		return;
	}
	
	HRESULT hr = m_devicePtr->CreateInputLayout(
		input_element_desc,
		layoutArraySize,
		VSBlob->GetBufferPointer(),
		VSBlob->GetBufferSize(),
		m_inputLayout.GetAddressOf()
	);
	assert(SUCCEEDED(hr));
}

bool ShaderProgram::checkSetShaderFile(ShaderType s, LPCWSTR file)
{
	bool isSet = false;

	if (setShaderFiles[s] == file)
		isSet = true;
	else
		setShaderFiles[s] = file;

	return isSet;
}

// old inputlayout setup with if-statements

//void ShaderProgram::inputLayoutSetup(std::string input_layout_type, Microsoft::WRL::ComPtr<ID3DBlob> VSBlob)
//{
//	if (input_layout_type == "null") //These if statements could be replaced with resource handler functionality or perhaps something more dynamic.
//	{
//		m_inputLayout = nullptr;
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
//			m_inputLayout.GetAddressOf()
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
//			m_inputLayout.GetAddressOf()
//		);
//		ASSERT;
//	}
//}