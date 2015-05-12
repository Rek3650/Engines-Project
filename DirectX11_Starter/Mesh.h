#pragma once

#include <DirectXMath.h>
#include "DXGame.h"

// vertex struct for triangles
struct Vertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT2 uv;
};

class Mesh
{
public:
	// Buffers to hold actual geometry
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	Vertex* verts;
	int numberVerts;
	int numInds;

	Mesh(Vertex* vertices, int numVerts, UINT* indices, int numIndices, ID3D11Device* device);
	~Mesh(void);

	Vertex* GetVerticies();
	int GetNumVerts();

	int GetNumInds();
};

