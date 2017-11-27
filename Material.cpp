#include "Material.h"


Material::Material(ID3D11ShaderResourceView* _albedoSRV, ID3D11ShaderResourceView* _normalSRV, ID3D11ShaderResourceView* _metallicSRV, ID3D11ShaderResourceView* _roughSRV, ID3D11SamplerState* _materialSampler)
{
	
	albedoSRV = _albedoSRV;
	normalSRV = _normalSRV;
	metallicSRV = _metallicSRV;
	roughSRV = _roughSRV;
	materialSampler = _materialSampler;
}

Material::~Material()
{
}

//SimplePixelShader* Material::GetPixelShader()
//{
//	return pixelShader;
//}
//
//SimpleVertexShader* Material::GetVertexShader()
//{
//	return vertexShader;
//}

ID3D11ShaderResourceView * Material::GetAlbedoSRV()
{
	return albedoSRV;
}

ID3D11ShaderResourceView * Material::GetNormalSRV()
{
	return normalSRV;
}

ID3D11ShaderResourceView * Material::GetMetallicSRV()
{
	return metallicSRV;
}

ID3D11ShaderResourceView * Material::GetRoughSRV()
{
	return roughSRV;
}

ID3D11SamplerState * Material::GetMaterialSampler()
{
	return materialSampler;
}