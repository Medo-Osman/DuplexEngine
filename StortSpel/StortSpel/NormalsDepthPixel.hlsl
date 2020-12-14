struct ps_in
{
    float4 pos : SV_POSITION;
    float3 vNormal : VNORMAL;
    float3 vPos : POSITION;
};

float4 main(ps_in input) : SV_TARGET
{
    return float4(normalize(input.vNormal), input.vPos.z);
}