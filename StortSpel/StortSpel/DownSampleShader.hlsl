Texture2DMS<float4> inputTex : register(t0);
Texture2DMS<float4> glowInputTex : register(t1);
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
    float4 hIntensity0 = lerp(value, value2, 0.5);
    
    value = glowInputTex.Load(inputPixelIndex + uint2(0, 1), 0);
    value2 = glowInputTex.Load(inputPixelIndex + uint2(1, 1), 0);
    float4 hIntensity1 = lerp(value, value2, 0.5);
    
    float4 glowIntensity = lerp(hIntensity0, hIntensity1, 0.5);
    
    if (glowIntensity.x == 0 && glowIntensity.y == 0 && glowIntensity.z == 0)
    {
        value = inputTex.Load(inputPixelIndex, 0);
        value2 = inputTex.Load(inputPixelIndex + uint2(1, 0), 0);
        hIntensity0 = lerp(value, value2, 0.5);
    
        value = inputTex.Load(inputPixelIndex + uint2(0, 1), 0);
        value2 = inputTex.Load(inputPixelIndex + uint2(1, 1), 0);
        hIntensity1 = lerp(value, value2, 0.5);
        
        float4 intensity = lerp(hIntensity0, hIntensity1, 0.5);
    
        if (length(pow(intensity.rgb, 3.f)) > 1.0f)
        {
            outputTex[pixelIndex] = float4(intensity.rgb, 1.0);
        }
        else
            outputTex[pixelIndex] = float4(0.f, 0.f, 0.f, 1.0);
    }
    else
    {
        outputTex[pixelIndex] = float4(glowIntensity.rgb, 1.0);
    }
}