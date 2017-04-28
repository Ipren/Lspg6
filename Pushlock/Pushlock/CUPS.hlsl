struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    int index : INDEX;
};
SamplerState sSampler;
texture2D menu : register(t0);
texture2D p1 : register(t1);
texture2D p2 : register(t2);
texture2D p3 : register(t3);
texture2D p4 : register(t4);



float4 main(VS_OUT input) : SV_TARGET
{
    if(input.index == 0)
    {
        return p1.Sample(sSampler, input.uv);
        //return float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    if (input.index == 1)
    {
        return p2.Sample(sSampler, input.uv);
        //return float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    if (input.index == 2)
    {
        return p3.Sample(sSampler, input.uv);
        //return float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    if (input.index == 3)
    {
        return p4.Sample(sSampler, input.uv);
        //return float4(1.0f, 1.0f, 0.0f, 1.0f);
    }
    return menu.Sample(sSampler, input.uv);
    //return float4(1.0f, 1.0f, 1.0f, 1.0f);
}