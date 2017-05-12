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

StructuredBuffer<PointLight> pLights : register(t0);

static const float3 normal = float3(0.0f, 1.0f, 0.0f);

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : POSITION;
	float4 viewPos : POSITION1;
};

VS_OUT VS(float3 pos : POSITION)
{
    VS_OUT output;
    output.pos = mul(Proj, mul(View, mul(World, float4(pos, 1.0))));
    output.wPos = mul(World, float4(pos, 1.0f));
    output.viewPos = mul(View, mul(World, float4(pos, 1.0f)));
    return output;

}

float4 PS(in VS_OUT input) : SV_TARGET
{
    float4 c = Color;
    
	float3 lightDir = normalize(dLightDirection);
	float3 diffuse = saturate(dot(normal, lightDir));
	diffuse *= c.xyz * dLightcolor.xyz;

	float4 coords = mul(ShadowProj, mul(ShadowView, mul(ShadowWorld, input.wPos)));
	float shadow = GetShadow(normal, dLightDirection, coords);
	float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);

	diffuse += CalcPointLights(pLights, input.wPos, normal, nrOfPointLights);

	return float4(diffuse + ambient + 0.2 * shadow, 1.0f);
}