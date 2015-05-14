#include "Primitives.h"


Primitives::Primitives(ID3D11Device* _device)
{
	device = _device;
}


Primitives::~Primitives(void)
{
}

// sets up the vertices and indices for a cube
// returns a newly allocated pointer to a GameEntity
GameEntity* Primitives::makeCube(ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
		Material* mat, Camera* camera, XMFLOAT4 color)
{
	Vertex verts[] =
	{
		{ XMFLOAT3(-0.5, 0.5, 0.5), color, XMFLOAT2(0, 0), XMFLOAT3(-1, 1, 1) },
		{ XMFLOAT3(0.5, 0.5, 0.5), color, XMFLOAT2(1, 0), XMFLOAT3(1, 1, 1) },
		{ XMFLOAT3(0.5, -0.5, 0.5), color, XMFLOAT2(1, 1), XMFLOAT3(1, -1, 1) },
		{ XMFLOAT3(-0.5, -0.5, 0.5), color, XMFLOAT2(0, 1), XMFLOAT3(-1, -1, 1) },
		{ XMFLOAT3(-0.5, 0.5, -0.5), color, XMFLOAT2(1, 0), XMFLOAT3(-1, 1, -1) },
		{ XMFLOAT3(0.5, 0.5, -0.5), color, XMFLOAT2(0, 0), XMFLOAT3(1, 1, -1) },
		{ XMFLOAT3(0.5, -0.5, -0.5), color, XMFLOAT2(0, 1), XMFLOAT3(1, -1, -1) },
		{ XMFLOAT3(-0.5, -0.5, -0.5), color, XMFLOAT2(1, 1), XMFLOAT3(-1, -1, -1) }
	};
	UINT inds[] =
	{
		3, 2, 1,
		3, 1, 0,
		2, 6, 5,
		2, 5, 1,
		6, 7, 4,
		6, 4, 5,
		7, 3, 0,
		7, 0, 4,
		0, 1, 5,
		0, 5, 4,
		7, 6, 2,
		7, 2, 3
	};

	return(new GameEntity(verts, 8, inds, 36, device, pixelShader, vertexShader, mat, camera));
}