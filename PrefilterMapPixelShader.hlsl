#include <IBLHeader.hlsli>

float4 main() : SV_TARGET
{
	float3 normalVec = normalize(//TODO);
	float3 R = normalVec;
	float3 viewDir = R;

	float3 PrefilteredColor = float3(0.0f, 0.0f, 0.0f);
	float totalWieght = 0.0f;
	
	const uint NumSamples = 1024;
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 halfwayVec = ImportanceSampleGGX(Xi, roughness, normalVec);
		float3 lightDir = 2 * dot(viewDir, halfwayVec) * halfwayVec - viewDir;
		float NdotL = saturate(dot(normalVec, lightVec));
		if (NdotL > 0)
		{
			// sample from the environment's mip level based on roughness/pdf
			float D = NormalDistributionGGXTR(normalVec, halfwayVec, roughness);
			float NdotH = max(dot(normalVec, halfwayVec), 0.0);
			float HdotV = max(dot(halfwayVec, viewDir), 0.0);
			float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

			float resolution = 512.0; // resolution of source cubemap (per face)
			float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
			float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			PrefilteredColor += EnvMap.SampleLevel(EnvMapSampler , L, 0).rgb * NdotL;
			totalWeight += NoL;
		}
	}
	PrefilteredColor /= TotalWeight;
	return float4(PrefilteredColor, 1.0f);
}