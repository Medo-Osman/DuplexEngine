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

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
[partitioning("fractional_odd")] // integer; fractional_even; fractional_odd; pow2
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(15.0)]
vs_out main(
	InputPatch<vs_out, 3> p,
	uint i : SV_OutputControlPointID)
{
	vs_out output;
	
	output.pos = p[i].pos;
	output.uv = p[i].uv;
	output.normal = p[i].normal;
	output.tangent = p[i].tangent;
	output.bitangent = p[i].bitangent;
	output.worldPos = p[i].worldPos;
	output.shadowPos = p[i].shadowPos;
	
	return p[i];
}

// Patch Constant Function Declaration
HS_CONSTANT_DATA_OUTPUT ConstantHS(
	InputPatch<vs_out, 3> i,
	uint PatchID : SV_PrimitiveID)
{
	//HS_CONSTANT_DATA_OUTPUT Output = (HS_CONSTANT_DATA_OUTPUT) 0;
	HS_CONSTANT_DATA_OUTPUT Output;
	
	//float gTessFactor = 1.0f;
	
	// Dynamic Tessellation
	
	//float maxTessellationFactor = 5.0f;
	//float minTessellationFactor = 1.0f;
	//float maxTessellationDistance = 300.0f;
	//float minTessellationDistance = 2.0f;
	
	// Calculate the center of the patch
	//float3 patchCenter = (i[0].pos + i[1].pos + i[2].pos) / 3.0f;
	//float3 patchPosition = mul(float4(patchCenter, 1.0f), worldMatrix).xyz;
	
	// Calculate tessellation factor based on distance from the camera
	//gTessFactor = max(
	//	min(maxTessellationFactor,
	//	(maxTessellationDistance - length(patchPosition - cameraPos.xyz)) / (maxTessellationDistance - minTessellationDistance) * maxTessellationFactor),
	//1.0f);
	
	//gTessFactor = max(
	//	min(maxTessellationFactor,
	//	(maxTessellationDistance - distance(patchPosition, cameraPos.xyz)) / (maxTessellationDistance - minTessellationDistance) * maxTessellationFactor),
	//1.0f);
	
	//gTessFactor = (distance(patchCenter, float3(0.0f, 0.0f, 0.0f))) * 0.005f;
	//gTessFactor = maxTessellationFactor - (distance(cameraPos.xyz, patchCenter) * 0.005f);
	
	//gTessFactor = (distance(patchPosition, cameraPos.xyz));
	
	//gTessFactor = 1.0f;
	
	//[unroll]
	//for (int vert = 0; vert < 3; vert++)
	//{
	//	Output.EdgeTessFactor[vert] = gTessFactor;
	//}
	
	//Output.InsideTessFactor = gTessFactor;
	
	float gTessFactor = 8.0f;
	
	[unroll]
	for (int vert = 0; vert < 3; vert++)
	{
		Output.EdgeTessFactor[vert] = gTessFactor;
	}
	
	Output.InsideTessFactor = gTessFactor;
	
	return Output;
}