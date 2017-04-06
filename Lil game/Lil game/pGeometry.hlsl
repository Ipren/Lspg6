struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
    int type : TYPE;
};

struct VS_OUT
{
    float3 position : POSITION;
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
    float4(-0.026f, 0.026f, 0.0f, 0.0f),
    float4(0.026f, 0.026f, 0.0f, 0.0f),
    float4(-0.026f, -0.026f, 0.0f, 0.0f),
    float4(0.026f, -0.026f, 0.0f, 0.0f)
};


static const float2 quadTexCoords[4] =
{
    float2(1.0f, 0.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 1.0f),
    float2(0.0f, 1.0f)
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
        output.pos = pos + quadCorners[i];
       
        output.uv = quadTexCoords[i];
        output.type = input[0].type;
        tStream.Append(output);
       
    }

    //float4 guadCorners[4];
    //guadCorners[0] = float4(input[0].position + float3(10.0f, -10.0f, 0.0f), 1.0f);
    //guadCorners[1] = float4(input[0].position + float3(10.0f, 10.0f, 0.0f), 1.0f);
    //guadCorners[2] = float4(input[0].position + float3(-10.0f, -10.0f, 0.0f), 1.0f);
    //guadCorners[3] = float4(input[0].position + float3(-10.0f, 10.0f, 0.0f), 1.0f);

    //GS_OUT output;
    //[unroll]
    //for (int i = 0; i < 4; i++)
    //{
    //    output.pos = mul(guadCorners[i], World);
    //    output.pos = mul(guadCorners[i], View);
    //    output.pos = mul(guadCorners[i], Proj);
    //    output.uv = quadTexCoords[i];
    //    output.type = input[0].type;

    //    tStream.Append(output);
    //}

}