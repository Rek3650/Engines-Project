#pragma once

#include "OBB.h"

class Collision
{
public:
	Collision(void);
	~Collision(void);

	XMFLOAT3 vectorSubtract(XMFLOAT3 A, XMFLOAT3 B);
	XMFLOAT3 cross(XMFLOAT3 A, XMFLOAT3 B);
	float dot(XMFLOAT3 A, XMFLOAT3 B);
	bool SAT(OBB* A, OBB* B);
};

