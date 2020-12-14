#include "UtilityLibrary.hlsli"

#define MAX_LIGHTS 8

static const float SHADOW_MAP_SIZE = 4096.f;
static const float SHADOW_MAP_DELTA = 1.f / SHADOW_MAP_SIZE;
static const float RANGE = 50.f;

static const float PI = 3.14159265359;

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

cbuffer cloudConstBuffer : register(b6)
{
	float cloudBedHeightPosition;
	float cloudDisplacementFactor;
	float cloudTessellationFactor;
	float worleyScale;
	float3 panSpeed;
	float tile;
	float dispPower;
	float occlusionFactor;
	float backlightFactor;
	float backlightStrength;
	float3 backlightColor;
}

cbuffer globalConstantBuffer : register(b4)
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

float3 ApplyFog(float3 originalColor, float eyePosY, float3 eyeToPixel)
{
	float pixelDist = length(eyeToPixel);
	float3 eyeToPixelNorm = eyeToPixel / pixelDist;
	// Find the fog staring distance to pixel distance
	float fogDist = max(pixelDist - FogStartDepthSkybox, 0.0);
	// Distance based fog intensity
	float fogHeightDensityAtViewer = exp(-FogHeightFalloff * eyePosY);
	float fogDistInt = fogDist * fogHeightDensityAtViewer;
	// Height based fog intensity
	float eyeToPixelY = eyeToPixel.y * (fogDist / pixelDist);
	float t = FogHeightFalloff * eyeToPixelY;
	const float thresholdT = 0.01;
	float fogHeightInt = abs(t) > thresholdT ? (1.0 - exp(-t)) / t : 1.0;
	// Combine both factors to get the final factor
	float fogFinalFactor = exp(-FogGlobalDensity * fogDistInt * fogHeightInt);
	// Find the sun highlight and use it to blend the fog color
	float sunHighlightFactor = saturate(dot(eyeToPixelNorm, normalize(FogSunDir)));
	sunHighlightFactor = pow(sunHighlightFactor, 8.0);

	float3 fogFinalColor = lerp(FogColor, FogHighlightColor, sunHighlightFactor);
	return lerp(fogFinalColor, originalColor, fogFinalFactor);
}

struct ps_in
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 worldNormal : WNORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITION;
	float4 shadowPos : SPOS;
	float3 uvWorley : UVWORLEY;
	float occlusion : OCCLUSION;
};

TextureCube skyIR : register(t0);
TextureCube skyPrefilter : register(t1);
Texture2D brdfLUT : register(t2);

Texture3D displacement1 : register(t3);
SamplerState sampState : SAMPLER : register(s0);

Texture2D shadowMap : TEXTURE : register(t7);

SamplerComparisonState shadowSampState : SAMPLER1 : register(s1);

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float2 texOffset(int u, int v)
{
	return float2(u * SHADOW_MAP_DELTA, v * SHADOW_MAP_DELTA);
}

float computeShadowFactor(float4 shadowPosH)
{
	float2 shadowUV = shadowPosH.xy / shadowPosH.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y;
    
	float depth = shadowPosH.z / shadowPosH.w; //In NDC, depthFromLightPosToPoint
    
	const float delta = SHADOW_MAP_DELTA;
	float percentLit = 0.0f;
    
    //PCF sampling for shadow map
	const int sampleRange = 3;
    [unroll]
	for (int x = -sampleRange; x <= sampleRange; x++)
	{
        [unroll]
		for (int y = -sampleRange; y <= sampleRange; y++)
		{
			percentLit += shadowMap.SampleCmpLevelZero(shadowSampState, shadowUV, depth, int2(x, y));
		}
	}
    //Avg of all samples
	percentLit /= ((sampleRange * 2 + 1) * (sampleRange * 2 + 1));
    
	return percentLit;
};

float4 main(ps_in input) : SV_TARGET
{
	float3 N = normalize(input.normal);
	float3 V = normalize(cameraPosition - input.worldPos);
	float3 albedo = float3(0.9f, 0.9f, 0.9f);
	float roughness = 0.9;
	float metallic = 0;
	
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);
	
	//float3 T = normalize(input.tangent);
	//float3 B = normalize(input.bitangent);

	//float3x3 TBN = float3x3(T, B, N);
	//N = normalize(mul(N, TBN));
	
	float NdotLInverted = float3(0, 0, 0);
	           
    // Reflectance equation
	float3 Lo = float3(0.0, 0.0, 0.0);
	{
		// Calculate per-light radiance
		float3 L = normalize(-skyLight.direction);
		float3 H = normalize(V + L);
		float3 radiance = skyLight.color * skyLight.brightness;
        
		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
		float3 kS = F;
		float3 kD = float3(1.0, 1.0, 1.0) - kS;
        
		float3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		float3 specular = numerator / max(denominator, 0.001);
		
		// Scale light based on incident light angle relative to normal
		float NdotL = max(dot(N, L), 0.0);
		NdotLInverted = 1 - NdotL;
		
		// Accumulate radiance
		float shadowFactor = computeShadowFactor(input.shadowPos);
		
		if (length(cameraPosition - input.worldPos) > RANGE)
			shadowFactor = 1.f;
		
		Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor;
	
		//Lo = Lo + shadowFactor * saturate(dot(-skyLight.direction.xyz, input.normal)) * skyLight.color.xyz * skyLight.brightness;
	}
	
	float3 ambient = float3(0.0, 0.0, 0.0);
	
	float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	float3 kS = F;
	float3 kD = 1.0 - kS;
  
	float3 irradiance = skyIR.Sample(sampState, N).rgb * environmentMapBrightness;
	float3 diffuse = irradiance * albedo;
	
	//float3 R = reflect(-V, N);
  
	//const float MAX_REFLECTION_LOD = 10.0;
	//float3 prefilteredColor = skyPrefilter.SampleLevel(sampState, R, roughness * MAX_REFLECTION_LOD).rgb * environmentMapBrightness;
	//float2 brdf = brdfLUT.Sample(sampState, float2(max(dot(N, V), 0.0f), roughness)).rg;
	//float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
	
	float occlusionInverted = 1 - input.occlusion;
	input.occlusion = pow(input.occlusion, occlusionFactor);
	
	ambient = (kD * diffuse) * input.occlusion;
	Lo *= input.occlusion;
	
	//NdotLInverted = pow(NdotLInverted, backlightFactor);
	occlusionInverted = pow(occlusionInverted, backlightFactor);
	float backSideMask = dot(skyLight.direction.xyz, N);
	float backLitCrevasseMask = clamp(occlusionInverted * backSideMask * backlightStrength * skyLight.brightness, 0, 100);
	float3 finalBacklightColor = backlightColor * backLitCrevasseMask;
	//return float4(finalBacklightColor, 1.0);
	//return float4(NdotLInverted, NdotLInverted, NdotLInverted, 1.0);
	//return float4(backLitCrevasseMask, backLitCrevasseMask, backLitCrevasseMask, 1.0);
	//return float4(backLitCrevasseMask, backLitCrevasseMask, backLitCrevasseMask, 1.0);
	
	float3 color = ambient + Lo + finalBacklightColor;

	// HDR tonemapping
	color = color / (color + float3(1.0, 1.0, 1.0));
	// Gamma correction
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
	
	// Atmospheric fog
	//float3 eyeToPixel = input.worldPos.xyz - cameraPosition.xyz;
	//color = ApplyFog(color, cameraPosition.y, eyeToPixel);
	
	//float yPos = input.worldPos.y;
	//float yRatio = 1 - remapToRange(yPos, cloudFogHeightStart, cloudFogHeightEnd, 0, 1);
    
	//color = lerp(color, cloudFogColor, clamp(yRatio, 0, 1) * cloudFogStrength);
	
	return float4(color, 1.0);
}