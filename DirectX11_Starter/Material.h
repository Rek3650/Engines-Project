#pragma once

#include <d3d11.h>
#include "WICTextureLoader.h"
#include <DirectXMath.h>
#include "DXGame.h"

using namespace DirectX;

class Material
{
public:
	Material(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* file);
	~Material(void);

	ID3D11ShaderResourceView* SRV;
	ID3D11SamplerState* sampState;
};