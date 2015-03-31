#include "Spline.h"
#include <xmmintrin.h>

// Initialize stuff
Spline::Spline(void)
{
	knotVector.clear();
}

// clean up after myself
Spline::~Spline(void)
{
}

// takes in a list of control points and the number of divisions and creates a cubic spline
// this is an implementation of De Boor's algorithm for finding points on a spline
std::vector<XMFLOAT3> Spline::makeSpline(const std::vector<XMFLOAT3>& ctrlPts, int numDiv)
{
	std::vector<XMFLOAT3> spline;
	spline.clear();

    if(ctrlPts.size() >= 4)
    {
        createKnotVector(4, ctrlPts.size()); 

        for(int steps=0; steps<=numDiv; steps++)
        {
            // use steps to get a 0-1 range value for progression along the curve
                    // then get that value into the range [k-1, n+1]
            // k-1 = subCurveOrder-1
            // n+1 = always the number of total control points

            float t = ( steps / (float)numDiv ) * ( ctrlPts.size() - 3 ) + 3;

            XMFLOAT3 newPoint(0,0,0);
            for(int i=1; i <= ctrlPts.size(); i++)
            {
                float weightForControl = calculateWeightForPointI(i, 4, t);
				XMFLOAT3 ctrlPt = ctrlPts[i-1];
				XMFLOAT3 weightedCtrlPt(ctrlPt.x*weightForControl, ctrlPt.y*weightForControl, ctrlPt.z*weightForControl);
				newPoint = XMFLOAT3(weightedCtrlPt.x+newPoint.x, weightedCtrlPt.y+newPoint.y, weightedCtrlPt.z+newPoint.z);
            }
            spline.push_back(newPoint);
        }
    }

	return spline;
}

// gets a specific point on the spline instead of generating the entire thing
// 0 <= t <= 1
XMFLOAT3 Spline::getPointOnSpline(const std::vector<XMFLOAT3>& ctrlPts, float t)
{
	// make sure i is in bounds
	if(t < 0) t = 0;
	else if(t > 1) t = 1;

	XMFLOAT3 splinePoint(0,0,0);

	if(ctrlPts.size() >= 4)
    {
        createKnotVector(4, ctrlPts.size()); 

		t = t * ( ctrlPts.size() - 3 ) + 3;

        for(int i=1; i <= ctrlPts.size(); i++)
        {
            float weightForControl = calculateWeightForPointI(i, 4, t);
			XMFLOAT3 ctrlPt = ctrlPts[i-1];
			XMFLOAT3 weightedCtrlPt(ctrlPt.x*weightForControl, ctrlPt.y*weightForControl, ctrlPt.z*weightForControl);
			splinePoint = XMFLOAT3(weightedCtrlPt.x+splinePoint.x, weightedCtrlPt.y+splinePoint.y, weightedCtrlPt.z+splinePoint.z);
        }
	}

	return splinePoint;
}

// http://en.wikipedia.org/wiki/De_Boor%27s_algorithm
//i = the weight we're looking for, i should go from 1 to n+1, where n+1 is equal to the total number of control points.
//k = curve order = power/degree +1. eg, to break whole curve into cubics use a curve order of 4
//numCtrlPts = number of total control points 
//t = current step/interp value
float Spline::calculateWeightForPointI( int i, int k, float t )
{
    //test if we've reached the bottom of the recursive call
    if( k == 1 )
    {
        if( t >= knot(i) && t < knot(i+1) )
            return 1;
        else
            return 0;
    } 

    float numA = ( t - knot(i) );
    float denomA = ( knot(i + k-1) - knot(i) );
    float numB = ( knot(i + k) - t );
    float denomB = ( knot(i + k) - knot(i + 1) );

    float subweightA = 0;
    float subweightB = 0;

    if( denomA != 0 )
        subweightA = numA / denomA * calculateWeightForPointI(i, k-1, t);
    if( denomB != 0 )
        subweightB = numB / denomB * calculateWeightForPointI(i+1, k-1, t);

    return subweightA + subweightB;

}

//returns the knot value at the passed in index 
float Spline::knot(int indexForKnot)
{
        // When getting the index for the knot function i remember to subtract 1 from i 
		// because of the difference caused by us counting from i=1 to n+1 and indexing a vector from 0
        return knotVector[indexForKnot-1];
}
//calculate the whole knot vector
void Spline::createKnotVector(int curveOrderK, int numControlPoints)
{
    int knotSize = curveOrderK + numControlPoints;
    for(int count = 0; count < knotSize; count++)
    {
        knotVector.push_back(count);
    }
}

#pragma region SIMD version of spline code

// takes in a list of control points and the number of divisions and creates a cubic spline
// this is an implementation of De Boor's algorithm for finding points on a spline
std::vector<XMFLOAT3> Spline::sseMakeSpline(const std::vector<XMFLOAT3>& ctrlPts, int numDiv)
{
	std::vector<XMFLOAT3> spline;
	spline.clear();

    if(ctrlPts.size() >= 4)
    {
        createKnotVector(4, ctrlPts.size()); 

        for(int steps=0; steps<=numDiv; steps++)
        {
            // use steps to get a 0-1 range value for progression along the curve
                    // then get that value into the range [k-1, n+1]
            // k-1 = subCurveOrder-1
            // n+1 = always the number of total control points

            float t = ( steps / (float)numDiv ) * ( ctrlPts.size() - 3 ) + 3;

            XMFLOAT3 newPoint(0,0,0);
			__m128 weight;
			__m128 controlPoint;
			__m128 weightedControlPoint;
			__m128 splinePoint;
			__declspec(align(16)) float temp[4];
            for(int i=1; i <= ctrlPts.size(); i++)
            {
                float weightForControl = calculateWeightForPointI(i, 4, t);
				XMFLOAT3 ctrlPt = ctrlPts[i-1];
				controlPoint = _mm_set_ps(ctrlPt.x, ctrlPt.y, ctrlPt.z, 0);
				weight = _mm_set_ps(weightForControl, weightForControl, weightForControl, weightForControl);
				weightedControlPoint = _mm_mul_ps(controlPoint, weight);
				splinePoint = _mm_set_ps(newPoint.x, newPoint.y, newPoint.z, 0);
				splinePoint = _mm_add_ps(splinePoint, weightedControlPoint);
				_mm_store_ps(temp, splinePoint);
				newPoint = XMFLOAT3(temp[3], temp[2], temp[1]);
            }
            spline.push_back(newPoint);
        }
    }

	return spline;
}

// gets a specific point on the spline instead of generating the entire thing
// 0 <= t <= 1
XMFLOAT3 Spline::sseGetPointOnSpline(const std::vector<XMFLOAT3>& ctrlPts, float t)
{
	// make sure i is in bounds
	if(t < 0) t = 0;
	else if(t > 1) t = 1;

	XMFLOAT3 newPoint(0,0,0);

	if(ctrlPts.size() >= 4)
    {
        createKnotVector(4, ctrlPts.size()); 

		t = t * ( ctrlPts.size() - 3 ) + 3;

        __m128 weight;
		__m128 controlPoint;
		__m128 weightedControlPoint;
		__m128 splinePoint;
		__declspec(align(16)) float temp[4];
        for(int i=1; i <= ctrlPts.size(); i++)
        {
            float weightForControl = calculateWeightForPointI(i, 4, t);
			XMFLOAT3 ctrlPt = ctrlPts[i-1];
			controlPoint = _mm_set_ps(ctrlPt.x, ctrlPt.y, ctrlPt.z, 0);
			weight = _mm_set_ps(weightForControl, weightForControl, weightForControl, weightForControl);
			weightedControlPoint = _mm_mul_ps(controlPoint, weight);
			splinePoint = _mm_set_ps(newPoint.x, newPoint.y, newPoint.z, 0);
			splinePoint = _mm_add_ps(splinePoint, weightedControlPoint);
			_mm_store_ps(temp, splinePoint);
			newPoint = XMFLOAT3(temp[3], temp[2], temp[1]);
        }
	}

	return newPoint;
}

#pragma endregion