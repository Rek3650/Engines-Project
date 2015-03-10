#pragma once

#include <DirectXMath.h>
#include <vector>

// For DirectX Math
using namespace DirectX;

class Spline
{
public:
	Spline(void);
	~Spline(void);

	std::vector<XMFLOAT3> makeSpline(const std::vector<XMFLOAT3>& ctrlPts, int numDiv);
	XMFLOAT3 getPointOnSpline(const std::vector<XMFLOAT3>& ctrlPts, float t);

private:
	std::vector<int> knotVector;
	float calculateWeightForPointI(int i, int k, int numCtrlPts, float t);
	float knot(int indexForKnot);
	void createKnotVector(int curveOrderK, int numControlPoints);
};

