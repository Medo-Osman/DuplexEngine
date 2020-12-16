#define MAX_LIGHTS 8

struct ps_in
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 ssaoPos : SSAOPOS;
};

struct ps_out
{
    float4 diffuse : SV_Target0;
    float4 glow : SV_Target1;
};

cbuffer MaterialBuffer : register(b3)
{
    float materialUVScale;
    float materialRoughness;
    float materialMetallic;
    int materialTextured;
    float materialEmissiveStrength;
}

Texture2D ambientOcclusionMap : TEXTURE : register(t8);
SamplerState sampState : SAMPLER : register(s0);

ps_out main(ps_in input) : SV_TARGET
{
    ps_out output;
    
    // SSAO
    input.ssaoPos /= input.ssaoPos.w;
    input.ssaoPos.x = (1.f + input.ssaoPos.x) * 0.5f;
    input.ssaoPos.y = (1.0f - input.ssaoPos.y) * 0.5f;
    float ambientFactor = ambientOcclusionMap.SampleLevel(sampState, input.ssaoPos.xy, 0.0f).r;
    
    // Emissive color
    float emissiveScalar = materialEmissiveStrength / 100.f;
    output.glow = float4(input.uv * emissiveScalar, 0.5f * emissiveScalar, 1.f);
    
    // Diffuse color
    output.diffuse = float4(input.uv, 0.5f, 1.f);
    
    return output;
}