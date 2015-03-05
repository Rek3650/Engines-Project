#include "Material.h"

Material::Material(ID3D11ShaderResourceView* SRV, ID3D11SamplerState* sampState)
{
	// set the sampler state
	this->sampState = sampState;

	// set the shader resource view
	this->SRV = SRV;
}


Material::~Material(void)
{
}
