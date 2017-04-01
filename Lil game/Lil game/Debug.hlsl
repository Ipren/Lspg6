cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
};

cbuffer Mat : register(b1) {
	float4 Color;
}

float4 VS(float3 pos : POSITION) : SV_POSITION
{
	return mul(Proj, mul(View, mul(World, float4(pos, 1.0))));
}

float4 PS(float4 pos : SV_POSITION) : SV_TARGET
{
	return Color;
}