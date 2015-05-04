#include "Collision.h"
#include <stdlib.h>

Collision::Collision(void)
{
}


Collision::~Collision(void)
{
}

XMFLOAT3 Collision::vectorSubtract(XMFLOAT3 A, XMFLOAT3 B)
{
	XMFLOAT3 result(0, 0, 0);

	result.x = A.x - B.x;
	result.y = A.y - B.y;
	result.z = A.z - B.z;

	return result;
}

XMFLOAT3 Collision::cross(XMFLOAT3 A, XMFLOAT3 B)
{
	XMFLOAT3 result(0, 0, 0);

	result.x = (A.y * B.z) - (A.z * B.y);
	result.y = (A.z * B.x) - (A.x * B.z);
	result.z = (A.x * B.y) - (A.y * B.x);

	return result;
}

float Collision::dot(XMFLOAT3 A, XMFLOAT3 B)
{
	return (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
}

// implementation of Seperating Axis Theorem
// http://www.jkh.me/files/tutorials/Separating%20Axis%20Theorem%20for%20Oriented%20Bounding%20Boxes.pdf
bool Collision::SAT(OBB* A, OBB* B)
{
	XMFLOAT3 T = vectorSubtract(B->CenterPos(), A->CenterPos());

	// A x-axis
	XMFLOAT3 L = A->XAxis();
	float n =	abs(dot(T, L));
	float m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
				abs(A->HalfHeight()*dot(A->YAxis(), L)) +
				abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
				abs(B->HalfWidth()*dot(B->XAxis(), L)) +
				abs(B->HalfHeight()*dot(B->YAxis(), L)) +
				abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case1 = n>m;

	// A y-axis
	L = A->YAxis();
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case2 = n>m;

	// A z-axis
	L = A->ZAxis();
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case3 = n>m;

	// B x-axis
	L = B->XAxis();
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case4 = n>m;

	// B y-axis
	L = B->YAxis();
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case5 = n>m;

	// B z-axis
	L = B->ZAxis();
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case6 = n>m;

	// A x-axis cross B x-axis
	L = cross(A->XAxis(), B->XAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case7 = n>m;

	// A x-axis cross B y-axis
	L = cross(A->XAxis(), B->YAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case8 = n>m;

	// A x-axis cross B z-axis
	L = cross(A->XAxis(), B->ZAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case9 = n>m;

	// A y-axis cross B x-axis
	L = cross(A->YAxis(), B->XAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case10 = n>m;

	// A y-axis cross B y-axis
	L = cross(A->YAxis(), B->YAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case11 = n>m;

	// A y-axis cross B z-axis
	L = cross(A->YAxis(), B->ZAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case12 = n>m;

	// A z-axis cross B x-axis
	L = cross(A->ZAxis(), B->XAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case13 = n>m;

	// A z-axis cross B y-axis
	L = cross(A->ZAxis(), B->YAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case14 = n>m;

	// A z-axis cross B z-axis
	L = cross(A->ZAxis(), B->ZAxis());
	n = abs(dot(T, L));
	m =	abs(A->HalfWidth()*dot(A->XAxis(), L)) +
		abs(A->HalfHeight()*dot(A->YAxis(), L)) +
		abs(A->HalfDepth()*dot(A->ZAxis(), L)) +
		abs(B->HalfWidth()*dot(B->XAxis(), L)) +
		abs(B->HalfHeight()*dot(B->YAxis(), L)) +
		abs(B->HalfDepth()*dot(B->ZAxis(), L));
	bool case15 = n>m;

	return (!case1  && !case2  && !case3  && 
			!case4  && !case5  && !case6  && 
			!case7  && !case8  && !case9  && 
			!case10 && !case11 && !case12 &&
			!case13 && !case14 && !case15);
}
