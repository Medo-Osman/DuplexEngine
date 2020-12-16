struct ps_in
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2DMS<float4> geometryTexture : register(t0);
Texture2D glowTexture : register(t1);
Texture2D randomTexture : register(t2);

SamplerState sampState : SAMPLER : register(s0);

float4 main(ps_in input) : SV_TARGET
{
    return mad(0.75f, glowTexture.Sample(sampState, input.uv), pow(geometryTexture.Load(input.pos.xy, 0), 2.2f));
    //return phongTexture.Load(input.pos.xy, 0);
}