Texture2DMS<float4> glowInputTex : register(t0);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 gruoupId : SV_GroupID, uint3 groupThredId : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchId : SV_DispatchThreadID)
{
    uint2 pixelIndex = uint2(dispatchId.x, dispatchId.y);
    
    // Downsampling
    uint2 inputPixelIndex = pixelIndex * 2;
   
    // Thresholding on Downsample
    float4 value = glowInputTex.Load(inputPixelIndex, 0); 
    float4 value2 = glowInputTex.Load(inputPixelIndex + uint2(1, 0), 0);
    float4 hIntensity0 = lerp(value, value2, 0.5f);
    
    value = glowInputTex.Load(inputPixelIndex + uint2(0, 1), 0);
    value2 = glowInputTex.Load(inputPixelIndex + uint2(1, 1), 0);
    float4 hIntensity1 = lerp(value, value2, 0.5f);
    
    float4 glowIntensity = lerp(hIntensity0, hIntensity1, 0.5f);
    
    if (glowIntensity.x == 0.f && glowIntensity.y == 0.f && glowIntensity.z == 0.f)
        outputTex[pixelIndex] = float4(0.f, 0.f, 0.f, 1.f);
    else
        outputTex[pixelIndex] = glowIntensity;
}