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
}

struct ps_in
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITION;
};

TextureCube skyIR : register(t0);
TextureCube skyPrefilter : register(t1);
Texture2D brdfLUT : register(t2);

Texture2D albedoTexture : TEXTURE : register(t3);
Texture2D normalTexture : TEXTURE : register(t4);
Texture2D roughnessTexture : TEXTURE : register(t5);
Texture2D metallicTexture : TEXTURE : register(t6);
Texture2D aoTexture : TEXTURE : register(t7);
SamplerState sampState : SAMPLER : register(s0);

static const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float4 main(ps_in input) : SV_TARGET
{
	float3 N = normalize(input.normal);
	float3 V = normalize(cameraPosition - input.worldPos);
	
	float3 albedo = float3(1.0, 1.0, 1.0);
	float3 metallic = materialMetallic;
	float roughness = materialRoughness;
	float ao = 1.0f;
	
	if (materialTextured)
	{
		albedo = pow(albedoTexture.Sample(sampState, input.uv).rgb, 2.2f);
		metallic = metallicTexture.Sample(sampState, input.uv).r;
		roughness = roughnessTexture.Sample(sampState, input.uv).r;
		ao = aoTexture.Sample(sampState, input.uv).r;
		float3 normalFromMap = normalTexture.Sample(sampState, input.uv).xyz * 2 - 1;
	
		input.tangent = normalize(input.tangent);

		float3 T = normalize(input.tangent - N * dot(input.tangent, N));
		float3 B = cross(T, N);

		float3x3 TBN = float3x3(T, B, N);
		N = normalize(mul(normalFromMap, TBN));
	}
	
	float3 F0 = float3(0.04, 0.04, 0.04);
	F0 = lerp(F0, albedo, metallic);
	           
    // Reflectance equation
	float3 Lo = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < nrOfPointLights; i++)
	{
		float3 lightPos = pointLights[i].position.xyz;
		float3 lightCol = pointLights[i].color;
		float lightIntensity = pointLights[i].intensity;
		
		// Calculate per-light radiance
		float3 L = normalize(lightPos - input.worldPos.xyz);
		float3 H = normalize(V + L);
		float distance = length(lightPos - input.worldPos.xyz);
		float attenuation = 1.0 / (distance * distance) * lightIntensity;
		float3 radiance = lightCol * attenuation;
        
		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        
		float3 kS = F;
		float3 kD = float3(1.0, 1.0, 1.0) - kS;
		kD *= 1.0 - metallic;
        
		float3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		float3 specular = numerator / max(denominator, 0.001);
		
		// Scale light based on incident light angle relative to normal
		float NdotL = max(dot(N, L), 0.0);
		
		// Accumulate radiance
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}
	
	float3 ambient = float3(0.0, 0.0, 0.0);
	
	// Ambient from IBL
	
	//float3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	//float3 kD = 1.0 - kS;
	//float3 irradiance = skyIR.Sample(sampState, N).rgb * albedo * kD;
	//float3 diffuse = irradiance * albedo;
	//ambient = (kD * diffuse) * ao;
	
	float3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

	float3 kS = F;
	float3 kD = 1.0 - kS;
	kD *= 1.0 - metallic;
  
	float3 irradiance = skyIR.Sample(sampState, N).rgb;
	float3 diffuse = irradiance * albedo;
	
	float3 R = reflect(-V, N);
  
	const float MAX_REFLECTION_LOD = 10.0;
	float3 prefilteredColor = skyPrefilter.SampleLevel(sampState, R, roughness * MAX_REFLECTION_LOD).rgb;
	float2 brdf = brdfLUT.Sample(sampState, float2(max(dot(N, V), 0.0f), roughness)).rg;
	float3 specular = prefilteredColor * (F * brdf.x + brdf.y);
  
	ambient = (kD * diffuse + specular) * ao;
	
	// Ambient from IBL
	//float3 F = fresnelSchlick(dot(N, V), F0);
	//float kD = (1.0 - F) * (1.0 - metallic);
	//float3 diffuse = skyIR.Sample(sampState, N).rgb * albedo * kD;
	
	//ambient = diffuse * ao;
	
	// Ambient constant
	//ambient = float3(0.03, 0.03, 0.03) * albedo * ao;
	
	// Combine ambience and specular
	float3 color = ambient + Lo;
	
	// HDR tonemapping
	color = color / (color + float3(1.0, 1.0, 1.0));
	// Gamma correction
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
   
	return float4(color, 1.0);
}