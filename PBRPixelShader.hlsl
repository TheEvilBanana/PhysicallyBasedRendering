#include <PBRHeader.hlsli>

cbuffer ExternalData : register(b0) {
	float3 albedo;
	float metallic;
	float roughness;
	float ao;

	float3 lightPos;
	float3 lightCol;

	float3 camPos;
};

//struct VertexToPixel
//{
//	float4 position		: SV_POSITION;
//	float3 normal		: NORMAL;
//	//float3 tangent		: TANGENT;
//	float3 worldPos		: POSITION;
//	//float2 uv			: TEXCOORD;
//};
//
//
//float NormalDistributionGGXTR(float3 normalVec, float3 halfwayVec, float a)    // a = roughness
//{
//	static const float PI = 3.14159265359;
//
//	float a2 = a * a;   // a2 = a^2
//	float NdotH = max(dot(normalVec, halfwayVec), 0.0);     // NdotH = normalVec.halfwayVec
//	float NdotH2 = NdotH * NdotH;   // NdotH2 = NdotH^2
//	float nom = a2;
//	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
//	denom = PI * denom * denom;
//
//	return nom / denom;
//}
//
//
//float GeometrySchlickGGX(float NdotV, float k)  // k is a remapping of roughness based on direct lighting or IBL lighting
//{
//	float nom = NdotV;
//	float denom = NdotV * (1.0f - k) + k;
//
//	return nom / denom;
//}
//
//float GeometrySmith(float3 normalVec, float3 viewDir, float3 lightDir, float k)
//{
//	float NdotV = max(dot(normalVec, viewDir), 0.0f);
//	float NdotL = max(dot(normalVec, lightDir), 0.0f);
//	float ggx1 = GeometrySchlickGGX(NdotV, k);
//	float ggx2 = GeometrySchlickGGX(NdotL, k);
//
//	return ggx1 * ggx2;
//}
//
//float3 FresnelSchlick(float cosTheta, float3 F0)   // cosTheta is n.v and F0 is the base reflectivity
//{
//	return (F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f));
//}

float4 main(VertexToPixel input) : SV_TARGET
{
	static const float PI = 3.14159265359;

	float3 viewDir = normalize(camPos - input.worldPos);
	float3 normalVec = normalize(input.normal);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

	//reflectance equation
	float3 Lo = float3(0.0f, 0.0f, 0.0f);
	
	//calculate light radiance
	float3 lightDir = normalize(lightPos - input.worldPos);
	float3 halfwayVec = normalize(viewDir + lightDir);
	float distance = length(lightPos - input.worldPos);
	float attenuation = 1.0f / (distance * distance);
	float3 radiance = lightCol * attenuation;

	//Cook-Torrance BRDF
	float D = NormalDistributionGGXTR(normalVec, halfwayVec, roughness);
	float G = GeometrySmith(normalVec, viewDir, lightDir, roughness);
	float3 F = FresnelSchlick(max(dot(halfwayVec, viewDir), 0.0f), F0);

	float3 kS = F;
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= 1.0 - metallic;

	float3 nom = D * G * F;
	float denom = 4 * max(dot(normalVec, viewDir), 0.0f) * max(dot(normalVec, lightDir), 0.0) + 0.001f; // 0.001f just in case product is 0
	float3 specular = nom / denom;

	//Add to outgoing radiance Lo
	float NdotL = max(dot(normalVec, lightDir), 0.0f);
	Lo += (((kD * albedo / PI) + specular) * radiance * NdotL);


	float3 ambient = float3(0.03f, 0.03f, 0.03f) * albedo * ao;
	float3 color = ambient + Lo;


	color = color / (color + float3(1.0f, 1.0f, 1.0f));
	color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
	
	return float4(color, 1.0f);
}