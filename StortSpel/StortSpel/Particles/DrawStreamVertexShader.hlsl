

struct particle //input
{
    float3 wPos : POSITION;
    float3 wVel : VEL;
    float2 wSize : SIZE;
    float time : TIME;
    uint type : TYPE;
};

struct particleGSDraw //Output
{
    float3 wPos : POSITION;
    float2 wSize : SIZE;
    uint type : TYPE;
};

cbuffer particleData : register(b0)
{
    float3 g_worldAcceleration;
};

particleGSDraw main(particle particleDraw)
{
    float time = particleDraw.time;
    particleGSDraw output;
    output.type = particleDraw.type;
    output.wSize = particleDraw.wSize;
    output.wPos = 0.5f * (time * time) * g_worldAcceleration + (time * particleDraw.wVel) + particleDraw.wPos; 
    
	return output;
}