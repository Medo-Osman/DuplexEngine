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

cbuffer playerLineBuffer : register(b1)
{
	float3 playerVelocity;
}

cbuffer rightOffset : register(b2)
{
    float3 rightVector;
    bool running;
}


SamplerState textureSampler : register(s0);
Texture1D randomTexture : register(t0);

[maxvertexcount(3)]
void main(point particle input[1], inout PointStream<particle> output)
{
    input[0].time += g_dt;
    float3 rightOffset = rightVector / 6.5f;
	if (input[0].type == 1) //Emitter
	{
        input[0].wOldPos = input[0].wPos;
        input[0].wPos = g_worldEmitPosition.xyz;
		if(running)
        {
            float3 offset = float3(0.f, 0.4f, 0.f);
		

            float y = cos(input[0].time * 3.f);
            offset.y = offset.y + (y * 0.25f);
            for (int i = 0; i < 2; i++)
            {
                particle newParticle;
                newParticle.wPos = g_worldEmitPosition.xyz + (offset) + rightOffset;
                newParticle.wVel = float3(0.f, 0.f, 0.f);
                newParticle.wSize = float2(0.2f, 0.2f);
                newParticle.time = 0.f;
                newParticle.type = 2;
                newParticle.wOldPos = input[0].wOldPos + offset + rightOffset;
                newParticle.oldTime = 0.f;
                output.Append(newParticle);
                rightOffset = rightOffset * -1.f;
            }
        }

        output.Append(input[0]); // Emitter

	}
	else
	{
		if (input[0].time <= 0.15f)
		{
			output.Append(input[0]);
		}
	}

}