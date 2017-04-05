struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
    int type : TYPE;
};
texture2D paricleTex : register(t0);
SamplerState sSampler : register(s0);


float4 main(in GS_OUT input) : SV_TARGET
{
    //return paricleTex.Sample(sSampler, input.uv);
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}