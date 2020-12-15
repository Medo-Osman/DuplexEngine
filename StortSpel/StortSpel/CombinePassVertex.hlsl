struct vs_in
{
    float3 pos : POSITION;
    float3 normals : NORMAL;
    float2 uv : TEXCOORD;
};

struct vs_out
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

vs_out main(vs_in input)
{
    vs_out output;
    output.pos = float4(input.pos, 1);
    output.uv = input.uv;
    
    return output;
}