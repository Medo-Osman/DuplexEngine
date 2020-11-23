
struct particleGSDrawInput //Output
{
	float3 wPos : POSITION;
	float3 wOldPos : OLDPOS;
	float3 wVel : VEL;
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

    if (input[0].type == 2)
    {
        particleGSDrawOutput points[4];

        float3 worldUp = float3(0.f, 1.f, 0.f);
        float3 centerToEye = g_eyePos - input[0].wPos;
        centerToEye = normalize(centerToEye);
        float right = cross(centerToEye, worldUp);
        right = normalize(right);
        float3 up = normalize(cross(centerToEye, right));

		float3 origin = input[0].wOldPos;
		float3 end = input[0].wPos;

        float3 originToEnd = end - origin;
		float3 pltp = g_eyePos - (origin + (end * 0.5f));
		float3 perp = normalize(cross(pltp, originToEnd));
		perp = perp * (length(originToEnd) * 0.25f);


        points[0].uv = float2(0, 1);
		points[0].hPos = mul(float4(origin + (perp * 0.5f), 1), g_viewProjectionMatrix);
        points[1].uv = float2(0, 0);
		points[1].hPos = mul(float4(end + (perp * 0.5f), 1), g_viewProjectionMatrix);
        points[2].uv = float2(1, 1);
		points[2].hPos = mul(float4(origin - (perp * 0.5f), 1), g_viewProjectionMatrix);
        points[3].uv = float2(1, 0);
		points[3].hPos = mul(float4(end - (perp * 0.5f), 1), g_viewProjectionMatrix);
		

        for (int i = 0; i < 4; i++)
        {
            output.Append(points[i]);
        }
    }
}