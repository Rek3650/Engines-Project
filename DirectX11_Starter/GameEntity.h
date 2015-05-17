#pragma once

#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "GeometryNode.h"
#include "OBB.h"

class GameEntity
{
public:
	GameEntity(Vertex* vertices, int numVerts, UINT* indices, int numIndices, 
		ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
		Material* mat=NULL, Material* normalMap=NULL, Camera* camera=NULL);
	GameEntity(Mesh* mesh, ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
		Material* mat=NULL, Material* normalMap=NULL, Camera* camera=NULL);
	~GameEntity();

	GeometryNode* geometry;

	bool checkForCollisions;
	OBB* getCollider();

	void Draw(ID3D11DeviceContext* deviceContext);
	void Update(ID3D11DeviceContext* deviceContext);
	
	void Translation(DirectX::XMFLOAT3 nTransVec);
	void Rotation(DirectX::XMFLOAT4 nRotQuat);
	void Scale(DirectX::XMFLOAT3 nScaleVec);

	void Move(DirectX::XMFLOAT3 moveVec);
	void Rotate(DirectX::XMFLOAT4 rotQuat);
	void Rescale(DirectX::XMFLOAT2 scaleVec);

	void AddChild(GeometryNode* child);
};

