
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
};

cbuffer perModel : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 wvpMatrix;
};

cbuffer shadowMap : register(b3)
{
    float4x4 lightViewMatrix;
    float4x4 lightProjMatrix;
    float4x4 shadowMatrix;
};



vs_out main(vs_in input)
{
    vs_out output;
    //output.pos = mul(float4(input.pos, 1), worldMatrix);
    //output.pos = mul(output.pos, lightViewMatrix);
    //output.pos = mul(output.pos, lightProjMatrix);
    output.pos = mul(float4(input.pos, 1), wvpMatrix);
    output.uv = input.uv;
    
    return output;
}