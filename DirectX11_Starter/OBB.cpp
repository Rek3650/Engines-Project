#include "OBB.h"
#include <stdlib.h>

OBB::OBB(Vertex* geomVertices, int numVerts)
{
	// set up the position and axis
	centerPos = XMFLOAT3(0, 0, 0);
	xAxis = XMFLOAT3(1, 0, 0);
	yAxis = XMFLOAT3(0, 1, 0);
	zAxis = XMFLOAT3(0, 0, 1);

	// set up the vertices
	XMFLOAT3 left = getLeftmostPoint(geomVertices, numVerts);
	XMFLOAT3 right = getRightmostPoint(geomVertices, numVerts);
	XMFLOAT3 top = getTopmostPoint(geomVertices, numVerts);
	XMFLOAT3 bottom = getBottommostPoint(geomVertices, numVerts);
	XMFLOAT3 front = getShallowestPoint(geomVertices, numVerts);
	XMFLOAT3 back = getDeepestPoint(geomVertices, numVerts);

	vertices[0] = XMFLOAT3(left.x, top.y, front.z);
	vertices[1] = XMFLOAT3(left.x, top.y, back.z);
	vertices[2] = XMFLOAT3(right.x, top.y, front.z);
	vertices[3] = XMFLOAT3(right.x, top.y, back.z);
	vertices[4] = XMFLOAT3(left.x, bottom.y, front.z);
	vertices[5] = XMFLOAT3(left.x, bottom.y, back.z);
	vertices[6] = XMFLOAT3(right.x, bottom.y, front.z);
	vertices[7] = XMFLOAT3(right.x, bottom.y, back.z);

	// set up the dimensions
	halfWidth = abs(right.x - left.x)/2;
	halfHeight = abs(top.y - bottom.y)/2;
	halfDepth = abs(front.z - back.z)/2;
}


OBB::~OBB(void)
{
}

// set the transforms
void OBB::setScale(XMFLOAT3 scale)
{
	halfWidth *= abs(scale.x);
	halfHeight *= abs(scale.y);
	halfDepth *= abs(scale.z);
}

void OBB::setRotation(XMFLOAT4 rotation)
{
	// rotate the vertices
	XMVECTOR vec;
	XMVECTOR rotQuat = XMVectorSet(rotation.x, rotation.y, rotation.z, rotation.w);
	XMMATRIX rot = XMMatrixRotationQuaternion(rotQuat);
	for(int i = 0; i < 8; i++)
	{
		vec = XMVectorSet(vertices[i].x, vertices[i].y, vertices[i].z, 0);
		vec = XMVector3Transform(vec, rot);
		XMStoreFloat3(&vertices[i], vec);
	}

	// rotate the axis
	vec = XMVectorSet(xAxis.x, xAxis.y, xAxis.z, 0);
	vec = XMVector3Transform(vec, rot);
	XMStoreFloat3(&xAxis, vec);

	vec = XMVectorSet(yAxis.x, yAxis.y, yAxis.z, 0);
	vec = XMVector3Transform(vec, rot);
	XMStoreFloat3(&yAxis, vec);

	vec = XMVectorSet(zAxis.x, zAxis.y, zAxis.z, 0);
	vec = XMVector3Transform(vec, rot);
	XMStoreFloat3(&zAxis, vec);
}

void OBB::setPosition(XMFLOAT3 position)
{
	centerPos = position;

	for(int i = 0; i < 8; i++)
	{
		vertices[i].x += position.x;
		vertices[i].y += position.y;
		vertices[i].z += position.z;
	}
}

// accessors for the extreme points (for setting up bounding boxes)
XMFLOAT3 OBB::getLeftmostPoint(Vertex* geomVertices, int numVerts)
{
	XMFLOAT3 vert = geomVertices[0].Position;

	for(int i = 1; i < numVerts; i++)
	{
		float x = geomVertices[i].Position.x;
		if(x < vert.x)	vert = geomVertices[i].Position;
	}

	return vert;
}

XMFLOAT3 OBB::getRightmostPoint(Vertex* geomVertices, int numVerts)
{
	XMFLOAT3 vert = geomVertices[0].Position;

	for(int i = 1; i < numVerts; i++)
	{
		float x = geomVertices[i].Position.x;
		if(x > vert.x)	vert = geomVertices[i].Position;
	}

	return vert;
}

XMFLOAT3 OBB::getTopmostPoint(Vertex* geomVertices, int numVerts)
{
	XMFLOAT3 vert = geomVertices[0].Position;

	for(int i = 1; i < numVerts; i++)
	{
		float y = geomVertices[i].Position.y;
		if(y > vert.y)	vert = geomVertices[i].Position;
	}

	return vert;
}

XMFLOAT3 OBB::getBottommostPoint(Vertex* geomVertices, int numVerts)
{
	XMFLOAT3 vert = geomVertices[0].Position;

	for(int i = 1; i < numVerts; i++)
	{
		float y = geomVertices[i].Position.y;
		if(y < vert.y)	vert = geomVertices[i].Position;
	}

	return vert;
}

XMFLOAT3 OBB::getDeepestPoint(Vertex* geomVertices, int numVerts)
{
	XMFLOAT3 vert = geomVertices[0].Position;

	for(int i = 1; i < numVerts; i++)
	{
		float z = geomVertices[i].Position.z;
		if(z > vert.z)	vert = geomVertices[i].Position;
	}

	return vert;
}

XMFLOAT3 OBB::getShallowestPoint(Vertex* geomVertices, int numVerts)
{
	XMFLOAT3 vert = geomVertices[0].Position;

	for(int i = 1; i < numVerts; i++)
	{
		float z = geomVertices[i].Position.z;
		if(z < vert.z)	vert = geomVertices[i].Position;
	}

	return vert;
}