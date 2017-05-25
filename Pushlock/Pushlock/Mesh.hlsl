#include "Common.hlsl"

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

cbuffer shrinkBuffer : register(b6)
{
	float shrinkAmount;
};

StructuredBuffer<PointLight> pLights : register(t0);

struct VS_IN
{
	float3 pos : POSITION;
	float3 nor : NORMAL;
};

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
	output.pos = mul(Proj, mul(View, mul(World, float4(input.pos, 1.0))));
	output.wPos = mul(World, float4(input.pos, 1.0f));
	output.nor = mul(NormalMatrix, float4(input.nor, 1.0f)).xyz;
	output.nor = normalize(output.nor);
	output.uv = float2(0, 0);
	return output;

}

float4 PS(in VS_OUT input) : SV_TARGET
{
	float4 c = Color;

	float3 lightDir = normalize(dLightDirection);
	float3 diffuse = saturate(dot(input.nor.xyz, lightDir));
	diffuse *= c.xyz * dLightcolor.xyz;

	float4 coords = mul(ShadowProj, mul(ShadowView, mul(ShadowWorld, input.wPos)));
	float shadow = GetShadow(input.nor, dLightDirection, coords);

	float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);
	diffuse += CalcPointLights(pLights, input.wPos, input.nor, nrOfPointLights);

	// TODO: lägg i VS
	float d = distance(input.wPos.xz, float2(0, 0));
	float l = 0;
	float shrink = 10 - shrinkAmount;
	if (d > shrink && d < shrink + 0.2) l = 1;
	//if (d > shrink && d < (10 - shrinkAmount) + 0.2) l = 1;
	d = 1 - saturate((d - shrink) / (shrink + 3 - shrink));

	float3 col = diffuse + float3(3, 0.4, 0.1) * l + ambient + 0.2 * shadow;

	return float4(lerp(col, float3(0.2, 0.2, 0.2), 1 - d), 1.0f);
}