#include "UtilityLibrary.hlsli"

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
	float cloudBedHeightPosition;
	float cloudDisplacementFactor;
	float cloudTessellationFactor;
	float worleyScale;
	float3 panSpeed;
	float tile;
	float dispPower;
	float normalIntensity;
	float occlusionFactor;
	float padding;
}

struct ds_out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 worldNormal : WNORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITION;
	float4 shadowPos : SPOS;
	float3 uvWorley : UVWORLEY;
	float occlusion : OCCLUSION;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3] : SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

void displaceStuff(inout float3 worldPos, inout float3 worldNormal, float3 worldTangent, float3 worldBinormal, inout float3 localNormal, inout float occlusion, float tile);

#define NUM_CONTROL_POINTS 3

Texture3D displacement1 : register(t3);
//Texture2D displacement2 : register(t4);

SamplerState sampState : SAMPLER : register(s0);

[domain("tri")]
ds_out main(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 BarycentricCoordinates : SV_DomainLocation,
	const OutputPatch<vs_out, 3> tri)
{
	ds_out Output = (ds_out) 0;
	
	float3 vWorldPos = BarycentricCoordinates.x * tri[0].worldPos.xyz + BarycentricCoordinates.y * tri[1].worldPos.xyz + BarycentricCoordinates.z * tri[2].worldPos.xyz;
	Output.worldPos = float4(vWorldPos, 1.0f);

	float3 vNormal = normalize(BarycentricCoordinates.x * tri[0].normal + BarycentricCoordinates.y * tri[1].normal + BarycentricCoordinates.z * tri[2].normal);
	Output.normal = vNormal;
	Output.worldNormal = normalize(mul(float4(vNormal, 0), worldMatrix));
	float3 vWorldNormal = Output.worldNormal;
	
	float3 finalTangent = normalize(BarycentricCoordinates.x * tri[0].tangent + BarycentricCoordinates.y * tri[1].tangent + BarycentricCoordinates.z * tri[2].tangent);
	Output.tangent = finalTangent;
	Output.tangent = normalize(mul(float4(finalTangent, 0), worldMatrix));
	
	float3 finalBiTangent = normalize(BarycentricCoordinates.x * tri[0].bitangent + BarycentricCoordinates.y * tri[1].bitangent + BarycentricCoordinates.z * tri[2].bitangent);
	Output.bitangent = finalBiTangent;
	Output.bitangent = normalize(mul(float4(finalBiTangent, 0), worldMatrix));
	
	float2 vUV = BarycentricCoordinates.x * tri[0].uv + BarycentricCoordinates.y * tri[1].uv + BarycentricCoordinates.z * tri[2].uv;
	Output.uv = vUV;
	
	// Displacement
	float3 texCoords = Output.worldPos.xyz * worleyScale + (time * panSpeed);
	Output.uvWorley = texCoords;
	float fDisplacement = displacement1.SampleLevel(sampState, texCoords, 0).r;
	fDisplacement = sin(fDisplacement);
	Output.occlusion = 1.0;

	// Get the coords for the tangent and binormal displacement
	float3 texCoordsTan = texCoords + normalize(Output.tangent.xyz) * 0.05;
	float3 texCoordsBiTan = texCoords + normalize(Output.bitangent.xyz) * 0.05;

	// Sample the displacement for the tangent normal
	float4 dispTan = displacement1.SampleLevel(sampState, texCoordsTan, 0);
	float4 dispBiTan = displacement1.SampleLevel(sampState, texCoordsBiTan, 0);

	// Get tangent normal offset from displacements
	//float2 localNormOffset = float2(fDisplacement.x - dispTan.x, fDisplacement.x - dispBiTan.x);
	float2 localNormOffset = float2(fDisplacement - dispBiTan.x, fDisplacement - dispTan.x);

	// Scale the normal by the one over the tiling value
	float oneOverTile = (1.0 / tile);
	Output.normal.xy += localNormOffset * 1.0 * oneOverTile;
	Output.occlusion *= pow(fDisplacement, oneOverTile);

	// Set up the tSpace thingy for converting tangent directions to world directions
	float3 tSpace0 = float3(Output.tangent.x, Output.bitangent.x, Output.worldNormal.x);
	float3 tSpace1 = float3(Output.tangent.y, Output.bitangent.y, Output.worldNormal.y);
	float3 tSpace2 = float3(Output.tangent.z, Output.bitangent.z, Output.worldNormal.z);

	// Update the world normal
	vWorldNormal.x = dot(tSpace0, Output.normal);
	vWorldNormal.y = dot(tSpace1, Output.normal);
	vWorldNormal.z = dot(tSpace2, Output.normal);
	vWorldNormal = normalize(Output.worldNormal);

	// push the world position in by the average value of the displacement map
	Output.worldPos.xyz -= vWorldNormal * 0.7937 * cloudDisplacementFactor * oneOverTile;
	// push the world position out based on the new world normal
	Output.worldPos.xyz += vWorldNormal * pow(fDisplacement, dispPower) * cloudDisplacementFactor * oneOverTile;

	float4x4 viewProjMatrix = mul(viewMatrix, projectionMatrix);
	Output.pos = mul(float4(Output.worldPos.xyz, 1), viewProjMatrix);
	//Output.normal = normalize(mul(float4(Output.normal, 0), worldMatrix));

	return Output;
}