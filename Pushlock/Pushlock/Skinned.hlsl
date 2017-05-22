cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
	float4x4 NormalMatrix;
};

cbuffer ShadowCamera : register(b5) {
	float4x4 ShadowWorld;
	float4x4 ShadowView;
	float4x4 ShadowProj;
};

cbuffer cbTest : register(b10)
{
	row_major float4x4 test;
};

cbuffer cbJointTransforms : register(b11)
{
	row_major float4x4 gBoneTransforms[96];
};

SamplerComparisonState ShadowSampler : register(s0);
Texture2D ShadowMap : register(t1);

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

cbuffer pLightCount : register(b4)
{
	uint nrOfPointLights;
}

struct pointLight
{
	float4 lightColor;
	float3 lightPos;
	float range;
};
StructuredBuffer<pointLight> pLights : register(t0);

	struct VS_IN
	{
		float3 pos			: POSITION;
		float3 nor			: NORMAL;
		float2 UV			: TEXCOORD;
		float4 weights		: WEIGHTS;
		int4 boneIndices	: BONEINDICES;
	};

//static const float3 normal = float3(0.0f, 1.0f, 0.0f);

float GetShadow(float3 normal, float4 coords)
{
	// orthographic..
	float3 proj = coords.xyz / coords.w;
	proj = proj * 0.5 + 0.5;
	proj.y = 1.0 - proj.y;

	// TODO: fixa
	float bias = 0;// max(0.05 * (1.0 - dot(normal, normalize(dLightDirection))), 0.00005);
	float shadowDepth = ShadowMap.SampleCmpLevelZero(ShadowSampler, proj.xy, proj.z + bias).r;

	return shadowDepth;
}


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
	input.UV.x += sum;
	output.uv = input.UV;
	return output;

}

float4 PS(in VS_OUT input) : SV_TARGET
{
	float4 c = Color;

	//return float4(input.nor.xyz, 1.0);
	float3 lightDir = normalize(dLightDirection);
	float3 diffuse = saturate(dot(input.nor.xyz, lightDir));
	//return float4(diffuse, 1.0);
	diffuse *= c.xyz * dLightcolor.xyz;

	float4 coords = mul(ShadowProj, mul(ShadowView, mul(ShadowWorld, input.wPos)));
	float shadow = 1 - GetShadow(input.nor, coords);

	float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);

	float attenuation = 1.0f;
	float3 P2L;
	float distance;
	float nDotL;
	float4 wLightPos;
	//float4 wNorm = float4(input.nor, 1.0f);
	for (uint i = 0; i < nrOfPointLights; i++)
	{
		wLightPos = float4(pLights[i].lightPos, 1.0f);
		P2L = wLightPos.xyz - input.wPos.xyz;
		distance = length(P2L);
		if (distance < pLights[i].range)
		{
			attenuation = saturate(1.0f - (distance / pLights[i].range));
			P2L /= distance;
			nDotL = saturate(dot(input.nor.xyz, P2L));

			//nDotL should be multiplied here but the light doesnt appear when you do : fix
			if (pLights[i].lightColor.w > 0)
			{
				diffuse += pLights[i].lightColor.xyz * attenuation;
			}
			else
			{
				diffuse *= pLights[i].lightColor.xyz * (1.0f - attenuation);
			}

		}
	}

	return float4(diffuse + ambient + 0.2 * shadow, 1.0f);
}