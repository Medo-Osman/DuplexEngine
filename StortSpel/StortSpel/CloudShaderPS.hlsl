#include "UtilityLibrary.hlsli"

#define MAX_LIGHTS 8

struct pointLight
{
	float4 position;
	float3 color;
	float intensity;
};

struct spotLight
{
	float4 position;
	float3 color;
	float intensity;
	float3 direction;
	float coneFactor;
};

struct directionalLight
{
	float4 direction;
	float4 color;
	float brightness;
};

cbuffer lightBuffer : register(b0)
{
	float1 ambientLightLevel;
	pointLight pointLights[8];
	int nrOfPointLights;
    
	spotLight spotLights[8];
	int nrOfSpotLights;
    
	directionalLight skyLight;
}

cbuffer cameraBuffer : register(b1)
{
	float4 cameraPosition;
}

cbuffer perModel : register(b2)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 wvpMatrix;
};

cbuffer globalConstantBuffer : register(b4)
{
	float3 playerPosition;
	float environmentMapBrightness;
	float time;
}

struct ps_in
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITION;
	float4 shadowPos : SPOS;
};

TextureCube skyIR : register(t0);
TextureCube skyPrefilter : register(t1);
Texture2D brdfLUT : register(t2);

Texture2D displacement1 : register(t3);
Texture2D normal1 : register(t5);
//Texture2D normal2 : register(t6);

SamplerState sampState : SAMPLER : register(s0);

float4 main(ps_in input) : SV_TARGET
{
	float3 N = normalize(input.normal);
	
	//float3 normalFromMap = normal1.Sample(sampState, input.uv).xyz * 2 - 1;
	float3 normalFromMap = normal1.Sample(sampState, input.uv).xyz;
	N = normalize(normalFromMap);
	
	//input.tangent = normalize(input.tangent);

	//float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	//float3 B = cross(T, N);

	//float3x3 TBN = float3x3(T, B, N);
	//N = normalize(mul(normalFromMap, TBN));
	
	float3 V = normalize(cameraPosition - input.worldPos);
	
	float yMask = -N.g;
	
	float3 albedo = float3(0.9f, 0.9f, 0.9f);
	float3 mixColor = float3(0.33f, 0.83f, 0.79f);
	
	float3 skyLightDirection = normalize(float3(1.0f, 0.0f, 0.0f));
	float skyLightDot = dot(-skyLightDirection, N);
  
	float3 irradiance = skyIR.Sample(sampState, N).rgb;
	//float3 diffuse = irradiance * albedo;
	float3 diffuse = lerp(irradiance, albedo, 0.7f);
	
	float backSideMask = skyLightDot;
	
	float3 mixedColor = lerp(diffuse, mixColor, saturate(backSideMask));
	
	float3 color = saturate(lerp(mixedColor, -skyLightDot * skyLight.color.xyz * skyLight.brightness, 0.9f));
	
	// HDR tonemapping
	color = color / (color + float3(1.0, 1.0, 1.0));
	// Gamma correction
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
   
	backSideMask *= 4.0f;
	
	float simpleDotColor = dot(N, -skyLightDot);

	//return float4(backSideMask, backSideMask, backSideMask, 1.0);
	//return float4(backSideMask, backSideMask, backSideMask, 1.0);
	//return float4(albedo, 1.0);
	//return float4(color, 1.0);
	//return float4(input.uv, 0.0, 1.0);
	
	//float fDisplacement = normal1.SampleLevel(sampState, input.uv + (time * 0.005), 0).r;
	//float fDisplacement2 = normal2.SampleLevel(sampState, input.uv + float2(0.133564, 0.785994) + (time * 0.006), 0).r;

	//float fDisplacement2 = normal2.SampleLevel(sampState, input.uv + float2(0.133564, 0.785994), 0).r;
	
	//float finalDisplacement = normalize(fDisplacement * fDisplacement2);
	//float3 finalDisplacement = fDisplacement;
	
	float height = input.pos.g / 1000;
	float shaded = remapToRange(height, 0, 0.1, 0, 1);
	
	float3 simpleDotColorColor = AdjustContrast(float3(simpleDotColor, simpleDotColor, simpleDotColor), 2);
	
	//return float4(finalDisplacement.r, 0.0, 0.0, 1.0);
	return float4(simpleDotColorColor, 1.0);
	//return float4(skyLightDot, skyLightDot, skyLightDot, 1.0);
	
	//float3 contrastN = AdjustContrast(N, 2);
	//return float4(contrastN, 1.0f);
	
	//return float4(normalFromMap, 1.0f);
}