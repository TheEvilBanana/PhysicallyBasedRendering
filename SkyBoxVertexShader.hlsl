
// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
	float3 position		: POSITION;
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

// Out of the vertex shader (and eventually input to the PS)
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 uvw			: TEXCOORD;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output
	VertexToPixel output;

	// Make a view matrix with NO translation
	matrix viewNoMovement = view;
	viewNoMovement._41 = 0;
	viewNoMovement._42 = 0;
	viewNoMovement._43 = 0;

	// Calculate output position
	matrix viewProj = mul(viewNoMovement, projection);
	output.position = mul(float4(input.position, 1.0f), viewProj);

	// Ensure our polygons are at max depth
	output.position.z = output.position.w;

	// Use the cube's vertex position as a direction in space
	// from the origin (center of the cube)
	output.uvw = input.position;

	return output;
}