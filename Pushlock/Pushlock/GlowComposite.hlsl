struct VSOut {
	float4 Pos : SV_POSITION;
	float2 Uv : TEXCOORD;
};

Texture2D Source : register(t0);
Texture2D Glow : register(t1);

SamplerState Sampler : register(s0);

float4 PS(VSOut input) : SV_Target
{
	float4 src = Source.Sample(Sampler, input.Uv);
	float4 glow = Glow.Sample(Sampler, input.Uv);

	return src + glow;
}