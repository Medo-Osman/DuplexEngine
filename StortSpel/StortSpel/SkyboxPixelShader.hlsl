struct ps_in
{
    float4 position : SV_Position;
    float3 pos      : POSITION;
};

TextureCube skyboxTexture : register(t0);
SamplerState textureCubeSampler : register(s1);

float4 main(ps_in input) : SV_TARGET
{
    float4 output;
    
    output = float4(skyboxTexture.Sample(textureCubeSampler, input.pos).xyz, 1);
    
    return output;
}