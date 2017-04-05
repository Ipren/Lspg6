struct Particle
{
    float3 position;
    float3 velocity;
    float age;
    int type;
};

AppendStructuredBuffer<Particle> particleBuffer : register(u0);

cbuffer emitterLocation : register(b0)
{
    float4 randomVector;
    float3 ePosition;
    int eType;
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
    float3(0.0f, 1.0f, 0.0f),
    float3(0.0f, -1.0f, 0.0f),
    float3(0.0f, 0.0f, 1.0f),
    float3(0.0f, 0.0f, -1.0f), 
};

[numthreads(6, 1, 1)]
void main(uint3 GTID : SV_GroupThreadID)
{
    if(pCount <= 2040)
    {
        for (int i = 0; i < eCount; i++)
        {
            Particle newParicle;

            newParicle.age = 0.0f;
            newParicle.position = ePosition;
            newParicle.type = eType;
            newParicle.velocity = reflect(randomVector.xyz, reflectVectors[GTID.x]);

            particleBuffer.Append(newParicle);
        }
    }

}