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

particle  main( particle passParticle )
{
    return passParticle;
}