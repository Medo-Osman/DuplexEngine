struct ps_in
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2DMS<float4> geometryTexture : register(t0);
Texture2D glowTexture : register(t1);
SamplerState sampState : SAMPLER : register(s0);

float4 main(ps_in input) : SV_TARGET
{
    float4 glowColor = glowTexture.Sample(sampState, input.uv);
    return mad(0.75f, float4(glowColor.rgb, 1.f), pow(geometryTexture.Load(input.pos.xy, 0), 2.2f));
    //return phongTexture.Load(input.pos.xy, 0);
}