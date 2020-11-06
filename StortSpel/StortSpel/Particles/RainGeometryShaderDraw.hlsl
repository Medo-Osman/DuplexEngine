
struct particleGSDrawInput
{
    float3 wPos : POSITION;
    float2 wSize : SIZE;
    uint type : TYPE;
};

struct particleGSDrawOutput
{
    float4 hPos : SV_Position;
    float2 uv : TEXTCOORD0;
};

cbuffer particleData : register(b0)
{
    float3 g_worldAcceleration;
};

cbuffer camera : register(b1)
{
    float4x4 g_viewProjectionMatrix;
    float3 g_eyePos;
    float pad;
};



[maxvertexcount(4)]
void main(
	point particleGSDrawInput input[1] : SV_POSITION,
	inout TriangleStream<particleGSDrawOutput> output
)
{
    
	if(input[0].type == 1) //rain Particle
    {
        particleGSDrawOutput points[4];
        
        float3 worldUp = float3(0.f, 1.f, 0.f);
        float3 centerToEye = g_eyePos - input[0].wPos;
        centerToEye = normalize(centerToEye);
        float right = cross(centerToEye, worldUp);
        right = normalize(right);
        float3 up = cross(centerToEye, right);
        
        
        
        points[0].uv = float2(0, 1);
        points[0].hPos = mul(float4(input[0].wPos + (input[0].wSize.x / 2 * right) - (input[0].wSize.y / 2 * up), 1), g_viewProjectionMatrix); //Lower left from camera
        points[1].uv = float2(0, 0);
        points[1].hPos = mul(float4(input[0].wPos + (input[0].wSize.x / 2 * right) + (input[0].wSize.y / 2 * up), 1), g_viewProjectionMatrix); //upper left from camera
        points[2].uv = float2(1, 1);
        points[2].hPos = mul(float4(input[0].wPos - (input[0].wSize.x / 2 * right) - (input[0].wSize.y / 2 * up), 1), g_viewProjectionMatrix); //lower right from camera
        points[3].uv = float2(1, 0);
        points[3].hPos = mul(float4(input[0].wPos - (input[0].wSize.x / 2 * right) + (input[0].wSize.y / 2 * up), 1), g_viewProjectionMatrix); //upper right from camera
        
        
        for (int i = 0; i < 4; i++)
        {
            output.Append(points[i]);
        }
    }
}