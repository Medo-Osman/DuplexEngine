#include "UtilityLibrary.hlsli"

struct vs_out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 worldNormal : WNORMAL;
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

cbuffer cameraBuffer : register(b1)
{
	float4 cameraPosition;
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
	float3 padding;
}

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3] : SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor : SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
[partitioning("fractional_odd")] // integer; fractional_even; fractional_odd; pow2
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(14.99)]
vs_out main(
	InputPatch<vs_out, 3> p,
	uint i : SV_OutputControlPointID)
{
	vs_out output;
	
	output.pos = p[i].pos;
	output.uv = p[i].uv;
	output.normal = p[i].normal;
	output.worldNormal = p[i].worldNormal;
	output.tangent = p[i].tangent;
	output.bitangent = p[i].bitangent;
	output.worldPos = p[i].worldPos;
	output.shadowPos = p[i].shadowPos;
	
	return p[i];
}

// Patch Constant Function Declaration
HS_CONSTANT_DATA_OUTPUT ConstantHS(
	InputPatch<vs_out, 3> inputPatch,
	uint PatchID : SV_PrimitiveID)
{
	//HS_CONSTANT_DATA_OUTPUT Output = (HS_CONSTANT_DATA_OUTPUT) 0;
	HS_CONSTANT_DATA_OUTPUT Output;
	
	float gTessFactor = cloudTessellationFactor;
	
	[unroll]
	for (int vert = 0; vert < 3; vert++)
	{
		Output.EdgeTessFactor[vert] = gTessFactor;
	}
	
	Output.InsideTessFactor = gTessFactor;
	
	return Output;
}
