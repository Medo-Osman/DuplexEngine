#define MAX_LIGHTS 8

static const float SHADOW_MAP_SIZE = 2048.f;
static const float SHADOW_MAP_DELTA = 1.f / SHADOW_MAP_SIZE;

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
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 worldPos : POSITION;
    float4 shadowPos : SPOS;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState sampState : SAMPLER : register(s0);
SamplerComparisonState shadowSampState : SAMPLER : register(s1);
Texture2D shadowMap : TEXTURE : register(t2);

struct lightComputeResult
{
    float3 lightColor;
    float diffuseLightFactor;
    float intensity;
};

float computeShadowFactor(float4 shadowPosH)
{
    shadowPosH.xyz /= shadowPosH.w; //Finish projection
    
    float depth = shadowPosH.z; //In NDC
    
    const float delta = SHADOW_MAP_DELTA;
    float percentLit = 0.0f;
    
    //Filtering matrix
    const float2 offsets[9] =
    {
        float2(-delta, -delta), float2(0.0f, -delta),   float2(delta, -delta),
        float2(-delta, 0.0f),   float2(0.0f, 0.0f),     float2(delta, 0.0f),
        float2(-delta, +delta), float2(0.0f, +delta),   float2(delta, +delta)
    };
    
    //PCF Filtering, sum all the samples
    [unroll]
    for (int i = 0; i < 9; i++) //9 because matrix size
    {
        percentLit += shadowMap.SampleCmpLevelZero(shadowSampState, shadowPosH.xy + offsets[i], depth);
        //percentLit += shadowMap.SampleCmpLevelZero(samShadow,shadowPosH.xy + offsets[i], depth).r;
    }
    
    //Avg of all samples
    return percentLit/9.f; //percentLit /= 9.f; //9 because matrix size.
};

//Only calculating diffuse light
lightComputeResult computeLightFactor(ps_in input)
{
    lightComputeResult result;
    
    float3 lightPos;
    float3 lightDir = float3(0, 0, 0);
    float diffuseLightFactor = 0;
    float3 finalColor = float3(0, 0, 0);
    float3 diffuse = diffuseTexture.Sample(sampState, input.uv).xyz;
    
    //Loop through all pointlights
    for (int i = 0; i < nrOfPointLights; i++)
    {
        //Summera ljusen, inte g� �ver 1
        lightPos = pointLights[i].position.xyz;
        lightDir = normalize(lightPos - input.worldPos.xyz);
      
        float d = distance(lightPos, input.worldPos.xyz);
        float attenuationFactor = pointLights[i].intensity / (0 + (0.1f * d) + (0 * (d * d)));
        
        diffuseLightFactor = clamp(diffuseLightFactor + saturate(((dot(lightDir, input.normal)))), 0.f, 1.f);
        finalColor = clamp(finalColor + pointLights[i].color * diffuseLightFactor * attenuationFactor, 0, 1);

    }
    
    //Loop through all spotlights
    for (int j = 0; j < nrOfSpotLights; j++)
    {
        //Summera ljusen, inte g� �ver 1
        lightPos = spotLights[j].position.xyz;
        lightDir = normalize(lightPos - input.worldPos.xyz);
        
        float d = distance(lightPos, input.worldPos.xyz);
        float attenuationFactor = saturate(spotLights[j].intensity / (0 + (0.01f * d) + (0 * (d * d))));
        
        float spotLightFactor = pow(max(dot(-lightDir, spotLights[j].direction), 0), spotLights[j].coneFactor);
        diffuseLightFactor = spotLightFactor * (diffuseLightFactor + attenuationFactor);
        finalColor = saturate(finalColor + (diffuseLightFactor * spotLights[j].color * attenuationFactor));
    }
    
    finalColor = finalColor + saturate(dot(-skyLight.direction.xyz, input.normal)) * skyLight.color.xyz * skyLight.brightness;
    
    
   // float shadowFactor = 1.f;
    float shadowFactor = computeShadowFactor(input.shadowPos);
    result.lightColor = (finalColor * diffuse * shadowFactor + (diffuse * ambientLightLevel));
    
    return result;
}

float4 main(ps_in input) : SV_TARGET
{
    
   
    lightComputeResult lightResult = computeLightFactor(input);
    return float4(lightResult.lightColor, 1);

}

    
