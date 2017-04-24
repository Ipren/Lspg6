struct VS_OUT
{
    float4 pos : SV_Position;
    float barNr : NR;
};

float4 main(in VS_OUT input) : SV_TARGET
{

    if(input.barNr < 0)
    {
        return float4(0.0f, 1.0f, 0.0f, 1.0f);
    }

    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}