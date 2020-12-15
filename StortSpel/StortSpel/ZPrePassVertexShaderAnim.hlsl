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
    float3 blendedPosition = zeroVector3;
    
    for (int i = 0; i < 4; i++)
    {
        blendedPosition += mul(float4(input.pos, 1.f), g_boneMatrixPallet[input.bones[i]]).xyz * input.weights[i];
    }
    
    output.uv = input.uv;
    output.pos = mul(float4(blendedPosition, 1.f), wvpMatrix);


    return output;
}