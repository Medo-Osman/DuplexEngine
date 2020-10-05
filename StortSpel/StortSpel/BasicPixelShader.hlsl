#define MAX_LIGHTS 8

struct pointLight
{
    float4 position;
    float3 color;
    float intensity;
};

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
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState sampState : SAMPLER : register(s0);

float computeLightFactor(ps_in input)
{
    float3 lightPos;
    float3 lightDir = float3(0, 0, 0);
    float diffuseLightFactor = 0;
    for (int i = 0; i < nrOfLights; i++)
    {
        //Summera ljusen, inte gå över 1
        lightPos = lightPosArray[i].xyz; //lightPosArray[0].xyz;
        lightDir = normalize(lightPos - input.worldPos.xyz);
        diffuseLightFactor = diffuseLightFactor + saturate(dot(lightDir, input.normal));
       
    }
    
    return diffuseLightFactor;
}

float4 main(ps_in input) : SV_TARGET
{
    
    float3 diffuse = diffuseTexture.Sample(sampState, input.uv).xyz;
    float diffuseLightFactor = computeLightFactor(input);
   
    return float4(diffuse * diffuseLightFactor, 1);

}