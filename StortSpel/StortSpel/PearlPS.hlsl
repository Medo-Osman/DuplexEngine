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

cbuffer MaterialBuffer : register(b3)
{
	float materialUVScale;
	float materialRoughness;
	float materialMetallic;
	int materialTextured;
	float materialEmissiveStrength;
	float3 materialBaseColor;
}

cbuffer globalConstBuffer : register(b4)
{
	float3 playerPosition;
	float environmentMapBrightness;
	float time;
}

cbuffer atmosphericFogConstantBuffer : register(b5)
{
	float3 FogColor;
	float FogStartDepth;
	float3 FogHighlightColor;
	float FogGlobalDensity;
	float3 FogSunDir;
	float FogHeightFalloff;
	float FogStartDepthSkybox;
	float cloudFogHeightStart;
	float cloudFogHeightEnd;
	float cloudFogStrength;
	float3 cloudFogColor;
}

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}