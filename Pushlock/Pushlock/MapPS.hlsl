#include "Common.hlsl"

texture2D mapTexture : register(t2);
texture2D normalMap : register(t3);

cbuffer Mat : register(b1)
{
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

struct GS_OUT
{

    float4 pos : SV_Position;
    float4 wPos : POSITION;
    float4 viewPos : POSITION1;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
};

float3 normalMapping(in float3 tangent, in float3 bitangent, in float3 normal, in float3 mapNormal)
{
    //from tangent to worldspace
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float3 newNormal = mul(mapNormal, TBN);

    return newNormal;
}

float4 main(in GS_OUT input) : SV_TARGET
{
    float3 normal = float3(0.0f, 1.0f, 0.0f);
    float3 mapNormal = 2 * normalMap.Sample(ShadowSampler, input.uv) - 1;
    normal = float4(normalize(normalMapping(input.tangent, input.bitangent, normal, mapNormal)), 1.0f);

    float4 c = mapTexture.Sample(ShadowSampler, input.uv);
    //float w = c.w;
    
    float3 lightDir = normalize(dLightDirection);
    float3 diffuse = saturate(dot(normal, lightDir));
	//return float4(diffuse, 1.0);
    diffuse *= c.xyz * dLightcolor.xyz;

    float4 coords = mul(ShadowProj, mul(ShadowView, mul(ShadowWorld, input.wPos)));
    float shadow = GetShadow(normal, dLightDirection, coords);
    float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);

	diffuse += CalcPointLights(pLights, input.wPos, normal, nrOfPointLights);

    return float4(diffuse + ambient + 0.2 * shadow, c.w);
}