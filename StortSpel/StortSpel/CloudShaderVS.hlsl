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
	float4 worldPos : POSITION;
	float4 shadowPos : SPOS;
};

cbuffer perModel : register(b0)
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4x4 wvpMatrix;
};

cbuffer shadowMap : register(b3)
{
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
	float4x4 shadowMatrix;
};

vs_out main(vs_in input)
{
	vs_out output;
	output.pos = mul(float4(input.pos, 1), wvpMatrix);
	output.uv = input.uv;
	output.normal = input.normal;
	//output.tangent = normalize(mul(float4(input.tangent, 0), worldMatrix));
	output.tangent = input.tangent;
	//output.bitangent = normalize(mul(float4(input.bitangent, 0), worldMatrix));
	//output.bitangent = normalize(mul(float4(cross(output.tangent, output.normal), 0), worldMatrix));
	//output.bitangent = cross(output.tangent, output.normal);
	output.bitangent = input.bitangent;
	output.worldPos = mul(float4(input.pos, 1), worldMatrix);
	
	output.shadowPos = mul(float4(input.pos, 1), worldMatrix);
	output.shadowPos = mul(output.shadowPos, lightViewMatrix);
	output.shadowPos = mul(output.shadowPos, lightProjMatrix);
    
	return output;
}
