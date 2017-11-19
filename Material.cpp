#include "Material.h"


Material::Material(SimplePixelShader* _pixelShader, SimpleVertexShader* _vertexShader, ID3D11ShaderResourceView* _materialSRV, ID3D11ShaderResourceView* _normalSRV, ID3D11SamplerState* _materialSampler)
{
	pixelShader = _pixelShader;
	vertexShader = _vertexShader;
	materialSRV = _materialSRV;
	normalSRV = _normalSRV;
	materialSampler = _materialSampler;
}

Material::~Material()
{
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

ID3D11ShaderResourceView * Material::GetMaterialSRV()
{
	return materialSRV;
}

ID3D11ShaderResourceView * Material::GetNormalSRV()
{
	return normalSRV;
}

ID3D11SamplerState * Material::GetMaterialSampler()
{
	return materialSampler;
}