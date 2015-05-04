#include "GameEntity.h"

using namespace DirectX;

GameEntity::GameEntity(Vertex* vertices, int numVerts, UINT* indices, int numIndices, 
					   ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
					   Material* mat, Camera* camera)
{
	collider = new OBB(vertices, numVerts);
	checkForCollisions = true;
	geometry = new GeometryNode(vertices, numVerts, indices, numIndices, device, pixelShader, vertexShader, mat, camera);
}

GameEntity::~GameEntity(void)
{
	//delete geometry;
}

void GameEntity::Translation(DirectX::XMFLOAT3 nTransVec)
{
	geometry->SetTranslation(nTransVec);

	collider->setPosition(geometry->GetPosition());
}

void GameEntity::Rotation(DirectX::XMFLOAT4 nRotQuat)
{
	geometry->SetRotation(nRotQuat);

	collider->setRotation(geometry->GetRotation());
}

void GameEntity::Scale(DirectX::XMFLOAT3 nScaleVec)
{
	geometry->SetScale(nScaleVec);

	collider->setScale(geometry->GetScale());
}

void GameEntity::AddChild(GeometryNode* child)
{
	geometry->AddChildren(child);
}

void GameEntity::Move(DirectX::XMFLOAT3 moveVec)
{
	geometry->SetTranslation(DirectX::XMFLOAT3(
		geometry->GetPosition().x + moveVec.x,
		geometry->GetPosition().y + moveVec.y,
		geometry->GetPosition().z + moveVec.z
		));

	collider->setPosition(geometry->GetPosition());
}

// updates mesh transforms, called once every frame
void GameEntity::Update(ID3D11DeviceContext* deviceContext)
{
	geometry->Update(deviceContext);
}

// draws the game entity to the screen
void GameEntity::Draw(ID3D11DeviceContext* deviceContext)
{
	geometry->Draw(deviceContext);
}

