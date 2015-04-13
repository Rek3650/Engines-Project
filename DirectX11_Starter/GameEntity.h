#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

// Struct to match vertex shader's constant buffer
// You update one of these locally, then push it to the corresponding
// constant buffer on the device when it needs to be updated
struct VertexShaderConstantBuffer
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

class GameEntity
{
private:
	int numIndices;
	ID3D11Buffer* cBuffer;
	VertexShaderConstantBuffer cBufferData;

public:
	Mesh* mesh;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotation;
	DirectX::XMFLOAT3 translation;
	Material* mat;
	ID3D11PixelShader* pixelShader; 
	ID3D11VertexShader* vertexShader;
	Camera* camera;

	GameEntity(Vertex* vertices, int numVerts, UINT* indices, int numIndices, 
		ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
		Material* mat=NULL, Camera* camera=NULL);

	~GameEntity(void);

	void Draw(ID3D11DeviceContext* deviceContext);
	void Update(ID3D11DeviceContext* deviceContext);
};

