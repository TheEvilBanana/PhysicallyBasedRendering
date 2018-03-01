#include <IBLHeader.hlsli>

float2 main() : SV_TARGET
{
	//float2 IntegrateBRDF(float NdotV, float Roughness)

	float3 viewDir;
	viewDir.x = sqrt(1.0f - NdotV * NdotV); // sin
	viewDir.y = 0;
	viewDir.z = NdotV; // cos

	float A = 0;
	float B = 0;

	float3 normalVec = float3(0.0f, 0.0f, 1.0f);

	const uint NumSamples = 1024;
	// generates a sample vector that's biased towards the
	// preferred alignment direction (importance sampling)
	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 halfwayVec = ImportanceSampleGGX(Xi, Roughness, normalVec);
		float3 lightDir = 2 * dot(viewDir, halwayVec) * halwayVec - viewDir;

		float NdotL = saturate(lightDir.z);
		float NdotH = saturate(halwayVec.z);
		float VdotH = saturate(dot(viewDir, halwayVec));

		if (NdotL > 0)
		{
			float G = IBLGeometrySmith(normalVec, viewDir, lightDir, Roughness);
			float G_Vis = G * VdotH / (NdotH * NdotV);
			float Fc = pow(1 - VdotH, 5);

			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	return float2(A, B) / NumSamples;

	
}