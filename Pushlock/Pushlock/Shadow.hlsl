cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 wPos : POSITION;
};

VS_OUT VS(float3 pos : POSITION)
{
	VS_OUT output;
	output.pos = mul(Proj, mul(View, mul(World, float4(pos, 1.0))));
	output.wPos =  mul(View, mul(World, float4(pos, 1.0f)));
	return output;

}

float PS(in VS_OUT input) : SV_TARGET
{
	return input.wPos.z / 30.0;
}