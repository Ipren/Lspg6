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
}

SamplerState ShadowSampler : register(s0);
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

float GetShadow(float3 normal, float4 coords)
{
	// orthographic..
	float3 proj = coords.xyz / coords.w;
	proj = proj * 0.5 + 0.5;
	proj.y = 1.0 - proj.y;

	// TODO: fixa
	float bias = max(0.0005 * (1.0 - dot(normal, normalize(dLightDirection))), 0.0005);
	float shadow = 0;

	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float shadowDepth = ShadowMap.Sample(ShadowSampler, proj.xy, int2(x, y)).r;
			if (shadowDepth - bias < proj.z)
				shadow += 1;
		}
	}

	return shadow / 9;
}

float4 PS(in VS_OUT input) : SV_TARGET
{
    float4 c = Color;
    
	float3 lightDir = normalize(dLightDirection);
	float3 diffuse = saturate(dot(normal, lightDir));
	//return float4(diffuse, 1.0);
	diffuse *= c.xyz * dLightcolor.xyz;

	float4 coords = mul(ShadowProj, mul(ShadowView, mul(ShadowWorld, input.wPos)));
	float shadow = 1-GetShadow(normal, coords);
	float3 ambient = c.xyz * float3(0.0f, 0.0f, 0.0f);

    float attenuation = 1.0f;
    float3 P2L;
    float distance;
    float nDotL;
    float4 wLightPos;
    float4 wNorm = float4(normal, 1.0f);
    for (uint i = 0; i < nrOfPointLights; i++)
    {
		wLightPos = float4(pLights[i].lightPos, 1.0f);
		P2L = wLightPos.xyz - input.wPos.xyz;
		distance = length(P2L);
		if (distance < pLights[i].range)
		{
			attenuation = saturate(1.0f - (distance / pLights[i].range));
			P2L /= distance;
			nDotL = saturate(dot(wNorm, P2L));

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