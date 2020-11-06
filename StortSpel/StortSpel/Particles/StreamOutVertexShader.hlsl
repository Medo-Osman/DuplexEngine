struct particle
{
    float3 wPos : POSITION;
    float3 wVel : VEL;
    float2 wSize : SIZE;
    float time : TIME;
    uint type : TYPE;
};

particle  main( particle passParticle )
{
    return passParticle;
}