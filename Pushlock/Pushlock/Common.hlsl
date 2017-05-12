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

float GetShadow(float3 normal, float3 lightDir, float4 coords)
{
	float3 proj = coords.xyz / coords.w;
	proj = proj * 0.5 + 0.5;
	proj.y = 1.0 - proj.y;

	float bias = max(0.0005 * (1.0 - dot(normal, normalize(lightDir))), 0.0005);
	float shadow = 0;
	
	[unroll]
	for (int x = -1; x <= 1; ++x) {
		[unroll]
		for (int y = -1; y <= 1; ++y) {
			float shadowDepth = ShadowMap.Sample(ShadowSampler, proj.xy, int2(x, y)).r;
			if (shadowDepth - bias < proj.z)
				shadow += 1;
		}
	}

	return 1 - (shadow / 9);
}

struct PointLight
{
	float4 lightColor;
	float3 lightPos;
	float range;
};

float CalcPointLights(StructuredBuffer<PointLight> lights, float4 worldpos, float3 normal, uint count)
{
	float diffuse = 0;

	for (uint i = 0; i < count; i++) {
		float4 wLightPos = float4(lights[i].lightPos, 1.0f);
		float3 P2L = wLightPos.xyz - worldpos;
		float distance = length(P2L);
		
		if (distance < lights[i].range) {
			float attenuation = saturate(1.0f - (distance / lights[i].range));
			P2L /= distance;
			float nDotL = saturate(dot(normal, P2L));
			
			if (lights[i].lightColor.w > 0) {
				diffuse += lights[i].lightColor.xyz * attenuation;
			} else {
				diffuse *= lights[i].lightColor.xyz * (1.0f - attenuation);
			}

		}
	}

	return diffuse;
}