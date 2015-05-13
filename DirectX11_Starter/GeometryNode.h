#pragma once
#include "SceneNode.h"
#include <DirectXMath.h>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "OBB.h"

// Struct to match vertex shader's constant buffer
// You update one of these locally, then push it to the corresponding
// constant buffer on the device when it needs to be updated
struct VertexShaderConstantBuffer
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

//A geometry node will contain the information for a rendering geometry
class GeometryNode : public SceneNode
{
private:
	OBB* collider;

	int numIndices;
	ID3D11Buffer* cBuffer;
	VertexShaderConstantBuffer cBufferData;
	
	Mesh* mesh;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	Material* mat;
	ID3D11PixelShader* pixelShader; 
	ID3D11VertexShader* vertexShader;

	Camera* camera;

public:
	GeometryNode(Vertex* vertices, int numVerts, UINT* indices, int numIndices, 
		ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
		Material* mat=NULL, Camera* camera=NULL);
	GeometryNode(Mesh* nMesh, ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
		Material* mat=NULL, Camera* camera=NULL);
	~GeometryNode();
	virtual void Update(ID3D11DeviceContext* deviceContext) override;
	virtual void Draw(ID3D11DeviceContext* deviceContext) override;

	OBB* GetCollider(){return collider;};
};