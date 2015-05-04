#pragma once

#include <DirectXMath.h>
#include "Mesh.h"

using namespace DirectX;

class OBB
{
private:
	XMFLOAT3 vertices[8];

	XMFLOAT3 centerPos;
	XMFLOAT3 xAxis;
	XMFLOAT3 yAxis;
	XMFLOAT3 zAxis;
	float halfWidth;
	float halfHeight;
	float halfDepth;

	XMFLOAT3 getLeftmostPoint(Vertex* geomVertices, int numVerts);
	XMFLOAT3 getRightmostPoint(Vertex* geomVertices, int numVerts);
	XMFLOAT3 getTopmostPoint(Vertex* geomVertices, int numVerts);
	XMFLOAT3 getBottommostPoint(Vertex* geomVertices, int numVerts);
	XMFLOAT3 getDeepestPoint(Vertex* geomVertices, int numVerts);
	XMFLOAT3 getShallowestPoint(Vertex* geomVertices, int numVerts);

public:
	OBB(Vertex* geomVertices, int numVerts);
	~OBB(void);

	XMFLOAT3 CenterPos(){return centerPos;};
	XMFLOAT3 XAxis(){return xAxis;};
	XMFLOAT3 YAxis(){return yAxis;};
	XMFLOAT3 ZAxis(){return zAxis;};
	float HalfWidth(){return halfWidth;};
	float HalfHeight(){return halfHeight;};
	float HalfDepth(){return halfDepth;};

	void setScale(XMFLOAT3 scale);
	void setRotation(XMFLOAT4 rotation);
	void setPosition(XMFLOAT3 position);
};

