#pragma once

#include <DirectXMath.h>


struct Vertex
{
	DirectX::XMFLOAT3 Position;	    // The position of the vertex
	DirectX::XMFLOAT2 UV;           // UV Coordinate for texturing (soon)
	DirectX::XMFLOAT3 Normal;       // Normal for lighting
	DirectX::XMFLOAT3 Tangent;		// For normal mapping
};