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
    float3 pos : POSITION;
    float3 nor : NORMAL;
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
    output.pos = mul(Proj, mul(View, mul(World, float4(input.pos, 1.0))));
    output.wPos = mul(World, float4(input.pos, 1.0f));
    output.nor = mul(NormalMatrix, float4(input.nor, 1.0f));
    output.nor = normalize(output.nor);
	output.uv = float2(0, 0);
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
		float3 lightDir = normalize(wLightPos.xyz - input.wPos.xyz);
		float diff = max(dot(input.nor.xyz, lightDir), 0.0);
		float distance = length(wLightPos.xyz - input.wPos.xyz);
		float attenuation = 1.0f / (1 + 0.99 * distance + 0.92 * (distance * distance));    
		diffuse += pLights[i].lightColor * diff * attenuation;
    }

    return float4(diffuse + ambient + 0.2 * shadow, 1.0f);
}