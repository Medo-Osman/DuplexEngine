struct particle
{
    float3 wPos : POSITION;
    float3 wOldPos : OLDPOS;
    float3 wVel : VEL;
    float2 wSize : SIZE;
    float time : TIME;
    float oldTime : OLDTIME;
    uint type : TYPE;
};

struct LineData
{
    float3 position;
    float pad;
    float3 direction;
    float paddy;
};


cbuffer particle : register(b0)
{
    float4 g_worldEmitPosition;
    float g_gameTime;
    float g_dt;
    float g_intensity; //1 - 100
};


cbuffer arrayOfLines : register(b1)
{
    LineData lineData[10];
}



SamplerState textureSampler : register(s0);
Texture1D randomTexture : register(t0);

[maxvertexcount(10)]
void main(point particle input[1], inout PointStream<particle> output)
{
    input[0].time += g_dt;
    if (input[0].type == 1) //Emitter
    {
        input[0].wOldPos = input[0].wPos;
        input[0].wPos = g_worldEmitPosition.xyz;

        for (int i = 2; i < 10; i++)
        {
            particle newParticle;
            newParticle.wPos = lineData[i].position;
            newParticle.wVel = lineData[i].direction;
            newParticle.wSize = float2(0.2, 0.2);
            newParticle.time = 0.f;
            newParticle.type = i;
            newParticle.wOldPos = newParticle.wPos + lineData[i].direction;
            newParticle.oldTime = 0;
            output.Append(newParticle);
        }

    }
    else
    {
        input[0].wPos = lineData[input[0].type - 2].position;
        input[0].wOldPos = lineData[input[0].type - 2].position + (lineData[input[0].type - 2].direction);
        
        output.Append(input[0]);
    }

}