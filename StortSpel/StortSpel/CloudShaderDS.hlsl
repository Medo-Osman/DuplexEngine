struct vs_out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITION;
	float4 shadowPos : SPOS;
};

cbuffer perModel : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 wvpMatrix;
};

cbuffer GlobalConstBuffer : register(b1)
{
	float3 playerPosition;
	float environmentMapBrightness;
	float time;
}

cbuffer cloudConstBuffer : register(b2)
{
	float cloudHeightPosition;
	float cloudDisplacementFactor;
	float cloudTessellationFactor;
	float cloudNoiseScale1;
	float cloudNoiseScale2;
	float cloudNoiseSpeed1;
	float cloudNoiseSpeed2;
	float cloudNoiseBlendFactor;
}

struct ds_out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITION;
	float4 shadowPos : SPOS;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3] : SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
};

#define NUM_CONTROL_POINTS 3

Texture2D displacementORM : register(t6);
Texture3D asdwet : register(t7);

SamplerState sampState : SAMPLER : register(s0);

[domain("tri")]
ds_out main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 BarycentricCoordinates : SV_DomainLocation,
	const OutputPatch<vs_out, 3> tri)
{
	ds_out Output = (ds_out) 0;
	
	float3 vWorldPos = BarycentricCoordinates.x * tri[0].worldPos + BarycentricCoordinates.y * tri[1].worldPos + BarycentricCoordinates.z * tri[2].worldPos;
	Output.worldPos = float4(vWorldPos, 1.0f);
	
	float3 vNormal = normalize(BarycentricCoordinates.x * tri[0].normal + BarycentricCoordinates.y * tri[1].normal + BarycentricCoordinates.z * tri[2].normal);
	Output.normal = vNormal;
	
	float3 finalTangent = normalize(BarycentricCoordinates.x * tri[0].tangent + BarycentricCoordinates.y * tri[1].tangent + BarycentricCoordinates.z * tri[2].tangent);
	Output.tangent = finalTangent;
	
	float3 finalBiTangent = normalize(BarycentricCoordinates.x * tri[0].bitangent + BarycentricCoordinates.y * tri[1].bitangent + BarycentricCoordinates.z * tri[2].bitangent);
	Output.bitangent = finalBiTangent;
	
	float2 vUV = BarycentricCoordinates.x * tri[0].uv + BarycentricCoordinates.y * tri[1].uv + BarycentricCoordinates.z * tri[2].uv;
	Output.uv = vUV;
	
	// Displacement
	float3 vDirection = vNormal;
	
	float fDisplacement1 = displacementORM.SampleLevel(sampState, Output.uv * cloudNoiseScale1 + (time * cloudNoiseSpeed1), 0).r;
	float fDisplacement2 = displacementORM.SampleLevel(sampState, Output.uv * cloudNoiseScale2 + (time * cloudNoiseSpeed2), 0).g;
	
	float finalDisplacement = lerp(fDisplacement1, fDisplacement2, cloudNoiseBlendFactor);
	
	vWorldPos += float4(0.0, cloudHeightPosition, 0.0, 1.0);
	vWorldPos += vDirection * finalDisplacement * cloudDisplacementFactor;

	float4x4 viewProjMatrix = mul(viewMatrix, projectionMatrix);
	
	Output.pos = mul(float4(vWorldPos.xyz, 1.0f), viewProjMatrix);

	return Output;
}
