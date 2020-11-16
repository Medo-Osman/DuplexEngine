#define RADIUS 15

Texture2D<float4> inputTex : register(t0);
RWTexture2D<float4> outputTex : register(u0);

cbuffer blurBuffer : register(b0)
{
    // Saved as float4 because float packing behaviour
    float4 weights[(RADIUS + 1) / 4];
    int radius;
    int direction;
}

[numthreads(8, 8, 1)]
void main(uint3 gruoupId : SV_GroupID, uint3 groupThredId : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchId : SV_DispatchThreadID)
{
    uint2 pixelIndex = uint2(dispatchId.x, dispatchId.y);
    float4 result = float4(0.f, 0.f, 0.f, 0.f);
    float4 inputColor;
    // Positive and Negative directions
    int2 direction2 = int2(direction - 1, direction);
    
    for (int i = -radius; i <= radius; ++i)
    {
        uint absOfi = (uint)abs(i);
        
        inputColor = inputTex[mad(i, direction2, pixelIndex)];
        result += (weights[absOfi >> 2][absOfi & 3] * inputColor.a) * float4(inputColor.rgb, 1.f);
    }
    
    outputTex[pixelIndex] = result;
}