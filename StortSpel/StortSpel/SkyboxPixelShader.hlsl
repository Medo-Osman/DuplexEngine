#include "UtilityLibrary.hlsli"

struct ps_in
{
    float4 position     : SV_POSITION;
    float3 pos          : POSITION;
};

cbuffer cameraBuffer : register(b1)
{
	float4 cameraPosition;
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

TextureCube skyboxTexture : register(t0);
SamplerState textureCubeSampler : register(s0);

float4 main(ps_in input) : SV_TARGET
{
	float res = mul(dot(float3(0, 1, 0), normalize(input.pos)) + 1, 0.5);
	//float3 skyboxColor = skyboxTexture.Sample(textureCubeSampler, float3(0, -res, 0.75)).xyz;
	//return float4(res,0,0,1);
	return float4(skyboxTexture.Sample(textureCubeSampler, normalize(input.pos)).xyz, 1);
    
	//float yPos = input.pos.y;
	//float yRatio = 1 - remapToRange(yPos, -.2, 0.15, 0, 1);
    
	//skyboxColor = lerp(skyboxColor, float4(1.0, 1.0, 1.0, 1.0), clamp(yRatio, 0, 1));
	
	//input.pos *= 0.001;
	
	//float3 eyeToPixel = input.pos.xyz - cameraPosition.xyz;
	//skyboxColor = ApplyFog(skyboxColor.xyz, cameraPosition.y, eyeToPixel);
	
	//float yPos = input.pos.y;
	//float yRatio = 1 - remapToRange(yPos, cloudFogHeightStart, cloudFogHeightEnd, 0, 1);
    
	//skyboxColor = lerp(skyboxColor, float3(1.0, 1.0, 1.0), clamp(yRatio, 0, 1) * cloudFogStrength);
    
	//return float4(skyboxColor, 1);

}