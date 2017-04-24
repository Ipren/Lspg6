struct VS_OUT
{
    float4 pos : SV_Position;
    float barNr : NR;
};
cbuffer Camera : register(b0)
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};
cbuffer hp : register(b1)
{
    float HP;
}
VS_OUT main(float4 pos : POSITION)
{
    VS_OUT output;
    output.barNr = pos.w;
    
    pos.w = 1.0f;
    if(output.barNr < 0 && pos.x - 0.29f > 0.001)
    {
            pos.x *= HP;
        
    }

    output.pos = mul(Proj, mul(View, mul(World, pos)));
    return output;
}