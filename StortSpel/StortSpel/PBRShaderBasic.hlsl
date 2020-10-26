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
	float materialReflectance;
	float materialRoughness;
	float materialMetallic;
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
SamplerState sampState : SAMPLER : register(s0);

struct lightComputeResult
{
	float3 lightColor;
	float diffuseLightFactor;
	float intensity;
};

//float distributionGGX(float NdotH, float roughness)
//{
//    float a = roughness * roughness;
//    float a2 = a * a;
//    float denom = NdotH * NdotH * (a2 - 1.0) + 1.0;
//    denom = PI * denom * denom;
//	return a2 / max(denom, 0.0000001);
//}

//float geometrySmith(float NdotV, float NdotL, float roughness)
//{
//	float r = roughness + 1.0;
//	float k = (r * r) / 8.0;
//	float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
//	float ggx2 = NdotL / (NdotL * (1.0 - k) + k);
//	return ggx1 * ggx2;
//}

//float3 fresnelSchlick(float HdotV, float3 baseReflectivity)
//{
//    // baseReflectivity in range 0 to 1
//    // return range of baseReflectivity to 1
//    // increases as HdotV decreases (more reflectivity when surface viewed at larger angles)
//	return baseReflectivity + (1.0 - baseReflectivity) * pow(1.0 - HdotV, 5.0);
//}

struct VertexToPixel
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 worldPos : POSITION;
	float2 uv : TEXCOORD;
};

static const float PI = 3.14159265359;

SamplerState SamplerAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
	AddressU = Wrap;
	AddressV = Wrap;
};

float NormalDistributionGGXTR(float3 normalVec, float3 halfwayVec, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a; // a2 = a^2
	float NdotH = max(dot(normalVec, halfwayVec), 0.0); // NdotH = normalVec.halfwayVec
	float NdotH2 = NdotH * NdotH; // NdotH2 = NdotH^2
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = PI * denom * denom;

	return nom / denom;
}


float GeometrySchlickGGX(float NdotV, float roughness)  // k is a remapping of roughness based on direct lighting or IBL lighting
{
	float r = roughness + 1.0f;
	float k = (r * r) / 8.0f;

	float nom = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return nom / denom;
}


float GeometrySmith(float3 normalVec, float3 viewDir, float3 lightDir, float k)
{
	float NdotV = max(dot(normalVec, viewDir), 0.0f);
	float NdotL = max(dot(normalVec, lightDir), 0.0f);
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 F0)   // cosTheta is n.v and F0 is the base reflectivity
{
	return (F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f));
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)   // cosTheta is n.v and F0 is the base reflectivity
{
	return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0f);
}



void CalcRadiance(ps_in input, float3 viewDir, float3 normalVec, float3 albedo, float roughness, float metallic, float3 lightPos, float3 lightCol, float3 F0, out float3 rad, out float tempReturnF, out float3 tempReturnF3)
{
	static const float PI = 3.14159265359;

	//calculate light radiance
	float3 lightDir = normalize(lightPos - input.worldPos.xyz);
	float3 halfwayVec = normalize(viewDir + lightDir);
	float pixelDistance = length(lightPos - input.worldPos.xyz);
	float attenuation = 1.0f / (pixelDistance * pixelDistance);
	float3 radiance = lightCol * attenuation;

	//Cook-Torrance BRDF
	float D = NormalDistributionGGXTR(normalVec, halfwayVec, roughness);
	float G = GeometrySmith(normalVec, viewDir, lightDir, roughness);
	float3 F = FresnelSchlick(max(dot(halfwayVec, viewDir), 0.0f), F0);

	float3 kS = F;
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= 1.0 - metallic;

	float3 nom = D * G * F;
	float denom = 4 * max(dot(normalVec, viewDir), 0.0f) * max(dot(normalVec, lightDir), 0.0) + 0.001f; // 0.001f just in case product is 0
	float3 specular = nom / denom;

	//Add to outgoing radiance Lo
	float NdotL = max(dot(normalVec, lightDir), 0.0f);
	rad = (((kD * albedo / PI) + specular) * radiance * NdotL);
	tempReturnF = roughness;
	tempReturnF3 = halfwayVec;
}

float4 main(ps_in input) : SV_TARGET
{
	//float3 albedo = pow(albedoTexture.Sample(sampState, input.uv).rgb, 2.2f);
	float3 albedo = float3(1.0f, 0.0f, 0.0f);

	//Normal
	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	
	float3 normalFromMap = normalTexture.Sample(sampState, input.uv).xyz * 2 - 1;

	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N));
	float3 B = cross(T, N);

	float3x3 TBN = float3x3(T, B, N);
	//input.normal = normalize(mul(normalFromMap, TBN));
	
	float3 normalVec = input.normal;
	
	//Metallic
	//float metallic = metallicTexture.Sample(sampState, input.uv).r;
	//float metallic = 0.5f;
	float metallic = materialMetallic;

	//Rough
	//float rough = roughnessTexture.Sample(sampState, input.uv).r;
	float rough = materialRoughness;
	
	float3 viewDir = normalize(cameraPosition - input.worldPos);
	
	float3 R = reflect(-viewDir, normalVec);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

	float3 rad = float3(0.0f, 0.0f, 0.0f);
	//reflectance equation
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	
	float3 lightPos1 = float3(10.0f, 10.0f, -10.0f);
	float lightIntensity = 50.0f;
	float3 lightCol = float3(1.0f, 1.0f, 1.0f);
	lightCol *= lightIntensity;

	float tempReturnF;
	float3 tempReturnF3;

	CalcRadiance(input, viewDir, normalVec, albedo, rough, metallic, lightPos1, lightCol, F0, rad, tempReturnF, tempReturnF3);
	Lo += rad;

	float3 kS = FresnelSchlickRoughness(max(dot(normalVec, viewDir), 0.0f), F0, rough);
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= 1.0 - metallic;

	float3 irradiance = skyIR.Sample(sampState, normalVec).rgb;
	float3 diffuse = albedo * irradiance;

	const float MAX_REF_LOD = 4.0f;
	float3 prefilteredColor = skyPrefilter.SampleLevel(sampState, R, rough * MAX_REF_LOD).rgb;
	float2 brdf = brdfLUT.Sample(sampState, float2(max(dot(normalVec, viewDir), 0.0f), rough)).rg;
	float3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

	float ao = 1.0f;
	float3 ambient = (kD * diffuse + specular) * ao;
	float3 color = ambient + Lo;

	color = color / (color + float3(1.0f, 1.0f, 1.0f));
	color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));

	return float4(ao, ao, ao, 1.0f);
}