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

struct lightComputeResult
{
	float3 lightColor;
	float diffuseLightFactor;
	float intensity;
};

//float distributionGGX(float NdotH, float roughness)
//{
//	float a = roughness * roughness;
//	float a2 = a * a;
//	float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
//	denom = PI * denom * denom;
//	return a2 / max(denom, 0.0000001);
//}

float distributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return nom / denom;
}

//float geometrySmith(float NdotV, float NdotL, float roughness)
//{
//	float r = roughness + 1.0;
//	float k = (r * r) / 8.0;
//	float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
//	float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
//	return ggx1 * ggx2;
//}

float geometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometrySchlickGGX(NdotV, roughness);
	float ggx1 = geometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

//float3 fresnelSchlick(float HdotV, float3 baseReflectivity)
//{
//    // baseReflectivity in range 0 to 1
//    // return range of baseReflectivity to 1
//    // increases as HdotV decreases (more reflectivity when surface viewed at larger angles)
//	return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - HdotV, 5.0);
//}

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    // baseReflectivity in range 0 to 1
    // return range of baseReflectivity to 1
    // increases as HdotV decreases (more reflectivity when surface viewed at larger angles)
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);

}

float3 fresnelSchlickRoughness(float HdotV, float3 F0, float roughness)
{
    // baseReflectivity in range 0 to 1
    // return range of baseReflectivity to 1
    // increases as HdotV decreases (more reflectivity when surface viewed at larger angles)
	//return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - HdotV, 5.0);
	return float3(F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - HdotV, 5.0));

}

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
};

SamplerState SamplerAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
	AddressU = Wrap;
	AddressV = Wrap;
};

float4 main(ps_in input) : SV_TARGET
{
	float3 albedo = float3(0.0, 1.0, 0.0);
	float3 metallic = materialMetallic;
	float roughness = materialRoughness;
	float ao = 1.0;
	
	float3 N = input.normal;
	float3 V = normalize(cameraPosition.xyz - input.worldPos.xyz);
	float NdotV = max(dot(N, V), 0.000001);
	float3 R = reflect(-V, N);
	
	float3 baseReflectivity = float3(0.04, 0.04, 0.04);
	baseReflectivity = lerp(baseReflectivity, albedo, metallic);
	
	// Reflectance equation
	float3 Lo = float3(0.0, 0.0, 0.0);
	for (int i = 0; i < 1; i++)
	{
		float3 L = normalize(pointLights[i].position.xyz - input.worldPos.xyz);
		float3 H = normalize(V + L);
		float pixelDistance = length(pointLights[i].position.xyz - input.worldPos.xyz);
		float attenuation = pointLights[i].intensity * (1.0 / (pixelDistance * pixelDistance));
		float3 radiance = pointLights[i].color * attenuation;

		// Cook-Torrance BRDF
		float NDF = distributionGGX(N, H, roughness);
		float G = geometrySmith(N, V, L, roughness);
		float3 F = fresnelSchlick(max(dot(H, V), 0.0), baseReflectivity);
		
		float3 nominator = NDF * G * F;
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
		float3 specular = nominator / denominator;
		
		// kS is equal to Fresnel
		float3 kS = F;
		
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		float3 kD = float3(1.0, 1.0, 1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;
		
		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);
		
		Lo += (kD * albedo / PI + specular) * radiance + NdotL;
	}
	
	// get ambient term from IBL
	float3 F = fresnelSchlick(NdotV, baseReflectivity);
	float3 kD = (1.0 - F) * (1.0 - metallic);
	float3 diffuse = skyIR.Sample(sampState, N).rgb * albedo * kD;
	
	float3 ambient = diffuse * ao;
	
	float3 color = ambient + Lo;
	
	// HDR tonemapping
	color = color / (color + float3(1.0, 1.0, 1.0));
	// Gamma correct
	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
	
	return float4(color, 1.0);
}

//float4 main(ps_in input) : SV_TARGET
//{
//	float3 albedo = float3(1.0, 0.0, 0.0);
//	//float actualRoughness = 0.0f;
//	float actualRoughness = materialRoughness;
//	//float actualMetallic = 1.0f;
//	float actualMetallic = materialMetallic;
	
//	input.normal = normalize(input.normal);
//	input.tangent = normalize(input.tangent);
	
//	float3 normalFromMap = normalTexture.Sample(sampState, input.uv).xyz * 2 - 1;

//	float3 N = input.normal;
//	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
//	float3 B = cross(T, N);

//	float3x3 TBN = float3x3(T, B, N);
//	input.normal = normalize(mul(normalFromMap, TBN));
	
//	N = input.normal;
//	float3 V = normalize(cameraPosition - input.worldPos);
	
//	float3 baseReflectivity = lerp(float3(0.04, 0.04, 0.04), albedo, actualMetallic);
	
//	// Reflectance equation
//	float3 Lo = float3(0.0, 0.0, 0.0);

//	for (int i = 0; i < nrOfPointLights; i++)
//	{	
//		float3 L = normalize(pointLights[i].position.xyz - input.worldPos.xyz);
//		float3 H = normalize(V + L);
//		float pixelDistance = length(pointLights[i].position.xyz - input.worldPos.xyz);
//		float attenuation = pointLights[i].intensity * 1.0 / (pixelDistance * pixelDistance);
//		float3 radiance = pointLights[i].color * attenuation;
	
//		// Cook-Torrance BRDF
//		float NdotV = max(dot(N, V), 0.0000001);
//		float NdotL = max(dot(N, L), 0.0000001);
//		float HdotV = max(dot(H, V), 0);
//		float NdotH = max(dot(N, H), 0);
	
//		float NDF = distributionGGX(NdotH, actualRoughness);
//		float G = geometrySmith(NdotV, NdotL, actualRoughness);
//		float3 F = fresnelSchlick(HdotV, baseReflectivity);
	
//		float3 nominator = NDF * G * F;
//		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
//		float3 specular = nominator / denominator;
	
//		float3 kS = F;
	
//		float3 kD = float3(1.0, 1.0, 1.0) - kS;
	
//		kD *= 1.0 - actualMetallic;
	
//		NdotL = max(dot(N, L), 0.0);
	
//		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
//	}
	
//	F = fresnelSchlickRoughness(NdotV, baseReflectivity, actualRoughness);
//	kD = (1.0 - F) * (1.0 - actualMetallic);
//	float3 diffuse = skyIR.Sample(sampState, N).rgb * albedo * kD;
	
//	const float MAX_REF_LOD = 4.0f;
//	float3 prefilteredColor = skyPrefilter.SampleLevel(sampState, reflect(-V, N), actualRoughness * MAX_REF_LOD).rgb;
//	float2 brdf = brdfLUT.Sample(sampState, float2(max(dot(input.normal, V), 0.0f), actualRoughness)).rg;
//	specular = prefilteredColor * (F * brdf.x + brdf.y);
	
//	float3 ambient = diffuse + specular;
	
//	float3 color = ambient + Lo;
	
//	color = color / (color + float3(1.0, 1.0, 1.0));
//	color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
	
//	return float4(color, 1.0);
//}