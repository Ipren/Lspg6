
struct VSIn {
	float2 Pos : POSITION;
	float2 Uv : TEXCOORD;
};

struct VSOut {
	float4 Pos : SV_POSITION;
	float2 Uv : TEXCOORD;
};

VSOut VS(VSIn input)
{
	VSOut output;

	output.Pos = float4(input.Pos, 0.0, 1.0);
	output.Uv = input.Uv;

	return output;
}

Texture2D TargetTexture : register(t0);
Texture2D HDRTexture : register(t1);
Texture2D DistortTexture : register(t2);

SamplerState HDRSampler : register(s0);

#define TONEMAP_GAMMA 1.0 

// Reinhard Tonemapper
float4 tonemap_reinhard(in float3 color)
{
	color *= 16;
	color = color / (1 + color);
	float3 ret = pow(color, TONEMAP_GAMMA); // gamma
	return float4(ret, 1);
}

float4 CalcBrightness(float4 col)
{
	float lum = dot(col.rgb, float3(0.2126, 0.7152, 0.0722));
	if (lum > 1.0) {
		return col;
	}
	else {
		return 0.0;
	}
}

struct PSOut {
	float4 Color : SV_Target0;
	float4 Brightness : SV_Target1;
};

PSOut PS(VSOut input) : SV_Target
{
	PSOut output;
	//float4 target = TargetTexture.Sample(HDRSampler, input.Uv);
	float4 distort = DistortTexture.Sample(HDRSampler, input.Uv);
	float4 hdr = HDRTexture.Sample(HDRSampler, input.Uv + distort.xy * 0.1);

	output.Color = hdr;
	output.Brightness = CalcBrightness(hdr);

	return output;
}