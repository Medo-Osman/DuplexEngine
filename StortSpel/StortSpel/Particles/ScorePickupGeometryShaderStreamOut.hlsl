/*
    OBS!
    Denna kommer bara köras en gång.
    (Kan inte återskapa nya partiklar)
*/

struct particle
{
    float3 wPos : POSITION;
    float3 wVel : VEL;
    float2 wSize : SIZE;
    float time : TIME;
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



[maxvertexcount(10)]
void main(point particle input[1], inout PointStream<particle> output)
{
    const int MAX_PARTICLE_COUNT = 10;
    const int MIN_PARTICLE_COUNT = 4;

    input[0].time += g_dt;
    if (input[0].type == 0) //Emitter
    {

  
        float randomNumber = randomTexture.SampleLevel(textureSampler, g_gameTime, 0).x;  // Between -1 and 1
        /*                         |         0 - 6        |  (+4)    4 - 10   |                     */
        int nrOfParticlesToSpawn = 3 + (randomNumber * 3) + MIN_PARTICLE_COUNT;     // Between 4 and 10

        float3 randomPointAroundEmitter;
        float3 randomDirectionVectorFromEmitter;
        float2 randomSize;
        const float PARTICLE_VELOCITY = 3;

        for (int i = 0; i < nrOfParticlesToSpawn; i++)
        {
            randomNumber = randomTexture.SampleLevel(textureSampler, g_gameTime, 0).x; // Get a new random number for every new particle

            randomPointAroundEmitter = g_worldEmitPosition.xyz + randomNumber;
            randomDirectionVectorFromEmitter = normalize(randomPointAroundEmitter - g_worldEmitPosition.xyz);
            /*         |             0 - 1            |  0.5 - 1.5  |       */
            randomSize = 0.5f + (randomNumber * 0.5f) + 0.5f;
            randomSize.x = 1;

            particle newParticle;
            newParticle.wPos = g_worldEmitPosition.xyz;
            newParticle.wVel = randomDirectionVectorFromEmitter * PARTICLE_VELOCITY;
            newParticle.wSize = randomSize;
            newParticle.time = 0.f;
            newParticle.type = 1;
            output.Append(newParticle);
        }

        //output.Append(input[0]); // Emitter

    }
    else
    {
        if (input[0].time <= 4.0f)
        {
            output.Append(input[0]);
        }
    }

}