#pragma once

#include <d3d11.h>
#include "WICTextureLoader.h"

class Material
{
public:
	Material(ID3D11ShaderResourceView* SRV, ID3D11SamplerState* sampState);
	~Material(void);

	ID3D11ShaderResourceView* SRV;
	ID3D11SamplerState* sampState;
};