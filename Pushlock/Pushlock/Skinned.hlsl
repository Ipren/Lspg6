#include "Common.hlsl"


cbuffer cbTest : register(b10)
{
	row_major float4x4 test;
};

cbuffer cbJointTransforms : register(b11)
{
	row_major float4x4 gBoneTransforms[96];
};


cbuffer Mat : register(b1) {
	float4 Color;
}

cbuffer dirLigth : register(b2)
{
	float3 dLightDirection;
	float4 dLightcolor;
}

cbuffer camPos : register(b3)
{
	float4 cameraPosition;
}

cbuffer shrinkBuffer : register(b6)
{
	float shrinkAmount;
};

cbuffer pLightCount : register(b4)
{
	uint nrOfPointLights;
}

StructuredBuffer<PointLight> pLights : register(t0);

	struct VS_IN
	{
		float3 pos			: POSITION;
		float3 nor			: NORMAL;
		float2 UV			: TEXCOORD;
		int4 boneIndices	: BONEINDICES;

		float4 weights		: WEIGHTS;
	};

//static const float3 normal = float3(0.0f, 1.0f, 0.0f);


struct VS_OUT
{
	float4 pos : SV_Position;
	float3 nor : NORMAL;
	float2 uv : UV;
	float4 wPos : POSITION;
};

VS_OUT VS(VS_IN input)
{
	VS_OUT output;

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
	float3 nor = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; i++)
	{
		if (input.boneIndices[i] >= 0) //unused bone indices are negative
		{
			//TODO: cbuffer gWorld
			position += weights[i] * mul(float4(input.pos, 1.0f),
				gBoneTransforms[input.boneIndices[i]]).xyz;
			position = float3(position.xyz);
			nor += weights[i] * mul(input.nor,
				(float3x3)gBoneTransforms[input.boneIndices[i]]);
		}
	}

	output.pos = mul(Proj, mul(View, mul(World, float4(position, 1.0f))));
	output.wPos = mul(World, float4(position, 1.0f));
	output.nor = mul(NormalMatrix, nor);
	output.nor = normalize(output.nor);

	//jeez..
	output.uv = input.UV;
	output.uv.y = 1 - input.UV.y;
	return output;

}

texture2D tex : register(t2);
SamplerState diffuseSampler : register(s2);

float4 PS(in VS_OUT input) : SV_TARGET
{
	float4 c = tex.Sample(diffuseSampler, input.uv);

	//return float4(input.nor.xyz, 1.0);
	float3 lightDir = normalize(dLightDirection);
	float3 diffuse = saturate(dot(input.nor.xyz, lightDir));
	//return float4(diffuse, 1.0);
	diffuse *= c.xyz * dLightcolor.xyz;

	float4 coords = mul(ShadowProj, mul(ShadowView, mul(ShadowWorld, input.wPos)));
	float shadow = GetShadow(input.nor, dLightDirection, coords);

	//float3 ambient = (c * 0.04f) + float3(0.04f, 0.04f, 0.04f);
	float3 ambient = float3(.15f, 0.0f, 0.0f);


	diffuse += CalcPointLights(pLights, input.wPos, input.nor, nrOfPointLights);

	// TODO: lägg i VS
	float d = distance(input.wPos.xz, float2(0, 0));
	float l = 0;
	float shrink = 10 - shrinkAmount;
	if (d > shrink && d < shrink + 0.2) return float4(5, 0.8, 0.4, 1.0);
	//if (d > shrink && d < (10 - shrinkAmount) + 0.2) l = 1;
	d = 1 - saturate((d - shrink) / (shrink + 3 - shrink));

	float3 col = diffuse + ambient + 0.2 * shadow;

	//return float4(lerp(col, float3(0.2, 0.2, 0.2), 1 - d), 1.0f);
	return float4(col, 1.0f);
}