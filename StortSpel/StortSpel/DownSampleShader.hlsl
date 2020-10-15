Texture2D<float4> inputTex : register(t0);
RWTexture2D<float4> outputTex : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 gruoupId : SV_GroupID, uint3 groupThredId : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchId : SV_DispatchThreadID)
{
    uint2 pixelIndex = uint2(dispatchId.x, dispatchId.y);

    if (inputTex[pixelIndex].a != 0.f)
    {
        // Downsampling
        uint2 inputPixelIndex = pixelIndex * 2;
        float4 hIntensity0 = lerp(inputTex[inputPixelIndex], inputTex[inputPixelIndex + uint2(1, 0)], 0.5);
        float4 hIntensity1 = lerp(inputTex[inputPixelIndex + uint2(0, 1)], inputTex[inputPixelIndex + uint2(1, 1)], 0.5);
        float4 intensity = lerp(hIntensity0, hIntensity1, 0.5);

        // Thresholding on Downsample
        if (length(intensity.rgb) > 1.f)
            outputTex[pixelIndex] = float4(intensity.rgb, 1.0);
        else
            outputTex[pixelIndex] = float4(0.f, 0.f, 0.f, 1.0);
    }
}