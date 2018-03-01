#include <IBLHeader.hlsli>

TextureCube EnvMap			: register(t0);
SamplerState basicSampler	: register(s0);

cbuffer ExternalData : register(b0) {
	float roughness;
}
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 uvw			: TEXCOORD;
};

float4 main(VertexToPixel input) : SV_TARGET
{
	float3 normalVec = normalize(input.uvw);
	float3 R = normalVec;
	float3 viewDir = R;

	float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
	float totalWeight = 0.0f;
	
	const uint NumSamples = 1024;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 halfwayVec = ImportanceSampleGGX(Xi, roughness, normalVec);
		float3 lightDir = 2 * dot(viewDir, halfwayVec) * halfwayVec - viewDir;
		float NdotL = saturate(dot(normalVec, lightDir));
		if (NdotL > 0)
		{
			// sample from the environment's mip level based on roughness/pdf
			float D = NormalDistributionGGXTR(normalVec, halfwayVec, roughness);
			float NdotH = max(dot(normalVec, halfwayVec), 0.0f);
			float HdotV = max(dot(halfwayVec, viewDir), 0.0f);
			float pdf = D * NdotH / (4.0f * HdotV) + 0.0001f;

			float resolution = 512.0f; // resolution of source cubemap (per face)
			float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
			float saSample = 1.0f / (float(NumSamples) * pdf + 0.0001f);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			PrefilteredColor += EnvMap.SampleLevel(basicSampler , lightDir, 0).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	PrefilteredColor /= totalWeight;

	return float4(PrefilteredColor, 1.0f);
}