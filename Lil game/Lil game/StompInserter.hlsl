struct Particle
{
    float3 position;
    float3 velocity;
    float age;
    int type;
};
AppendStructuredBuffer<Particle> particleBuffer : register(u0);
cbuffer PlayerPos : register(b0)
{
    float3 pPos;
}
cbuffer CpuParticles : register(b1)
{
    Particle cpuP[50];
}
cbuffer particleCount : register(b2)
{
    uint pCount;
}


[numthreads(50, 1, 1)]
void main(uint3 GTID : SV_GroupThreadID)
{
    if (pCount <= 524282)
    {
        particleBuffer.Append(cpuP[GTID.x]);
    }

}