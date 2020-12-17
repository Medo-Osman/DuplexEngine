struct ps_in
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState sampState : SAMPLER : register(s0);



void main(ps_in input)
{
    clip(diffuseTexture.Sample(sampState, input.uv).a - 0.1);
}