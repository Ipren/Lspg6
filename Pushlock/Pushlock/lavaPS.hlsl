#include "Common.hlsl"

texture2D mapTexture : register(t2);

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

cbuffer heatHaze : register(b6)
{
    float offset;
};

cbuffer deltaTime : register(b7)
{
    float dTime;
}

StructuredBuffer<PointLight> pLights : register(t0);

static const float3 normal = float3(0.0f, 1.0f, 0.0f);


struct VS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : POSITION;
    float4 viewPos : POSITION1;
    float2 uv : TEXCOORD;
};

float4 main(in VS_OUT input) : SV_TARGET
{

    
    float2 texcoord = input.uv;
    float distortion = sin(texcoord.y * offset + dTime * 0.0042f);
    distortion /= 938.0f;
    float4 c = mapTexture.Sample(ShadowSampler, float2(texcoord.x + distortion, texcoord.y));

    if(c.r > 0.27)
    {
        c.r += 0.52f;
    }
    
    float3 lightDir = normalize(dLightDirection);
    float3 diffuse = saturate(dot(normal, lightDir));
    diffuse *= c.xyz * dLightcolor.xyz;

    float4 coords = mul(ShadowProj, mul(ShadowView, mul(ShadowWorld, input.wPos)));
    float shadow = GetShadow(normal, dLightDirection, coords);
    float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);

    diffuse += CalcPointLights(pLights, input.wPos, normal, nrOfPointLights);

    return float4(diffuse * shadow, c.w);
}