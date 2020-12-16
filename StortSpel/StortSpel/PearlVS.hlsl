cbuffer globalConstBuffer : register(b4)
{
	float3 playerPosition;
	float environmentMapBrightness;
	float time;
}

float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}