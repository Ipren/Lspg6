struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
	float age : TEXCOORD;
    int type : TYPE;
};

struct VS_OUT
{
    float3 position : POSITION;
	float age : TEXCOORD;
    int type : TYPE;
};

cbuffer Camera : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};

static const float4 quadCorners[4] =
{
    float4(-0.030f, 0.03f, 0.0f, 0.0f),
    float4(0.023f, 0.030f, 0.0f, 0.0f),
    float4(-0.030f, -0.030f, 0.0f, 0.0f),
    float4(0.030f, -0.030f, 0.0f, 0.0f)
};


static const float2 quadTexCoords[4] =
{
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 1.0f)
};

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> tStream)
{
    
    GS_OUT output;

    float4 pos = mul(float4(input[0].position, 1.0f), World);
    pos = mul(pos, View);
    pos = mul(pos, Proj);
    
    for (int i = 0; i < 4; i++)
    {
        output.pos = pos + quadCorners[i] * 10.f;
       
        output.uv = quadTexCoords[i] / 16.0;
		output.age = input[0].age;
        output.type = input[0].type;
        tStream.Append(output);
       
    }
}