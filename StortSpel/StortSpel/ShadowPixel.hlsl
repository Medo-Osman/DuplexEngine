#define MAX_LIGHTS 8

struct pointLight
{
    float4 position;
    float3 color;
    float intensity;
};

struct spotLight
{
    float4 position;
    float3 color;
    float intensity;
    float3 direction;
    float coneFactor;
};

struct directionalLight
{
    float4 direction;
    float4 color;
    float brightness;
};

RasterizerState Depth
{
    DepthBias = 10000;
    DepthBiasClamp = 0.0f;
    SlopeScaledDepthBias = 1.0f;
};

cbuffer lightBuffer : register(b0)
{
    float1 ambientLightLevel;
    pointLight pointLights[8];
    int nrOfPointLights;
    
    spotLight spotLights[8];
    int nrOfSpotLights;
    
    directionalLight skyLight;
}

cbuffer cameraBuffer : register(b1)
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
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState sampState : SAMPLER : register(s0);

struct lightComputeResult
{
    float3 lightColor;
    float diffuseLightFactor;
    float intensity;
};

float4 main(ps_in input) : SV_TARGET
{
    
    float4 diffuse = diffuseTexture.Sample(sampState, input.uv);
   // clip(diffuse.a - 0.15f);
    
    return diffuse; //float4(lightResult.lightColor, 1);

}