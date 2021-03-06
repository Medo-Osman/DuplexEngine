#pragma once
#include "ShaderProgram.h"

enum ShaderProgramsEnum
{
	DEFAULT,
	TEMP_TEST,
	SKYBOX,
	SKEL_ANIM,
	PBRTEST,
	SKEL_PBR,
	BLOOM_COMBINE,
	OBJECTSPACEGRID,
	SHADOW_DEPTH,
	DEFAULT_SHADOW,
	SHADOW_DEPTH_ANIM,
	EMISSIVE,
	LUCY_FACE,
	RAINBOW,
	CLOUD,
	PEARL,
	Z_PRE_PASS,
	Z_PRE_PASS_ANIM,
	NORMALS_DEPTH,
	NORMALS_DEPTH_ANIM,
	SSAO_MAP,
	NONE
};

inline ShaderProgramsEnum charToShaderEnum(char e)
{
	switch (e)
	{
	case 'E':
		return EMISSIVE;
	case 'T':
		return TEMP_TEST;
	case 'S':
		return SKEL_ANIM;
	case 's':
		return SKEL_PBR;
	case 'P':
		return PBRTEST;
	case 'O':
		return OBJECTSPACEGRID;
	case 'L':
		return LUCY_FACE;
	case 'C':
		return CLOUD;
	default:
		return TEMP_TEST;
	}
}

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
		VertexLayoutType::renderQuadVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::OBJECTSPACEGRID] = new ShaderProgram
	(
		{ L"ObjectSpaceGridVS.hlsl",L"null",L"null",L"null",L"ObjectSpaceGridPS.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);


	(*compiledShadersMap)[ShaderProgramsEnum::SHADOW_DEPTH] = new ShaderProgram
	(
		{ L"ShadowVertex.hlsl",L"null",L"null",L"null",L"ShadowPixel.hlsl" }, 
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);
	
	(*compiledShadersMap)[ShaderProgramsEnum::EMISSIVE] = new ShaderProgram
	(
		{ L"VertexShaderBasic.hlsl",L"null",L"null",L"null",L"EmissivePixelShader.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::RAINBOW] = new ShaderProgram
	(
		{ L"VertexShaderBasic.hlsl",L"null",L"null",L"null",L"RainbowShader.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);
	

	(*compiledShadersMap)[ShaderProgramsEnum::SHADOW_DEPTH_ANIM] = new ShaderProgram
	(
		{ L"VertexShaderAnim.hlsl",L"null",L"null",L"null",L"ShadowPixel.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::SKEL_PBR] = new ShaderProgram
	(
		{ L"VertexShaderAnim.hlsl",L"null",L"null",L"null",L"PBRShaderBase.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::LUCY_FACE] = new ShaderProgram
	(
		{ L"VertexShaderAnim.hlsl",L"null",L"null",L"null",L"SeeThroughPixel.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::Z_PRE_PASS] = new ShaderProgram
	(
		{ L"ZPrePassVertexShaderBasic.hlsl",L"null",L"null",L"null",L"ZPrePassPixelShader.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

		(*compiledShadersMap)[ShaderProgramsEnum::Z_PRE_PASS] = new ShaderProgram
	(
		{ L"ZPrePassVertexShaderBasic.hlsl",L"null",L"null",L"null",L"ZPrePassPixelShader.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::Z_PRE_PASS_ANIM] = new ShaderProgram
	(
		{ L"ZPrePassVertexShaderAnim.hlsl",L"null",L"null",L"null",L"ZPrePassPixelShader.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);


	(*compiledShadersMap)[ShaderProgramsEnum::CLOUD] = new ShaderProgram
	(
		//{ L"CloudShaderVS.hlsl", L"CloudShaderHS.hlsl", L"CloudShaderDS.hlsl", L"null", L"CloudShaderPS.hlsl" },
		{ L"CloudShaderVS.hlsl", L"CloudShaderHS.hlsl", L"CloudShaderDS.hlsl", L"null", L"CloudShaderPS.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	// Allow binding of a displacement map to the domain shader
	(*compiledShadersMap)[ShaderProgramsEnum::CLOUD]->setShaderNeedsResource(ShaderType::Domain, true);
	(*compiledShadersMap)[ShaderProgramsEnum::CLOUD]->setShaderNeedsResource(ShaderType::Pixel, true);

	(*compiledShadersMap)[ShaderProgramsEnum::PEARL] = new ShaderProgram
	(
		{ L"PearlVS.hlsl", L"null", L"null", L"null", L"PearlPS.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	// Allow binding of a displacement map to the domain shader
	(*compiledShadersMap)[ShaderProgramsEnum::PEARL]->setShaderNeedsResource(ShaderType::Vertex, true);
	(*compiledShadersMap)[ShaderProgramsEnum::PEARL]->setShaderNeedsResource(ShaderType::Pixel, true);

	(*compiledShadersMap)[ShaderProgramsEnum::NORMALS_DEPTH] = new ShaderProgram
	(
		{ L"NormalsDepthVertex.hlsl",L"null",L"null",L"null",L"NormalsDepthPixel.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::NORMALS_DEPTH_ANIM] = new ShaderProgram
	(
		{ L"NormalsDepthVertexAnim.hlsl",L"null",L"null",L"null",L"NormalsDepthPixel.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::LRSMVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);

	(*compiledShadersMap)[ShaderProgramsEnum::SSAO_MAP] = new ShaderProgram
	(
		{ L"SSAOVertex.hlsl",L"null",L"null",L"null",L"SSAOPixel.hlsl" },
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		VertexLayoutType::renderQuadVertexLayout,
		devicePtr, dContextPtr, depthStencilPtr
	);
}