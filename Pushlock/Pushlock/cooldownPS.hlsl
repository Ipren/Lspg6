struct VS_OUT
{
    float4 pos : SV_Position;
    float circleNr : NR;
};

cbuffer cds : register(b0)
{
    uint4 cooldowns;
}



float4 main(in VS_OUT input) : SV_TARGET
{

    if (cooldowns[(float)input .circleNr] == 1)
    {
        return float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}