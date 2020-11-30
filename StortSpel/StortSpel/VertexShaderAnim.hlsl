#define zeroVector3 {0, 0, 0};
struct vs_in
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	uint4 bones : BONES;
    float4 weights : WEIGHTS;
};

struct vs_out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
    float3 vNormal : VNORMAL;
    float depth : DEPTH;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	
};

cbuffer perModel : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 wvpMatrix;
};

cbuffer anim : register(b2)
{
    float4x4 g_boneMatrixPallet[30]; //Final transfroms for bones
}


vs_out main(vs_in input, in uint vID : SV_VertexID)
{
    vs_out output;
    float4 localPosition;
    float3 blendedPosition = zeroVector3;
    float3 blendedTangent = zeroVector3;
	float3 blendedBitangent = zeroVector3;
    float3 blendedNormal = zeroVector3;
    
    for (int i = 0; i < 4; i++)
    {
		blendedPosition  += mul(float4(input.pos, 1.f),     g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
        blendedTangent   += mul(input.tangent, (float3x3)   g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
		blendedBitangent += mul(input.bitangent, (float3x3) g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
		blendedNormal    += mul(input.normal, (float3x3)    g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
    }
    
    localPosition = float4(blendedPosition, 1.f);
    
    //output.normal = mul(input.normal, (float3x3) inverseTransposeWorldMatrix);
	output.normal = blendedNormal;
    float4 worldPos = mul(float4(input.pos, 1), worldMatrix);
    float4 vPos = mul(worldPos, viewMatrix);
    output.vNormal = normalize(mul(float4(output.normal, 0), viewMatrix));
	output.tangent = blendedTangent;
	output.bitangent = blendedBitangent;
	output.uv = input.uv;
    output.pos = mul(localPosition, wvpMatrix);
    output.depth = 1 - output.pos.z;

	//output.wsPos = mul(localPosition, worldMatrix);
    //output.wsTangent = mul(input.tangent, (float3x3) worldMatrix);
    
    //output.hsShadowPos = mul(localPosition, shadowTransform);

    
		return output;
	}