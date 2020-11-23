struct particleGSDraw //Output
{
	float3 wPos : POSITION;
	float3 wOldPos : OLDPOS;
	float2 wSize : SIZE;
	uint type : TYPE;
};

struct particleGSDrawOutput
{
	float4 hPos : SV_Position;
	float2 uv : TEXTCOORD0;
};

cbuffer camera : register(b1)
{
	float4x4 g_viewProjectionMatrix;
	float3 g_eyePos;
	float pad;
};



[maxvertexcount(2)]
void main(point particleGSDraw input[1],
            inout LineStream<particleGSDrawOutput> output)
{
	if (input[0].type != 1)
	{
		particleGSDrawOutput v0;
        v0.hPos = mul(float4(input[0].wPos, 1.0f), g_viewProjectionMatrix);
		v0.uv = float2(0.0f, 0.0f);
		output.Append(v0);
		
		particleGSDrawOutput v1;
        v1.hPos = mul(float4(input[0].wOldPos, 1.0f), g_viewProjectionMatrix);
		v1.uv = float2(1.0f, 1.0f);
		output.Append(v1);
	}
}