cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
};

float4 VS(float3 pos : POSITION) : SV_POSITION
{
	return mul(Proj, mul(View, float4(pos, 1.0)));
}

float4 PS(float4 pos : SV_POSITION) : SV_TARGET
{
	return float4(float3(0.2, 0.4, 0.6), 1.0);
}