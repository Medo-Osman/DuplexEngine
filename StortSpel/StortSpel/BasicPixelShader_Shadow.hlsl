#define MAX_LIGHTS 8

static const float SHADOW_MAP_SIZE = 4096.f;
static const float SHADOW_MAP_DELTA = 1.f / SHADOW_MAP_SIZE;
static const float RANGE = 90.f;

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

cbuffer MaterialBuffer : register(b3)
{
    float materialUVScale;
    float materialRoughness;
    float materialMetallic;
    int materialTextured;
    float materialEmissiveStrength;
}

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

struct ps_out
{
    float4 diffuse : SV_Target0;
    float4 glow : SV_Target1;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState sampState : SAMPLER : register(s0);
SamplerComparisonState shadowSampState : SAMPLER1 : register(s1);
Texture2D shadowMap : TEXTURE : register(t7);

struct lightComputeResult
{
    float3 lightColor;
    float diffuseLightFactor;
    float intensity;
};

float2 texOffset(int u, int v)
{
    return float2(u * SHADOW_MAP_DELTA, v * SHADOW_MAP_DELTA);
}

float computeShadowFactor(float4 shadowPosH)
{
    float2 shadowUV = shadowPosH.xy / shadowPosH.w * 0.5f + 0.5f; 
    shadowUV.y = 1.0f - shadowUV.y;
    
    float depth = shadowPosH.z / shadowPosH.w; //In NDC, depthFromLightPosToPoint
    
    const float delta = SHADOW_MAP_DELTA;
    float percentLit = 0.0f;
    
    //PCF sampling for shadow map
    const int sampleRange = 2;
    [unroll]
    for (int x = -sampleRange; x <= sampleRange; x++)
    {
        [unroll]
        for (int y = -sampleRange; y <= sampleRange; y++)
        {
            percentLit += shadowMap.SampleCmpLevelZero(shadowSampState, shadowUV, depth, int2(x, y));
        }
    }
    //Avg of all samples
    percentLit /= ((sampleRange * 2 + 1) * (sampleRange * 2 + 1));
    
    return percentLit; 
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
    float shadowFactor = computeShadowFactor(input.shadowPos);
    
    //Loop through all pointlights
    for (int i = 0; i < nrOfPointLights; i++)
    {
        //Summera ljusen, inte gå över 1
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
        //Summera ljusen, inte gå över 1
        lightPos = spotLights[j].position.xyz;
        lightDir = normalize(lightPos - input.worldPos.xyz);
        
        float d = distance(lightPos, input.worldPos.xyz);
        float attenuationFactor = saturate(spotLights[j].intensity / (0 + (0.01f * d) + (0 * (d * d))));
        
        float spotLightFactor = pow(max(dot(-lightDir, spotLights[j].direction), 0), spotLights[j].coneFactor);
        diffuseLightFactor = spotLightFactor * (diffuseLightFactor + attenuationFactor);
        finalColor = saturate(finalColor + (diffuseLightFactor * spotLights[j].color * attenuationFactor));
    }
    if (length(cameraPosition - input.worldPos) > RANGE)
        shadowFactor = 1.f;
    
    finalColor = finalColor + shadowFactor*saturate(dot(-skyLight.direction.xyz, input.normal)) * skyLight.color.xyz * skyLight.brightness * 0.2f;
    
    result.lightColor = (finalColor * diffuse + (diffuse * ambientLightLevel));
    
    return result;
}

ps_out main(ps_in input)
{
    ps_out output;
    lightComputeResult lightResult = computeLightFactor(input);
    
    // Emissive color
    float emissiveScalar = materialEmissiveStrength / 100.f;
    output.glow = float4(lightResult.lightColor * emissiveScalar, 1.f);
    
    // Diffuse color
    output.diffuse = float4(lightResult.lightColor, 1.f);
    
    return output;
}