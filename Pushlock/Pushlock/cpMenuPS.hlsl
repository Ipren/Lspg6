struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};
SamplerState sSampler;
texture2D menu : register(t0);


float4 main(VS_OUT input) : SV_TARGET
{

    return menu.Sample(sSampler, input.uv);
}