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

cbuffer cloudConstBuffer : register(b6)
{
	float cloudHeightPosition;
	float cloudDisplacementFactor;
	float cloudTessellationFactor;
	float cloudNoiseScale1;
	float cloudNoiseScale2;
	float cloudNoiseSpeed1;
	float cloudNoiseSpeed2;
	float cloudNoiseBlendFactor;
}

float3 ApplyFog(float3 originalColor, float eyePosY, float3 eyeToPixel)
{
	float pixelDist = length(eyeToPixel);
	float3 eyeToPixelNorm = eyeToPixel / pixelDist;
	// Find the fog staring distance to pixel distance
	float fogDist = max(pixelDist - FogStartDepth, 0.0);
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

#define NUM_CELLS	16.0	// Needs to be a multiple of TILES!
#define TILES 		2.0		// Normally set to 1.0 for a creating a tileable texture.

#define SHOW_TILING			// Display yellow lines at tiling locations.
#define ANIMATE			// Basic movement using texture values.

//float2 Hash2(float p)
//{
//#ifdef ANIMATE
	
//	float t = frac(time * .0003);
//	return texture(iChannel0, p * float2(.135 + t, .2325 - t), -100.0).xy;
	
//#else
	
//	float r = 523.0*sin(dot(p, vec2(53.3158, 43.6143)));
//	return vec2(fract(15.32354 * r), fract(17.25865 * r));
	
//#endif
//}

//float Cells(in float2 p, in float numCells)
//{
//	p *= numCells;
//	float d = 1.0e10;
//	for (int xo = -1; xo <= 1; xo++)
//	{
//		for (int yo = -1; yo <= 1; yo++)
//		{
//			float2 tp = floor(p) + float2(xo, yo);
//			tp = p - tp - Hash2(fmod(tp, numCells / TILES));
//			d = min(d, dot(tp, tp));
//		}
//	}
//	return sqrt(d);
//	//return 1.0 - d;// ...Bubbles.
//}

TextureCube skyIR : register(t0);
TextureCube skyPrefilter : register(t1);
Texture2D brdfLUT : register(t2);

Texture2D perlin32N : register(t3);
Texture2D perlin64N : register(t5);
Texture2D displacementORM : register(t6);

SamplerState sampState : SAMPLER : register(s0);

static const float PI = 3.14159265359;

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

float4 main(ps_in input) : SV_TARGET
{
	float3 N = normalize(input.normal);
	
	//float3 normalFromMap = normal1.Sample(sampState, input.uv).xyz * 2 - 1;
	float3 normalFromMap1 = perlin32N.Sample(sampState, input.uv * cloudNoiseScale1 + (time * cloudNoiseSpeed1)).rgb * 2 - 1;
	float3 normalFromMap2 = perlin64N.Sample(sampState, input.uv * cloudNoiseScale2 + (time * cloudNoiseSpeed2)).rgb * 2 - 1;
		
	//float3 blendedNormal = BlendAngleCorrectedNormals(normalFromMap1, normalFromMap2);
	//float3 N2 = normalize(normalFromMap1);
	//N = normalize(lerp(normalFromMap1, normalFromMap2, cloudNoiseBlendFactor));
	//N = normalize(blendedNormal);
	
	float3 colorFromDispMap1 = displacementORM.Sample(sampState, input.uv * cloudNoiseScale1 + (time * cloudNoiseSpeed1)).r;
	float3 colorFromDispMap2 = displacementORM.Sample(sampState, input.uv * cloudNoiseScale2 + (time * cloudNoiseSpeed2)).g;
	float finalDisplacement = lerp(colorFromDispMap1, colorFromDispMap2, cloudNoiseBlendFactor);
	
	float roughness = 0.9;
	float3 albedo = float3(0.9f, 0.9f, 0.9f);

	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	//float3 B = cross(T, N);
	float3 B = normalize(input.bitangent - N * dot(input.bitangent, N));

	float3x3 TBN = float3x3(T, B, N);
	N = normalize(mul(normalFromMap1, TBN));
	
	//float3 V = normalize(cameraPosition - input.worldPos);
	
	//float yMask = -N.g;
	
	//float3 albedo = float3(0.9f, 0.9f, 0.9f);
	//float3 mixColor = float3(0.33f, 0.83f, 0.79f);
	
	//float3 skyLightDirection = normalize(float3(1.0f, 0.0f, 0.0f));
	//float skyLightDot = dot(-skyLightDirection, N);
  
	//float3 irradiance = skyIR.Sample(sampState, N).rgb;
	//float3 diffuse = lerp(irradiance, albedo, 0.7f);
	
	//float backSideMask = skyLightDot;
	
	//float3 mixedColor = lerp(diffuse, mixColor, saturate(backSideMask));
	
	float3 V = normalize(cameraPosition - input.worldPos);
	
	float3 F0 = float3(0.04, 0.04, 0.04);
	
	float3 Lo = float3(0.0, 0.0, 0.0);
	
	// Directional light calculation
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
		
		// Accumulate radiance
		//float shadowFactor = computeShadowFactor(input.shadowPos);
		
		//if (length(cameraPosition - input.worldPos) > RANGE)
		//	shadowFactor = 1.f;
		
		//Lo += (kD * albedo / PI + specular) * radiance * NdotL * shadowFactor;
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	
		//Lo = Lo + shadowFactor * saturate(dot(-skyLight.direction.xyz, input.normal)) * skyLight.color.xyz * skyLight.brightness;
	}
	
	float3 ambient = float3(0.0, 0.0, 0.0);
	
	float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	float3 kS = F;
	float3 kD = 1.0 - kS;
  
	float3 irradiance = skyIR.Sample(sampState, N).rgb;
	float3 diffuse = irradiance * albedo;
	
	float3 R = reflect(-V, N);
  
	const float MAX_REFLECTION_LOD = 10.0;
	float3 prefilteredColor = skyPrefilter.SampleLevel(sampState, R, roughness * MAX_REFLECTION_LOD).rgb;
	float2 brdf = brdfLUT.Sample(sampState, float2(max(dot(N, V), 0.0f), roughness)).rg;
	float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
  
	//ambient = (kD * diffuse + specular) * ao;
	ambient = (kD * diffuse + specular);
	
	float3 color = ambient + Lo;
	
	// HDR tonemapping
	color = color / (color + float3(1.0, 1.0, 1.0));
	// Gamma correction
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
   
	//backSideMask *= 4.0f;
	
	//float simpleDotColor = dot(N, -skyLightDot);

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
	
	//float3 simpleDotColorColor = AdjustContrast(float3(simpleDotColor, simpleDotColor, simpleDotColor), 4);
	
	//return float4(finalDisplacement.r, 0.0, 0.0, 1.0);
	
	
	
	
	//float3 eyeToPixel = input.worldPos.xyz - cameraPosition.xyz;
	//color = ApplyFog(color, cameraPosition.y, eyeToPixel);
	
	//float yPos = input.worldPos.y;
	//float yRatio = 1 - remapToRange(yPos, cloudFogHeightStart, cloudFogHeightEnd, 0, 1);
    
	//color = lerp(color, cloudFogColor, clamp(yRatio, 0, 1) * cloudFogStrength);
	
	
	
	
	//return float4(simpleDotColorColor, 1.0);
	//return float4(N, 1.0);
	return float4(color, 1.0);
	//return float4(skyLightDot, skyLightDot, skyLightDot, 1.0);
	//return float4(finalDisplacement, finalDisplacement, finalDisplacement, 1.0);
	
	//float3 contrastN = AdjustContrast(N, 2);
	//return float4(contrastN, 1.0f);
	//return float4(color, 1.0f);
	//return float4(skyLight.direction.xyz, 1.0f);
	//return float4(shaded, shaded, shaded, 1.0);
	
	//return float4(normalFromMap, 1.0f);
}