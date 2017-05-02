cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
};

struct VS_IN
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_Position;
	float4 wPos : POSITION;
};

VS_OUT VS(VS_IN input)
{
	VS_OUT output;
	output.pos = mul(Proj, mul(View, mul(World, float4(input.pos, 1.0))));
	output.pos.z = (output.pos.z + output.pos.w) * 0.5;
	output.wPos = mul(View, mul(World, float4(input.pos, 1.0f)));
	return output;

}

float PS(in VS_OUT input) : SV_TARGET
{
	return input.wPos.z / 30.0;
}