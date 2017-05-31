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


cbuffer cbJointTransforms : register(b11)
{
	row_major float4x4 gBoneTransforms[96];
};


cbuffer Mat : register(b1) {
	float4 Color;
}

struct SkinnedVS_IN
{
	float3 pos			: POSITION;
	float3 nor			: NORMAL;
	float2 UV			: TEXCOORD;
	int4 boneIndices	: BONEINDICES;

	float4 weights		: WEIGHTS;
};

//static const float3 normal = float3(0.0f, 1.0f, 0.0f);


struct SkinnedVS_OUT
{
	float4 pos : SV_Position;
};

SkinnedVS_OUT SkinnedVS(SkinnedVS_IN input)
{
	SkinnedVS_OUT output;

	//FRANK D. LUNA (p.781)

	//init array
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = input.weights.x;
	weights[1] = input.weights.y;
	weights[2] = input.weights.z;
	weights[3] = input.weights.w;

	//DEBUG WEIGHTS //
	float sum = float(0.0f);
	for (int i = 0; i < 4; i++)
	{
		if (input.boneIndices[i] >= 0)
			sum += weights[i];
	}
	//////////////////

	//Blend verts
	float3 position = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; i++)
	{
		if (input.boneIndices[i] >= 0) //unused bone indices are negative
		{
			//TODO: cbuffer gWorld
			position += weights[i] * mul(float4(input.pos, 1.0f),
				gBoneTransforms[input.boneIndices[i]]).xyz;
			position = float3(position.xyz);
		}
	}

	output.pos = mul(Proj, mul(View, mul(World, float4(position, 1.0f))));
	output.pos.z = (output.pos.z + output.pos.w) * 0.5;
	return output;
}
float PS(in VS_OUT input) : SV_TARGET
{
	return input.wPos.z / 30.0;
}