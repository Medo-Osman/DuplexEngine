struct vs_in
{
    float3 pos          : POSITION;
    float2 uv           : TEXCOORD;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 bitangent    : BITANGENT;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float3 pos      : POSITION;
};

cbuffer skyboxCbuffer : register(b1)
{
    float4x4 wvpMatrix;
};

vs_out main(vs_in input)
{
    vs_out output;
    
    output.position = mul(float4(input.pos, 1.0f), wvpMatrix).xyww;
    output.pos = input.pos;
    
    return output;
}