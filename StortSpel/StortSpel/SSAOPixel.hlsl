struct vs_out
{
    float4 pos : SV_POSITION;
    float3 toFarPlane : TEXCOORD0;
    float2 uv : TEXCOORD1;
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

float occlusionFunction(float distZ)
{
    float occlusion = 0.0f;
    if (distZ > surfaceEpsilon)
    {
        float fadeLength = occlusionFadeEnd - occlusionFadeStart;
        occlusion = saturate((occlusionFadeEnd - distZ) / fadeLength);
    }
    return occlusion;
}

Texture2D normalsNDepthTexture : register(t0);
Texture2D randomTexture : register(t1);

SamplerState randomVectorSamplerState : register(s2);
SamplerState normalsNDepthSamplerState : register(s3);

float4 main(vs_out input) : SV_TARGET
{
    float4 normalDepth = normalsNDepthTexture.SampleLevel(normalsNDepthSamplerState, input.uv, 0.0f);
 
    float3 n = normalDepth.xyz;
    float pz = normalDepth.w;

    float3 p = (pz / input.toFarPlane.z) * input.toFarPlane;
        
    float3 randVec = 2.0f * randomTexture.SampleLevel(randomVectorSamplerState, 4.0f * input.uv, 0.0f).rgb - 1.0f;

    float occlusionSum = 0.0f;
    
    int sampleCount = 14;
    
    [unroll]
    for (int i = 0; i < sampleCount; ++i)
    {
        float3 offset = reflect(offsetVectors[i].xyz, randVec);
    
        float flip = sign(dot(offset, n));
        
        float3 q = p + flip * occlusionRadius * offset;
        
        float4 projQ = mul(float4(q, 1.0f), viewToTexSpace);
        projQ /= projQ.w;

        float rz = normalsNDepthTexture.SampleLevel(normalsNDepthSamplerState, projQ.xy, 0.0f).a;

        float3 r = (rz / q.z) * q;
        
        float distZ = p.z - r.z;
        float dp = max(dot(n, normalize(r - p)), 0.0f);
        float occlusion = dp * occlusionFunction(distZ);
        
        occlusionSum += occlusion;
    }
    
    occlusionSum /= sampleCount;
    
    float access = 1.0f - occlusionSum;
    
    return saturate(pow(access, 4.f));
}