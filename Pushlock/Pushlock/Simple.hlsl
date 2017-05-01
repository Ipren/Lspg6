cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
};

cbuffer Mat : register(b1) {
	float4 Color;
}
struct VS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : POSITION;
	float4 viewPos : POSITION1;
};

VS_OUT VS(float3 pos : POSITION)
{
    VS_OUT output;
    output.pos = mul(Proj, mul(View, float4(pos, 1.0)));
    output.wPos = mul(World, float4(pos, 1.0f));
    output.viewPos = mul(View, mul(World, float4(pos, 1.0f)));
    return output;
}

float4 PS(VS_OUT input) : SV_TARGET
{
	return Color;
}