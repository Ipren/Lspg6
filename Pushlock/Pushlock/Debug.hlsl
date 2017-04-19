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

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 wPos : POSITION;
};

VS_OUT VS(float3 pos : POSITION)
{
    VS_OUT output;
    output.pos = mul(Proj, mul(View, mul(World, float4(pos, 1.0))));
    output.wPos = mul(World, float4(pos, 1.0f));
    return output;

}

float4 PS(in VS_OUT input) : SV_TARGET
{
    float4 c = Color;
    
    float3 diffuse = saturate(dot(-dLightDirection, normal));
    diffuse *= c.xyz * dLightcolor.xyz;

    float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);

    float attenuation = 1.0f;
    float3 P2L;
    float distance;
    float nDotL;
    float4 wLightPos;
    for (uint i = 0; i < nrOfPointLights; i++)
    {
        wLightPos = mul(World, float4(pLights[i].lightPos, 1.0f)); 
        P2L =  wLightPos.xyz - input.wPos.xyz;
        distance = length(P2L);
        if(distance < pLights[i].range)
        {
            attenuation = saturate(1.0f - (distance / pLights[i].range));
            P2L /= distance;
            nDotL = saturate(dot(normal, P2L));

            //nDotL should be multiplied here but the light doesnt appear when you do : fix
            if(pLights[i].lightColor.w > 0)
            {
                diffuse += pLights[i].lightColor.xyz * attenuation;
            }
            else
            {
                diffuse *= pLights[i].lightColor.xyz * (1.0f - attenuation);
            }
           
        }
    }

    return float4(diffuse + ambient, 1.0f);
}