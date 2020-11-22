
//struct vs_in
//{
//    float3 pos : POSITION;
//    float3 color : COLOR;
//};

struct vs_in
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct vs_out
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITIONWS;
	float4 objPos : POSITIONOS;
};

cbuffer perModel : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 wvpMatrix;
};


vs_out main(vs_in input)
{
	vs_out output;
	output.pos = mul(float4(input.pos, 1), wvpMatrix);
	output.uv = input.uv;
	output.normal = normalize(mul(float4(input.normal, 0), worldMatrix)).xyz;
	output.tangent = input.tangent;
	output.bitangent = input.bitangent;
	output.worldPos = mul(float4(input.pos, 1), worldMatrix);
	output.objPos = float4(input.pos, 1.0);
    
	return output;
}