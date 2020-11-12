

struct particle //input
{
    float3 wPos : POSITION;
	float3 wOldPos : OLDPOS;
    float3 wVel : VEL;
    float2 wSize : SIZE;
    float time : TIME;
	float oldTime : OLDTIME;
    uint type : TYPE;
};

struct particleGSDraw //Output
{
    float3 wPos : POSITION;
	float3 wOldPos : OLDPOS;
    float3 wVel : VEL;
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
    output.wPos = (time * particleDraw.wVel) + particleDraw.wPos;
	output.wOldPos = (particleDraw.oldTime * particleDraw.wVel) + particleDraw.wOldPos;

    return output;
}