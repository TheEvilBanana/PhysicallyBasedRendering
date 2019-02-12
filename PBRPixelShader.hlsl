#include <PBRHeader.hlsli>

TextureCube skyIR			: register(t0);
TextureCube skyPrefilter	: register(t1);
Texture2D brdfLUT			: register(t2);

SamplerState basicSampler	: register(s0);

cbuffer ExternalData : register(b0) {
	float3 albedo;
	float metallic;
	float roughness;
	float ao;

	float3 lightPos1;
	float3 lightPos2;
	float3 lightPos3;
	float3 lightPos4;
	float3 lightCol;

	float3 camPos;
};

void CalcRadiance(VertexToPixel input, float3 viewDir, float3 normalVec, float3 lightPos, float3 lightCol, float3 F0, out float3 rad)
{
	static const float PI = 3.14159265359;

	//calculate light radiance
	float3 lightDir = normalize(lightPos - input.worldPos);
	float3 halfwayVec = normalize(viewDir + lightDir);
	float distance = length(lightPos - input.worldPos);
	float attenuation = 1.0f / dot(float3(1.0f, 0.0f, 1.0f), float3(1.0f, distance, distance*distance));
	float3 radiance = lightCol * attenuation;

	//Cook-Torrance BRDF
	float D = NormalDistributionGGXTR(normalVec, halfwayVec, roughness);
	float G = GeometrySmith(normalVec, viewDir, lightDir, roughness);
	float3 F = FresnelSchlick(max(dot(halfwayVec, viewDir), 0.0f), F0);

	float3 kS = F;
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= (1.0 - metallic);

	float3 nom = D * G * F;
	float denom = 4 * max(dot(normalVec, viewDir), 0.0f) * max(dot(normalVec, lightDir), 0.0) + 0.001f; // 0.001f just in case product is 0
	float3 specular = nom / denom;

	//Add to outgoing radiance Lo
	float NdotL = max(dot(normalVec, lightDir), 0.0f);
	rad = (((kD * albedo / PI) + specular) * radiance * NdotL);
}

float4 main(VertexToPixel input) : SV_TARGET
{
	//static const float PI = 3.14159265359;

	float3 viewDir = normalize(camPos - input.worldPos);
	float3 normalVec = normalize(input.normal);
	float3 R = reflect(-viewDir, normalVec);

	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

	float3 rad = float3(0.0f, 0.0f, 0.0f);
	//reflectance equation
	float3 Lo = float3(0.0f, 0.0f, 0.0f);

	CalcRadiance(input, viewDir, normalVec, lightPos1, lightCol, F0, rad);
	Lo += rad;

	CalcRadiance(input, viewDir, normalVec, lightPos2, lightCol, F0, rad);
	Lo += rad;

	CalcRadiance(input, viewDir, normalVec, lightPos3, lightCol, F0, rad);
	Lo += rad;

	CalcRadiance(input, viewDir, normalVec, lightPos4, lightCol, F0, rad);
	Lo += rad;

	
	float3 kS = FresnelSchlickRoughness(max(dot(normalVec, viewDir), 0.0f), F0, roughness);
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= 1.0 - metallic;

	float3 irradiance = skyIR.Sample(basicSampler, normalVec).rgb;
	float3 diffuse = albedo * irradiance;

	const float MAX_REF_LOD = 4.0f;
	float3 prefilteredColor = skyPrefilter.SampleLevel(basicSampler, R, roughness * MAX_REF_LOD).rgb;
	float2 brdf = brdfLUT.Sample(basicSampler, float2(max(dot(normalVec, viewDir), 0.0f), roughness)).rg;
	float3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

	float3 ambient = (kD * diffuse + specular) * ao;
	float3 color = ambient + Lo;


	color = color / (color + float3(1.0f, 1.0f, 1.0f));
	color = pow(color, float3(1.0f / 2.2f, 1.0f / 2.2f, 1.0f / 2.2f));
	
	return float4(color, 1.0f);
	
}