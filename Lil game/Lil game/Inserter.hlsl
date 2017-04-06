struct Particle
{
    float3 position;
    float3 velocity;
    float age;
    int type;
};

AppendStructuredBuffer<Particle> particleBuffer : register(u0);
struct el
{
    float4 randomVector;
    float3 ePosition;
    int eType;
};

cbuffer emitterLocation : register(b0)
{
    el eLocations[2048];
}
cbuffer particleCount : register(b1)
{
    uint pCount;
}
cbuffer emitterCount : register(b2)
{
    int eCount;
}


static const float3 reflectVectors[6] =
{
    float3(1.0f, 0.0f, 0.0f),
    float3(-1.0f, 0.0f, 0.0f),
    float3(0.0f, -1.0f, 1.0f),
    float3(0.0f, -1.0f, 0.0f),
    float3(0.0f, 0.0f, 1.0f),
    float3(0.0f, 0.0f, -1.0f), 
};

[numthreads(6, 1, 1)]
void main(uint3 GTID : SV_GroupThreadID)
{
    if (pCount <= 524282)
    {
        for (int i = 0; i < eCount; i++)
        {
            Particle newParticle;

            newParticle.age = 0.0f;
            newParticle.position = eLocations[i].ePosition;
            newParticle.type = eLocations[i].eType;
            newParticle.velocity = reflect(eLocations[i].randomVector.xyz, reflectVectors[GTID.x]);
            if(sign(newParticle.velocity.y) == -1)
            {
                newParticle.velocity.y *= -1.0f; 
            }

            particleBuffer.Append(newParticle);
        }
    }

}