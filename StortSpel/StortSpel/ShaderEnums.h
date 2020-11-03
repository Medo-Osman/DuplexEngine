#pragma once
#include "ShaderProgram.h"

enum ShaderProgramsEnum
{
	DEFAULT,
	TEMP_TEST,
	SKYBOX,
	SKEL_ANIM,
	PBRTEST,
	BLOOM_COMBINE,
	OBJECTSPACEGRID,
	SHADOW,
	DEFAULT_SHADOW,
	NONE
};

inline void compileAllShaders(std::unordered_map<ShaderProgramsEnum, ShaderProgram*>* compiledShadersMap, ID3D11Device* devicePtr, ID3D11DeviceContext* dContextPtr, ID3D11DepthStencilView* depthStencilPtr)
{
	// Copy this and paste with diffrent parameters for each diffrent ShaderProgramEnum
	//
	(*compiledShadersMap)[ShaderProgramsEnum::DEFAULT] = new ShaderProgram
	(
		//{ L"VertexShaderBasic.hlsl",L"null",L"null",L"null",L"BasicPixelShader.hlsl" },
		{ L"VertexShaderBasic.hlsl",L"null",L"null",L"null",L"BasicPixelShader_Shadow.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::TEMP_TEST] = new ShaderProgram
	(
		{ L"VertexShaderBasic.hlsl",L"null",L"null",L"null",L"BasicPixelShader_temp_for_testing_shaderSwitching.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::SKYBOX] = new ShaderProgram
	(
		{ L"SkyboxVertexShader.hlsl",L"null",L"null",L"null",L"SkyboxPixelShader.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::SKEL_ANIM] = new ShaderProgram
	(
		{ L"VertexShaderAnim.hlsl",L"null",L"null",L"null",L"BasicPixelShader_temp_for_testing_shaderSwitching.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);
	
	(*compiledShadersMap)[ShaderProgramsEnum::PBRTEST] = new ShaderProgram
	(
		{ L"VertexShaderBasic.hlsl",L"null",L"null",L"null",L"PBRShaderBase.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::BLOOM_COMBINE] = new ShaderProgram
	(
		{ L"CombinePassVertex.hlsl",L"null",L"null",L"null",L"CombinePassPixel.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::OBJECTSPACEGRID] = new ShaderProgram
	(
		{ L"ObjectSpaceGridVS.hlsl",L"null",L"null",L"null",L"ObjectSpaceGridPS.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);


	(*compiledShadersMap)[ShaderProgramsEnum::SHADOW] = new ShaderProgram
	(
		{ L"ShadowVertex.hlsl",L"null",L"null",L"null",L"ShadowPixel.hlsl" }, 
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::DEFAULT_SHADOW] = new ShaderProgram
	(
		{ L"VertexShaderBasic.hlsl",L"null",L"null",L"null",L"BasicPixelShader_Shadow.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);
}