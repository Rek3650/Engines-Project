#pragma once

#include <DirectXMath.h>
#include "Mesh.h"

using namespace DirectX;

class OBB
{
private:
	XMFLOAT3 vertices[8];
	XMFLOAT3 verts[8];

	XMFLOAT3 centerPos;
	XMFLOAT3 offset;
	XMFLOAT3 xAxis;
	XMFLOAT3 yAxis;
	XMFLOAT3 zAxis;
	float halfWidth;
	float halfHeight;
	float halfDepth;

	XMFLOAT3 getLeftmostPoint();
	XMFLOAT3 getRightmostPoint();
	XMFLOAT3 getTopmostPoint();
	XMFLOAT3 getBottommostPoint();
	XMFLOAT3 getDeepestPoint();
	XMFLOAT3 getShallowestPoint();

	void updateVerts();

public:
	OBB();
	~OBB(void);

	XMFLOAT3 CenterPos(){return centerPos;};
	XMFLOAT3 XAxis(){return xAxis;};
	XMFLOAT3 YAxis(){return yAxis;};
	XMFLOAT3 ZAxis(){return zAxis;};
	float HalfWidth(){return halfWidth;};
	float HalfHeight(){return halfHeight;};
	float HalfDepth(){return halfDepth;};

	void SetPos(XMFLOAT3 pos);
	void SetScale(XMFLOAT3 scale);

	void Update(XMFLOAT4X4 w);
	XMFLOAT3* GetVerts();
};

