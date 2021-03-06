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

cbuffer particle : register(b0)
{
    float4 g_worldEmitPosition;
    float g_gameTime;
    float g_dt;
    float g_intensity; //1 - 100
};

SamplerState g_sampler : register(s0);
Texture1D g_randomTexture : register(t0);


[maxvertexcount(45)]
void main(
	point particle input[1], 
	inout PointStream<particle> output
)
{
    input[0].time += g_dt;
	if(input[0].type == 2) //Not emitter
    {
        if(input[0].time <= 4.0f)
        {
            output.Append(input[0]);
        }
    }
    else if(input[0].type == 1) //Emitter
    {
        //Emitt cetain umber of new raindrops at certain time intervall dependign on intensity of weather
        if (input[0].time > 0.5)
        {
            uint nrOfParticleToCreateOnEmit = 44;
            for (uint i = 0; i < nrOfParticleToCreateOnEmit; i++)
            {
                float3 randomVector = 35.0f * g_randomTexture.SampleLevel(g_sampler, (float)((float)i/(float)nrOfParticleToCreateOnEmit + g_gameTime), 0).xyz;
                randomVector.y = 30.0f;
                
                particle newParticle;
                newParticle.time = 0.f;
                newParticle.type = 2;
                newParticle.wPos = g_worldEmitPosition.xyz + randomVector;
                newParticle.wVel = float3(0.f, 0.f, 0.f);
                newParticle.wSize = float2(0.5f, 0.5f);
                
                output.Append(newParticle);

            }
            
            input[0].time = 0.0f;

        }
        
        output.Append(input[0]);
    }
}