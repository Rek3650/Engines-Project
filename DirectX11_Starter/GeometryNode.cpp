#include "GeometryNode.h"
#include <iostream>

//A Geometry node represents an object in 3d space
GeometryNode::GeometryNode(Vertex* vertices, int numVerts, UINT* indices, int numIndices, 
							ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
							Material* mat, Camera* camera)
{
	this->pixelShader = pixelShader;
	this->vertexShader = vertexShader;
	this->mat = mat;
	this->camera = camera;

	collider = new OBB();

	// set up the transformations and mesh
	this->numIndices = numIndices;
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	rotation = XMFLOAT4(0,0,0,1);
	translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mesh = new Mesh(vertices, numVerts, indices, numIndices, device);

	// Set up a dummy view matrix
	// will be updated every frame by the camera
	XMVECTOR position	= XMVectorSet(0, 0, 0, 0);
	XMVECTOR target		= XMVectorSet(0, 0, 1, 0);
	XMVECTOR up			= XMVectorSet(0, 1, 0, 0);
	XMMATRIX V			= XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	// Set up world matrix
	// update this every frame when the object moves
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W));

	// set up a dummy projection matrix
	// will be updated by the camera
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		((float)1600/900),
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

GeometryNode::GeometryNode(Mesh* nMesh, ID3D11Device* device, ID3D11PixelShader* pixelShader, ID3D11VertexShader* vertexShader, 
							Material* mat, Camera* camera)
{
	this->pixelShader = pixelShader;
	this->vertexShader = vertexShader;
	this->mat = mat;
	this->camera = camera;

	collider = new OBB();

	// set up the transformations and mesh
	scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	rotation = XMFLOAT4(0,0,0,1);
	translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mesh = nMesh;
	this->numIndices = mesh->GetNumInds();

	// Set up a dummy view matrix
	// will be updated every frame by the camera
	XMVECTOR position	= XMVectorSet(0, 0, 0, 0);
	XMVECTOR target		= XMVectorSet(0, 0, 1, 0);
	XMVECTOR up			= XMVectorSet(0, 1, 0, 0);
	XMMATRIX V			= XMMatrixLookAtLH(position, target, up);
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V));

	// Set up world matrix
	// update this every frame when the object moves
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W));

	// set up a dummy projection matrix
	// will be updated by the camera
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,
		((float)1600/900),
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

GeometryNode::~GeometryNode()
{
	delete mesh;
}

//Update objects logical data here.
//Should be reserved for geometry calculations
void GeometryNode::Update(ID3D11DeviceContext* deviceContext)
{
	// Update local constant buffer data
	cBufferData.world		= worldMatrix;
	if(camera != NULL)
	{
		cBufferData.view		= camera->GetView();
		cBufferData.projection	= camera->GetProjection();
	}
	else
	{
		cBufferData.view		= viewMatrix;
		cBufferData.projection	= projectionMatrix;
	}

	// play with the scale of the shape
	XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);
	XMMATRIX r = XMMatrixRotationQuaternion(XMLoadFloat4(&rotation));
	XMMATRIX t = XMMatrixTranslation(translation.x, translation.y, translation.z);

	//XMMATRIX w = r * t;
	//XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(w));
	

	XMMATRIX w = s * r * t;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(w));
	collider->Update(worldMatrix);

	// Update the constant buffer itself
	deviceContext->UpdateSubresource(
		cBuffer,
		0,			
		NULL,		
		&cBufferData,
		0,
		0);

	//Call super class's update function
	SceneNode::Update(deviceContext);
}

//Objects draw function
void  GeometryNode::Draw(ID3D11DeviceContext* deviceContext)
{
	//Draw Geometry here
	
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
	if(mat != NULL)
	{
		deviceContext->PSSetShaderResources(0, 1, &(mat->SRV));
		deviceContext->PSSetSamplers(0, 1, &(mat->sampState));
	}

	// Finally do the actual drawing
	deviceContext->DrawIndexed(
		numIndices,	// The number of indices we're using in this draw
		0,
		0);
	
	SceneNode::Draw(deviceContext);
}