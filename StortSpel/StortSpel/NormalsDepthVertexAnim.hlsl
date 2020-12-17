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
    float3 vNormal : VNORMAL;
    float3 vTangent : TANGENT;
    float3 vBitangent : BITANGENT;
    float3 vPos : POSITION;
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

cbuffer cbPerFrame : register(b7)
{
    float4x4 viewToTexSpace;
    float4x4 worldInverseTransposeView;
    float4 offsetVectors[14];
    float4 frustumCorners[4];
    
    float occlusionRadius = 0.5f;
    float occlusionFadeStart = 0.2f;
    float occlusionFadeEnd = 2.0f;
    float surfaceEpsilon = 0.05f;
};

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
        blendedPosition += mul(float4(input.pos, 1.f), g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
        blendedTangent += mul(input.tangent, (float3x3) g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
        blendedBitangent += mul(input.bitangent, (float3x3) g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
        blendedNormal += mul(input.normal, (float3x3) g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
    }
    
    localPosition = float4(blendedPosition, 1.f);
    
    output.pos = mul(localPosition, wvpMatrix);
    output.uv = input.uv;
    
    output.vNormal = normalize(mul(float4(input.normal, 0), worldInverseTransposeView));
    output.vTangent = normalize(mul(float4(input.tangent, 0), worldInverseTransposeView));
    output.vBitangent = normalize(mul(float4(input.bitangent, 0), worldInverseTransposeView));
    
    float4 worldPos = mul(float4(input.pos, 1), worldMatrix);
    float4 vPos = mul(worldPos, viewMatrix);
    output.vPos = mul(float4(input.pos, 1), worldMatrix);
    output.vPos = mul(float4(output.vPos, 1), viewMatrix);
    
    return output;
}