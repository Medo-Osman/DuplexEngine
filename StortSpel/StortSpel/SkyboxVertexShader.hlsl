struct vs_in
{
    float3 pos  : POSITION;
    float2 uv   : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_Position;
    float3 pos      : POSITION;
};

cbuffer skyboxCbuffer : register(b0)
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