struct Particle
{
    float3 position;
    float3 velocity;
    float age;
    int type;
};
StructuredBuffer<Particle> particles;

struct VS_OUT
{
    float3 position : POSITION;
    int type : TYPE;
};

VS_OUT main(uint vertexID : SV_VertexID)
{
    VS_OUT output;
	
    //creates the verticies with the vertexId as the index of current sim buffer
    output.position = particles[vertexID].position;
    output.type = particles[vertexID].type;
    
    return output;
}