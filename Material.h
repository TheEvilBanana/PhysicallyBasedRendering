#pragma once

#include "SimpleShader.h"

class Material
{
public:
	Material(ID3D11ShaderResourceView* albedoSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* metallicSRV, ID3D11ShaderResourceView* roughSRV, ID3D11SamplerState* materialSampler);
	~Material();
	/*SimplePixelShader* GetPixelShader();
	SimpleVertexShader* GetVertexShader();*/
	ID3D11ShaderResourceView* GetAlbedoSRV();
	ID3D11ShaderResourceView* GetNormalSRV();
	ID3D11ShaderResourceView* GetMetallicSRV();
	ID3D11ShaderResourceView* GetRoughSRV();
	ID3D11SamplerState* GetMaterialSampler();

private:
	/*SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;*/
	ID3D11ShaderResourceView* albedoSRV;
	ID3D11ShaderResourceView* normalSRV;
	ID3D11ShaderResourceView* metallicSRV;
	ID3D11ShaderResourceView* roughSRV;
	ID3D11SamplerState* materialSampler;
};


