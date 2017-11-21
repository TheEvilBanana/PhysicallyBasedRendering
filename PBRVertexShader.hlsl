#include <PBRHeader.hlsli>

cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

//struct VertexToPixel
//{
//	float4 position		: SV_POSITION;
//	float3 normal		: NORMAL;
//	//float3 tangent		: TANGENT;
//	float3 worldPos		: POSITION;
//	//float2 uv			: TEXCOORD;
//};

VertexToPixel main(VertexShaderInput input)
{
	VertexToPixel output;

	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	output.normal = mul(input.normal, (float3x3)world);

	output.worldPos = mul(float4(input.position, 1.0f), world).xyz;

	return output;
}