#define MAX_LIGHTS 8

struct ps_in
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};




float4 main(ps_in input) : SV_TARGET
{
    return float4(input.color.xyz, 1);
}