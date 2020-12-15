cbuffer MaterialBuffer : register(b3)
{
    float materialUVScale;
    float materialRoughness;
    float materialMetallic;
    int materialTextured;
    float materialEmissiveStrength;
}

struct ps_in
{
    float4 pos          : SV_POSITION;
    float2 uv           : TEXCOORD;
    float3 vNormal      : VNORMAL;
    float3 vTangent     : TANGENT;
    float3 vBitangent   : BITANGENT;
    float3 vPos         : POSITION;
};

Texture2D normalTexture : TEXTURE : register(t2);
SamplerState sampState  : SAMPLER : register(s0);

float4 main(ps_in input) : SV_TARGET
{
    float3 N = normalize(input.vNormal);
    
    if (materialTextured)
    {
        float3 normalFromMap = normalTexture.Sample(sampState, input.uv).rgb * 2 - 1;
        input.vTangent = normalize(input.vTangent);

        float3 T = normalize(input.vTangent - N * dot(input.vTangent, N));
        float3 B = normalize(input.vBitangent - N * dot(input.vBitangent, N));

        float3x3 TBN = float3x3(T, B, N);
        N = normalize(mul(normalFromMap, TBN));
    }
    return float4(normalize(N), input.vPos.z);
}