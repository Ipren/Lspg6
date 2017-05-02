cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
};

struct VSOut {
	float3 WorldPos : POSITION;
	float4 Pos : SV_POSITION;
};

VSOut VS(float3 pos : POSITION)
{
	VSOut output;

	output.WorldPos = pos;
	output.Pos = mul(Proj, mul(View, float4(pos, 1.0)));

	return output;
}

SamplerState PlaneSampler : register(s0);
Texture2D PlaneTexture : register(t0);

float4 PS(VSOut input) : SV_TARGET
{
	return PlaneTexture.Sample(PlaneSampler, input.WorldPos.xz/4.f);
}