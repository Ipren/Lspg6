cbuffer Camera : register(b0) {
	float4x4 World;
	float4x4 View;
	float4x4 Proj;
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

static const float3 normal = float3(0.0f, 1.0f, 0.0f);

float4 VS(float3 pos : POSITION) : SV_POSITION
{
	return mul(Proj, mul(View, mul(World, float4(pos, 1.0))));
}

float4 PS(float4 pos : SV_POSITION) : SV_TARGET
{
    float4 c = Color;
    
    float3 diffuse = saturate(dot(-dLightDirection, normal));
    diffuse *= c.xyz * dLightcolor.xyz;

    float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);

    float attenuation = 1.0f;
    float3 P2L;
    float distance;
    float nDotL;
    for (int i = 0; i < nrOfPointLights; i++)
    {
        P2L = pLights[i].lightPos - pos.xyz;
        distance = length(P2L);
        if(distance < pLights[i].range)
        {
            attenuation = max(0, 1.0f - (distance / pLights[i].range));
            P2L /= distance;
            nDotL = saturate(dot(normal, P2L));
            diffuse *= nDotL * pLights[i].lightColor * attenuation;
        }
    }

    return float4(diffuse + ambient, 1.0f);
}