#pragma once
#include"3DPCH.h"
//From https://docs.microsoft.com/en-us/windows/win32/direct3d11/how-to--compile-a-shader
static const HRESULT compileShader(LPCWSTR fileName, LPCSTR entryPoint, LPCSTR shaderVer, ID3DBlob** blob)
{
	if (!fileName || !entryPoint || !shaderVer || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif


	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(fileName, //Name of file
		0, //Pointer to array of macros
		0, //Includes
		entryPoint, //Function name/Entry point
		shaderVer, //Shader version
		0, //CompileFlag 1
		0, //CompileFlag 2);
		&shaderBlob, //blob
		&errorBlob //Error message
	);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}