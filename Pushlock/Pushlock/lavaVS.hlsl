struct VS_IN
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : POSITION;
    float4 viewPos : POSITION1;
    float2 uv : TEXCOORD;
};

cbuffer Camera : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};


VS_OUT main(VS_IN input)
{
    VS_OUT output;

    output.pos = mul(Proj, mul(View, input.pos));
    output.wPos = mul(World, input.pos);
    output.viewPos = mul(View, mul(World, input.pos));
    output.uv = input.uv;
    return output;
}