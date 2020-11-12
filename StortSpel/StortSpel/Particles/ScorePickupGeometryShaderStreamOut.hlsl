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


SamplerState textureSampler : register(s0);
Texture1D randomTexture : register(t0);

[maxvertexcount(31)]
void main(point particle input[1], inout PointStream<particle> output)
{
	float oldTime = input[0].time;
	input[0].time += g_dt;
    if (input[0].type == 1) //Emitter
    {
        for (int i = 0; i < 30; i++)
        {
            float offset = (float)((float)i / (float)30);
            float3 randomDirectionVectorFromEmitter = randomTexture.SampleLevel(textureSampler, offset, 0).xyz; // Get a new random number for every new particle
            //randomDirectionVectorFromEmitter.y = 1;
            //randomDirectionVectorFromEmitter.z = 0;
			randomDirectionVectorFromEmitter = normalize(randomDirectionVectorFromEmitter) * 7;

            particle newParticle;
            newParticle.wPos = g_worldEmitPosition.xyz;
            newParticle.wVel = randomDirectionVectorFromEmitter;
            newParticle.wSize = float2(0.2, 0.2);
            newParticle.time = 0.f;
            newParticle.type = 2;
			newParticle.wOldPos = g_worldEmitPosition.xyz;
            newParticle.oldTime = 0;
            output.Append(newParticle);
        }

        //output.Append(input[0]); // Emitter

    }
    else
    {
        if (input[0].time <= 4.0f)
        {
			input[0].oldTime = oldTime;
            output.Append(input[0]);
        }
    }

}