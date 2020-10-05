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

cbuffer lightBuffer : register(b0)
{
    pointLight pointLights[8];
    int nrOfPointLights;
    
    spotLight spotLights[8];
    int nrOfSpotLights;
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

struct lightComputeResult
{
    float3 lightColor;
    float diffuseLightFactor;
    float intensity;
};

//Only calculating diffuse light
lightComputeResult computeLightFactor(ps_in input)
{
    lightComputeResult result;
    
    float3 lightPos;
    float3 lightDir = float3(0, 0, 0);
    float diffuseLightFactor = 0;
    float3 resultColor = float3(0, 0, 0);
    
    //Loop through all pointlights
    for (int i = 0; i < nrOfPointLights; i++)
    {
        //Summera ljusen, inte gå över 1
        lightPos = pointLights[i].position.xyz; //lightPosArray[0].xyz;
        lightDir = normalize(lightPos - input.worldPos.xyz);
        diffuseLightFactor = clamp(diffuseLightFactor + saturate(dot(lightDir, input.normal))*pointLights[i].intensity, 0.f, 1.f);
        
        //resultColor = saturate(resultColor + pointLights[i].color);
    }
    
    //Loop through all spotlights
    for (int j = 0; j < nrOfSpotLights; j++)
    {
        //Summera ljusen, inte gå över 1
        lightPos = spotLights[j].position.xyz; //lightPosArray[0].xyz;
        lightDir = normalize(lightPos - input.worldPos.xyz);
        
        diffuseLightFactor = diffuseLightFactor + pow(max(dot(-lightDir, spotLights[j].direction), 0), spotLights[j].coneFactor);
        resultColor = spotLights[j].color; //Have to fix color blending
    }
    
    result.diffuseLightFactor = max(0.1f, diffuseLightFactor); //0.2f is ambient light
    result.lightColor = resultColor;
    
    return result;
}

float4 main(ps_in input) : SV_TARGET
{
    
    float3 diffuse = diffuseTexture.Sample(sampState, input.uv).xyz;
    lightComputeResult lightResult = computeLightFactor(input);
   
    return float4(diffuse * lightResult.lightColor * lightResult.diffuseLightFactor, 1);

}