#include "GameEntity.h"

using namespace DirectX;

GameEntity::GameEntity(Vertex* vertices, int numVerts, UINT* indices, int numIndices, ID3D11Device* device, Material* mat)
{
	this->mat = mat;

	// set up the transformations and mesh
	this->numIndices = numIndices;
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	//rotation = 0.0f;
	rotation = DirectX::XMQuaternionIdentity();
	translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mesh = new Mesh(vertices, numVerts, indices, numIndices, device);

	// Set up view matrix (camera)
	// In an actual game, update this when the camera moves (so every frame)
	XMVECTOR position	= XMVectorSet(0, 0, -5, 0);
	XMVECTOR target		= XMVectorSet(0, 0, 0, 0);
	XMVECTOR up			= XMVectorSet(0, 1, 0, 0);
	XMMATRIX V			= XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	// Set up world matrix
	// In an actual game, update this when the object moves (so every frame)
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W));

	// set up projection matrix with hard-coded value for now, needs to be changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		((float)800/600),
		0.1f,
		100.0f);
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P));

	// set up constant buffer
	D3D11_BUFFER_DESC cBufferDesc;
	cBufferDesc.ByteWidth			= sizeof(cBufferData);
	cBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	cBufferDesc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags		= 0;
	cBufferDesc.MiscFlags			= 0;
	cBufferDesc.StructureByteStride = 0;
	HR(device->CreateBuffer(
		&cBufferDesc,
		NULL,
		&cBuffer));
}

GameEntity::~GameEntity(void)
{
	delete mesh;
}

// updates mesh transforms, called once every frame
void GameEntity::Update(ID3D11DeviceContext* deviceContext)
{
	// Update local constant buffer data
	cBufferData.world		= worldMatrix;
	cBufferData.view		= viewMatrix;
	cBufferData.projection	= projectionMatrix;

	// play with the scale of the shape
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX r = XMMatrixRotationQuaternion(rotation);
	//XMMATRIX r = XMMatrixRotationX(rotation);
	XMMATRIX t = XMMatrixTranslation(translation.x, translation.y, translation.z);
	XMMATRIX w = s * r * t;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(w));

	// Update the constant buffer itself
	deviceContext->UpdateSubresource(
		cBuffer,
		0,			
		NULL,		
		&cBufferData,
		0,
		0);
}

// draws the game entity to the screen
void GameEntity::Draw(ID3D11DeviceContext* deviceContext, ID3D11PixelShader* pixelShader, 
		ID3D11VertexShader* vertexShader)
{
	// Set buffers in the input assembler
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &(mesh->vertexBuffer), &stride, &offset);
	deviceContext->IASetIndexBuffer(mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the current vertex and pixel shaders, as well the constant buffer for the vert shader
	deviceContext->VSSetShader(vertexShader, NULL, 0);
	deviceContext->VSSetConstantBuffers(
		0,	// Corresponds to the constant buffer's register in the vertex shader
		1, 
		&cBuffer);

	deviceContext->PSSetShader(pixelShader, NULL, 0);
	
	// set up the texture
	deviceContext->PSSetShaderResources(0, 1, &(mat->SRV));
	deviceContext->PSSetSamplers(0, 1, &(mat->sampState));

	// Finally do the actual drawing
	deviceContext->DrawIndexed(
		numIndices,	// The number of indices we're using in this draw
		0,
		0);
}