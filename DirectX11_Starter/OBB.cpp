#include "OBB.h"
#include <stdlib.h>
#include <math.h>

OBB::OBB()
{
	// set up the position and axis
	centerPos = XMFLOAT3(0, 0, 0);
	offset = XMFLOAT3(0, 0, 0);
	xAxis = XMFLOAT3(1, 0, 0);
	yAxis = XMFLOAT3(0, 1, 0);
	zAxis = XMFLOAT3(0, 0, 1);

	// set up the dimensions
	halfWidth = 1;
	halfHeight = 1;
	halfDepth = 1;

	// set up the vertices
	updateVerts();
}


OBB::~OBB(void)
{
}


void OBB::Update(XMFLOAT4X4 w)
{
	XMMATRIX world = XMMatrixSet(w._11, w._12, w._13, w._14,
								 w._21, w._22, w._23, w._24,
								 w._31, w._32, w._33, w._34,
								 w._41, w._42, w._43, w._44);
	world = XMMatrixInverse(&XMMatrixDeterminant(world), world);

	// update the x axis
	XMVECTOR vec = XMVectorSet(1, 0, 0, 1);
	vec = XMVector3Transform(vec, world);
	XMStoreFloat3(&xAxis, vec);

	float mag = sqrtf((xAxis.x*xAxis.x)+(xAxis.y*xAxis.y)+(xAxis.z*xAxis.z));
	xAxis.x /= mag;
	xAxis.y /= mag;
	xAxis.z /= mag;

	// update the y axis
	vec = XMVectorSet(0, 1, 0, 1);
	vec = XMVector3Transform(vec, world);
	XMStoreFloat3(&yAxis, vec);

	mag = sqrtf((yAxis.x*yAxis.x)+(yAxis.y*yAxis.y)+(yAxis.z*yAxis.z));
	yAxis.x /= mag;
	yAxis.y /= mag;
	yAxis.z /= mag;

	// update the z axis
	vec = XMVectorSet(0, 0, 1, 1);
	vec = XMVector3Transform(vec, world);
	XMStoreFloat3(&zAxis, vec);

	mag = sqrtf((zAxis.x*zAxis.x)+(zAxis.y*zAxis.y)+(zAxis.z*zAxis.z));
	zAxis.x /= mag;
	zAxis.y /= mag;
	zAxis.z /= mag;

	vec = XMVectorSet(offset.x, offset.y, offset.z, 1);
	vec = XMVector3Transform(vec, world);
	XMFLOAT3 tempOffset(0, 0, 0);
	XMStoreFloat3(&tempOffset, vec);

	// get the position out of the matrix and add in an offset
	centerPos.x = w._14 + tempOffset.x;
	centerPos.y = w._24 + tempOffset.y;
	centerPos.z = w._34 + tempOffset.z;

	// update the vertices
	for(int i = 0; i < 8; i++)
	{
		vec = XMVectorSet(vertices[i].x, vertices[i].y, vertices[i].z, 1);
		vec = XMVector3Transform(vec, world);
		XMStoreFloat3(&(verts[i]), vec);

		verts[i].x += centerPos.x;
		verts[i].y += centerPos.y;
		verts[i].z += centerPos.z;
	}
}

// set the position of the bounding box (relative to the local axis)
void OBB::SetPos(XMFLOAT3 pos)
{
	offset = pos;
}

// sets the size of the bounding box
void OBB::SetScale(XMFLOAT3 scale)
{
	halfWidth = scale.x/2;
	halfHeight = scale.y/2;
	halfDepth = scale.z/2;

	updateVerts();
}

// returns an array of length 8 that contains the vertices of the bounding box
XMFLOAT3* OBB::GetVerts()
{
	return verts;
}

void OBB::updateVerts()
{
	XMFLOAT3 left = getLeftmostPoint();
	XMFLOAT3 right = getRightmostPoint();
	XMFLOAT3 top = getTopmostPoint();
	XMFLOAT3 bottom = getBottommostPoint();
	XMFLOAT3 front = getShallowestPoint();
	XMFLOAT3 back = getDeepestPoint();

	vertices[0] = XMFLOAT3(left.x, top.y, front.z);
	vertices[1] = XMFLOAT3(left.x, top.y, back.z);
	vertices[2] = XMFLOAT3(right.x, top.y, front.z);
	vertices[3] = XMFLOAT3(right.x, top.y, back.z);
	vertices[4] = XMFLOAT3(left.x, bottom.y, front.z);
	vertices[5] = XMFLOAT3(left.x, bottom.y, back.z);
	vertices[6] = XMFLOAT3(right.x, bottom.y, front.z);
	vertices[7] = XMFLOAT3(right.x, bottom.y, back.z);

	for(int i = 0; i < 8; i++)
	{
		verts[i] = vertices[i];
	}
}

// accessors for the extreme points (for setting up bounding boxes)
XMFLOAT3 OBB::getLeftmostPoint()
{
	XMFLOAT3 vert(0, 0, 0);

	vert.x = centerPos.x-(xAxis.x*(halfWidth/2));
	vert.y = centerPos.y-(xAxis.y*(halfWidth/2));
	vert.z = centerPos.z-(xAxis.z*(halfWidth/2));

	return vert;
}

XMFLOAT3 OBB::getRightmostPoint()
{
	XMFLOAT3 vert(0, 0, 0);

	vert.x = centerPos.x+(xAxis.x*(halfWidth/2));
	vert.y = centerPos.y+(xAxis.y*(halfWidth/2));
	vert.z = centerPos.z+(xAxis.z*(halfWidth/2));

	return vert;
}

XMFLOAT3 OBB::getTopmostPoint()
{
	XMFLOAT3 vert(0, 0, 0);

	vert.x = centerPos.x+(yAxis.x*(halfWidth/2));
	vert.y = centerPos.y+(yAxis.y*(halfWidth/2));
	vert.z = centerPos.z+(yAxis.z*(halfWidth/2));

	return vert;
}

XMFLOAT3 OBB::getBottommostPoint()
{
	XMFLOAT3 vert(0, 0, 0);

	vert.x = centerPos.x-(yAxis.x*(halfWidth/2));
	vert.y = centerPos.y-(yAxis.y*(halfWidth/2));
	vert.z = centerPos.z-(yAxis.z*(halfWidth/2));

	return vert;
}

XMFLOAT3 OBB::getDeepestPoint()
{
	XMFLOAT3 vert(0, 0, 0);

	vert.x = centerPos.x+(zAxis.x*(halfWidth/2));
	vert.y = centerPos.y+(zAxis.y*(halfWidth/2));
	vert.z = centerPos.z+(zAxis.z*(halfWidth/2));

	return vert;
}

XMFLOAT3 OBB::getShallowestPoint()
{
	XMFLOAT3 vert(0, 0, 0);

	vert.x = centerPos.x-(zAxis.x*(halfWidth/2));
	vert.y = centerPos.y-(zAxis.y*(halfWidth/2));
	vert.z = centerPos.z-(zAxis.z*(halfWidth/2));

	return vert;
}