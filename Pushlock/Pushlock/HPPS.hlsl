struct VS_OUT
{
    float4 pos : SV_Position;
    float barNr : NR;
};

cbuffer hp : register(b0)
{
    float HP;
}



float4 main(in VS_OUT input) : SV_TARGET
{

    if(input.barNr < 0)
    {
        float4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}