struct ps_in
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D normalsNDepth : register(t0);
Texture2D randomTexture : register(t1);

SamplerState sampState : SAMPLER : register(s0);

float4 main(ps_in input) : SV_TARGET
{
    float4 normalDepth = normalsNDepth.SampleLevel(sampState, input.uv, 0.0f);

    float3 n = normalDepth.xyz;
    float pz = normalDepth.w;
    
    float3 randVec = 2.0f * randomTexture.SampleLevel(sampState, 4.0f * input.uv, 0.0f).rgb - 1.0f;


    float output;
    
    return output;
}