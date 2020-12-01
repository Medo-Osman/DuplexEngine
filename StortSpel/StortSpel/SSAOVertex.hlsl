struct vs_in
{
    float3 pos : POSITION;
    float3 toFarPlaneIndex : NORMAL;
    float2 uv : TEXCOORD;
};

struct vs_out
{
    float4 pos : SV_POSITION;
    float3 toFarPlane : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

cbuffer cbPerFrame
{
    float4x4 viewToTexSpace;
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
    
    // Already in NDC space
    output.pos = float4(input.pos, 1);
    
    // We store the index to the frustum corner in the normal x-coord slot 
    output.toFarPlane = frustumCorners[input.toFarPlaneIndex.x].xyz;
    
    output.uv = input.uv;
    
    return output;
}