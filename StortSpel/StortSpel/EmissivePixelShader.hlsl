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
    float3 vNormal : VNORMAL;
    float depth : DEPTH;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 worldPos : POSITION;
    float4 shadowPos : SPOS;
    float4 ssaoPos : SSAOPOS;
};

struct ps_out
{
    float4 diffuse : SV_Target0;
    float4 glow : SV_Target1;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
Texture2D emissiveTexture : TEXTURE : register(t1);
Texture2D ambientOcclusionMap : TEXTURE : register(t8);
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
    float3 finalColor = float3(0, 0, 0);
    float3 diffuse = diffuseTexture.Sample(sampState, input.uv).xyz;
    
    // SSAO
    input.ssaoPos /= input.ssaoPos.w;
    input.ssaoPos.x = (1.f + input.ssaoPos.x) * 0.5f;
    input.ssaoPos.y = (1.0f - input.ssaoPos.y) * 0.5f;
    float ambientFactor = ambientOcclusionMap.SampleLevel(sampState, input.ssaoPos.xy, 0.0f).r;
    
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
    
    finalColor = finalColor + saturate(dot(-skyLight.direction.xyz, input.normal)) * skyLight.color.xyz * skyLight.brightness;
    
    result.lightColor = (finalColor * diffuse + (diffuse * ambientLightLevel * ambientFactor));
    //result.lightColor = (finalColor * diffuse + (float3(1, 0, 0) * (-ambientFactor + 1)));
    //result.lightColor = float3(ambientFactor, ambientFactor, ambientFactor);
    
    return result;
}

ps_out main(ps_in input) : SV_TARGET
{
    ps_out output;
    lightComputeResult lightResult = computeLightFactor(input);
    
    // Emissive color
    float4 emissive = emissiveTexture.Sample(sampState, input.uv);
    float emissiveScalar = materialEmissiveStrength / 100.f;
    float3 emStrengthColor = float3(emissiveScalar, emissiveScalar, emissiveScalar);
    output.glow = float4(emissive.rgb * emissiveScalar, 1.f);
    
    // Diffuse color
    float3 diffuse = lightResult.lightColor * (1.f - length(emissive.rgb));
    output.diffuse = float4(diffuse, 1.f) + float4(emissive.rgb + (emStrengthColor * length(emissive.rgb)), 1.f);
    
    return output;
}