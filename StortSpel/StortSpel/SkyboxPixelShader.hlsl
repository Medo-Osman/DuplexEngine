#include "UtilityLibrary.hlsli"

struct ps_in
{
    float4 position     : SV_POSITION;
    float3 pos          : POSITION;
};

TextureCube skyboxTexture : register(t0);
SamplerState textureCubeSampler : register(s0);

float4 main(ps_in input) : SV_TARGET
{
	float res = mul(dot(float3(0, 1, 0), normalize(input.pos)) + 1, 0.5);
	float4 skyboxColor = float4(skyboxTexture.Sample(textureCubeSampler, float3(0, -res, 0.75)).xyz, 1);
	//return float4(res,0,0,1);
	//return float4(skyboxTexture.Sample(textureCubeSampler, normalize(input.pos)).xyz, 1);
    
	float yPos = input.pos.y * 100;
	float yRatio = remapToRange(yPos, 0, 200, 0, 1);
    
	skyboxColor = lerp(skyboxColor, float4(1.0, 1.0, 1.0, 1.0), clamp(yRatio, 0, 1));
    
	return skyboxColor;
}