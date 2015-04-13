#include "Material.h"

Material::Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* file)
{
	ID3D11SamplerState* sampState;
	ID3D11ShaderResourceView* SRV;
	D3D11_SAMPLER_DESC sampDesc;
	sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MaxAnisotropy	= 1;
	sampDesc.MipLODBias		= 0;
	sampDesc.MaxLOD			= FLT_MAX;
	sampDesc.MinLOD			= -FLT_MAX;
	HR(device->CreateSamplerState(
		&sampDesc, 
		&sampState));
	HR(CreateWICTextureFromFile(
		device, 
		deviceContext, 
		file, 
		0, 
		&SRV));

	// set the sampler state
	this->sampState = sampState;

	// set the shader resource view
	this->SRV = SRV;
}


Material::~Material(void)
{
}
