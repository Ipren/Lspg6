struct Particle
{
    float3 position;
    float3 velocity;
    float age;
    int type;
};
ConsumeStructuredBuffer<Particle> CurrentSimState : register(u0);
AppendStructuredBuffer<Particle> nextSimState : register(u1);

cbuffer particleCount : register(b0)
{
    uint pCount;
}
cbuffer deltaTime : register(b1)
{
    float dTime;
}

[numthreads(1024, 1, 1)]
void main(uint3 DTID : SV_DispatchThreadID)
{

     //flatten id
    uint threadID = DTID.x + DTID.y * 512 + DTID.z * 512 * 512;

    //check if this thread has a particle to update
    if (threadID < pCount)
    {
        Particle currentParticle = CurrentSimState.Consume();
        
        currentParticle.age += dTime;

        if (currentParticle.age < 15.0f)
        {
            currentParticle.position.xyz += float3(currentParticle.velocity * dTime * 0.01);
            nextSimState.Append(currentParticle);
        }
    }
}