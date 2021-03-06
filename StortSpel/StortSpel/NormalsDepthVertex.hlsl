struct vs_in
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
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

vs_out main(vs_in input)
{
    vs_out output;
    
    output.pos = mul(float4(input.pos, 1), wvpMatrix);
    output.uv = input.uv;
    
    output.vNormal = normalize(mul(float4(input.normal, 0), worldInverseTransposeView));
    output.vTangent = normalize(mul(float4(input.tangent, 0), worldInverseTransposeView));
    output.vBitangent = normalize(mul(float4(input.bitangent, 0), worldInverseTransposeView));
    
    output.vPos = mul(float4(input.pos, 1), worldMatrix);
    output.vPos = mul(float4(output.vPos, 1), viewMatrix);
    
    return output;
}