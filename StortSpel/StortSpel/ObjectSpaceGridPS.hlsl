struct ps_in
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float4 worldPos : POSITIONWS;
	float4 objPos : POSITIONOS;
};

cbuffer cameraBuffer : register(b1)
{
	float4 cameraPosition;
}

float4 main(ps_in input) : SV_TARGET
{
	// 1
	float3 FaceColor = float3(1.0, 0.0, 0.0);
	float3 LineColor = float3(1.0, 1.0, 1.0);
	
	float GridInterval = 0.75;
	float LineExp = 20;
	float LineBoost = 0.2;
	float SpaceOpacity = 0;
	
	float FalloffStartScale = 0.025;
	float FalloffRadius = 250;
	float FadeExp = 0.5;
	float LineBrightness = 1;
	float FaceBrightness = 1;
	
	//float3 vWSOS = float3(input.worldPos.xyz - input.objPos.xyz);
	float3 vWSOS = input.worldPos;
	float3 vWSOSdGridInterval = cos(vWSOS / GridInterval);
	float3 lineExpAmplify = pow(vWSOSdGridInterval, LineExp);
	
	float addXandZ = lineExpAmplify.x + lineExpAmplify.z;
	float multiplyLineBoost = clamp((addXandZ * LineBoost), SpaceOpacity, 1.0);
	float result1 = multiplyLineBoost;
	
	// 2
	float vWSCPLength = length(input.worldPos.xyz - cameraPosition.xyz);
	float FSSFR = FalloffStartScale * FalloffRadius;
	float A = vWSCPLength - FSSFR;
	float B = FalloffRadius - FSSFR;
	float C = clamp(A, 0.0, B);
	float D = C / B;
	D = -D;
	
	float E = pow(-D, FadeExp);
	float result2 = E;
	
	// 1 * 2
	//float2 maskResult = saturate(result1 * result2);
	
	// Blend between colors
	//float2 finalResult = lerp(FaceColor, LineColor, maskResult);
	
	//return float4(finalResult, 1.0f);
	return float4(result1, result1, result1, 1.0f);
}