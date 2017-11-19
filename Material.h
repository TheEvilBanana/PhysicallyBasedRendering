#pragma once

#include "SimpleShader.h"

class Material
{
public:
	Material(SimplePixelShader* pixelShader, SimpleVertexShader* vertexShader, ID3D11ShaderResourceView* materialSRV, ID3D11ShaderResourceView* normalSRV, ID3D11SamplerState* materialSampler);
	~Material();
	SimplePixelShader* GetPixelShader();
	SimpleVertexShader* GetVertexShader();
	ID3D11ShaderResourceView* GetMaterialSRV();
	ID3D11ShaderResourceView* GetNormalSRV();
	ID3D11SamplerState* GetMaterialSampler();

private:
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
	ID3D11ShaderResourceView* materialSRV;
	ID3D11ShaderResourceView* normalSRV;
	ID3D11SamplerState* materialSampler;
};


