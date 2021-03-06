struct VS_OUT
{
    float4 pos : SV_Position;
    float circleNr : NR;
};
cbuffer Camera : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};
VS_OUT main( float4 pos : POSITION )
{
    VS_OUT output;
    output.circleNr = pos.w;
    pos.w = 1.0f;
    output.pos = mul(Proj, mul(View, mul(World, pos)));
    return output;
}