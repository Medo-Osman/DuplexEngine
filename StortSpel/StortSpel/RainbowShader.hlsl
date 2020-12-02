struct ps_in
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float4 worldPos : POSITION;
};

cbuffer perModel : register(b2)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
    float4x4 wvpMatrix;
};

float3 HueShift(float3 Color, float Shift)
{
    float3 P = float3(0.55735, 0.55735, 0.55735) * dot(float3(0.55735, 0.55735, 0.55735), Color);
    
    float3 U = Color - P;
    
    float3 V = cross(float3(0.55735, 0.55735, 0.55735), U);

    Color = U * cos(Shift * 6.2832) + V * sin(Shift * 6.2832) + P;
    
    return float3(Color);
}

float3 rgb2hsl(float3 c)
{
    float h = 0.0;
    float s = 0.0;
    float l = 0.0;
    float r = c.r;
    float g = c.g;
    float b = c.b;
    float cMin = min(r, min(g, b));
    float cMax = max(r, max(g, b));

    l = (cMax + cMin) / 2.0;
    if (cMax > cMin)
    {
        float cDelta = cMax - cMin;
        
        //s = l < .05 ? cDelta / ( cMax + cMin ) : cDelta / ( 2.0 - ( cMax + cMin ) ); Original
        s = l < .0 ? cDelta / (cMax + cMin) : cDelta / (2.0 - (cMax + cMin));
        
        if (r == cMax)
        {
            h = (g - b) / cDelta;
        }
        else if (g == cMax)
        {
            h = 2.0 + (b - r) / cDelta;
        }
        else
        {
            h = 4.0 + (r - g) / cDelta;
        }

        if (h < 0.0)
        {
            h += 6.0;
        }
        h = h / 6.0;
    }
    return float3(h, s, l);
}

float4 main(ps_in input) : SV_TARGET
{
    float3 modelPosition = worldMatrix._41_42_43;

    float3 color = rgb2hsl(modelPosition);
    return float4(HueShift(color, worldMatrix._43/2), 1);
}