SamplerState g_sampler : register(s0);
Texture2D g_texture : register(t0);

struct particleGSDrawOutput
{
    float4 hPos : SV_Position;
    float2 uv : TEXTCOORD0;
};

float4 main(particleGSDrawOutput input) : SV_TARGET
{
    float4 textureSamp = g_texture.Sample(g_sampler, input.uv);
    clip(textureSamp.a - 0.1);
    return textureSamp;
}