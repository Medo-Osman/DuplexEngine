#define MAX_LIGHTS 8

cbuffer lightBuffer : register(b0)
{
    float4 lightPosArray[8];
    float4 lightColorArray[8];
    int nrOfLights;
}

cbuffer lightBuffer : register(b1)
{
    float4 cameraPosition;
}

cbuffer perModel : register(b2)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 wvpMatrix;
};

struct ps_in
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
    float4 worldPos : POSITION;
    float4 worldNormal : WORLDNORMAL;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState sampState : SAMPLER : register(s0);

float4 main(ps_in input) : SV_TARGET
{
    for (int i = 0; i < nrOfLights; i++)
    {
        //Summera ljusen, inte gå över 1
    }
    
    float3 diffuse = diffuseTexture.Sample(sampState, input.uv).xyz;
    
    float3 lightPos = lightPosArray[0]; //lightPosArray[0].xyz;
    float3 lightDir = normalize(input.worldPos.xyz - lightPos);
    float diffuseLightFactor = saturate(dot(-lightDir, input.normal));

    return float4(diffuse * diffuseLightFactor, 1); 

}