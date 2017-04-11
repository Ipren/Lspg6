struct GS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
	float age : TEXCOORD;
    int type : TYPE;
};
texture2D paricleTex : register(t0);
SamplerState sSampler : register(s0);


float4 main(in GS_OUT input) : SV_TARGET
{
	float age = (input.age-1.6)*(1.0/0.4);
	float start = 1.0 / 16.0 * floor(age * 16.0);
	float end = 1.0 / 16.0 * ceil(age * 16.0);

	float4 st = paricleTex.Sample(sSampler, float2(input.uv.x + start,  input.uv.y));
	float4 et = paricleTex.Sample(sSampler, float2(input.uv.x + end,  input.uv.y));

	float4 texel = lerp(st, et, (age-start)*16.0);
	if (texel.w <= 0.000001) {
		discard;
	}
    return texel;

    if(input.type == 0)
    {
        return float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    if (input.type == 1)
    {
        return float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    if (input.type == 2)
    {
        return float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
    if (input.type == 4)
    {
        return float4(1.0f, 1.0f, 0.0f, 1.0f);
    }
    if (input.type == 0)
    {
        return float4(1.0f, 0.0f, 1.0f, 1.0f);
    }
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}