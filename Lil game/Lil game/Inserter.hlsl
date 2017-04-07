struct Particle
{
    float3 position;
    float3 velocity;
    float age;
    int type;
};

struct el
{
    float4 velocityVector;
    float3 ePosition;
    int eType;
};

AppendStructuredBuffer<Particle> particleBuffer : register(u0);
StructuredBuffer<el> emitters;


//cbuffer emitterLocation : register(b0)
//{
//    el eLocations[1024];
//}
cbuffer particleCount : register(b0)
{
    uint pCount;
}
cbuffer emitterCount : register(b1)
{
    int eCount;
}
cbuffer randomVector : register(b2)
{
    float4 rv;
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
    if (pCount <= 524282)
    {
        for (int i = 0; i < eCount; i++)
        {
            Particle newParticle;

            newParticle.age = 0.0f;
            newParticle.position = emitters[i].ePosition;
            newParticle.type = emitters[i].eType;

            //arcane
            if(emitters[i].eType == 0)
            {
            //newParticle.velocity = reflect(emitters[i].velocityVector.xyz, reflectVectors[GTID.x]);
                newParticle.velocity = reflect(rv.xyz, reflectVectors[GTID.x]);
                newParticle.velocity += emitters[i].velocityVector;
                if (sign(newParticle.velocity.y) == -1)
                {
                    newParticle.velocity.y *= -1.0f;
                }
            }

            particleBuffer.Append(newParticle);
        }
    }

}