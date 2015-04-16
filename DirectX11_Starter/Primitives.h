#pragma once

#include "GameEntity.h"
#include <DirectXMath.h>
#include "Mesh.h"

using namespace DirectX;

class Primitives
{
public:
	Primitives(ID3D11Device* _device);
	~Primitives(void);
	
	GameEntity* makeCube(ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
		Material* mat=NULL, Camera* camera=NULL, XMFLOAT4 color=XMFLOAT4(0, 0, 0, 1));

private:
	ID3D11Device* device;
};

