struct vs_in
{
    float3 pos          : POSITION;
};

struct vs_out
{
    float4 position     : SV_POSITION;
    float3 pos          : POSITION;
};

cbuffer skyboxCbuffer : register(b1)
{
    matrix wvpMatrix;
};

vs_out main(vs_in input)
{
    vs_out output;
    
    output.position = mul(float4(input.pos, 1), wvpMatrix).xyww;
    output.pos = input.pos;
    
    return output;
}