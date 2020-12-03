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
	return float4(skyboxTexture.Sample(textureCubeSampler, float3(0, -res, 0.75)).xyz, 1);
	//return float4(res,0,0,1);
	return float4(skyboxTexture.Sample(textureCubeSampler, normalize(input.pos)).xyz, 1);
}