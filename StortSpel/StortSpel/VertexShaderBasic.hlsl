
struct vs_in
{
    float3 pos : POSITION;
    float3 color : COLOR;
};

struct vs_out
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

cbuffer perModel : register(b0)
{
    float4x4 wvpMatrix;
};


vs_out main(vs_in input)
{
    vs_out output;
    output.pos = mul(float4(input.pos, 1), wvpMatrix);
    output.color = input.color;
    
    return output;
}