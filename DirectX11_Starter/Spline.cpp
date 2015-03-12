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
std::vector<XMFLOAT3> Spline::makeSpline(const std::vector<XMFLOAT3>& ctrlPts, int numDiv)
{
	std::vector<XMFLOAT3> spline;
	spline.clear();

	int subCurveOrder = 4; // = k = I want to break my curve into to cubics

    // De boor 1st attempt
    if(ctrlPts.size() >= subCurveOrder)
    {
        createKnotVector(subCurveOrder, ctrlPts.size()); 

        for(int steps=0; steps<=numDiv; steps++)
        {
            // use steps to get a 0-1 range value for progression along the curve
                    // then get that value into the range [k-1, n+1]
            // k-1 = subCurveOrder-1
            // n+1 = always the number of total control points

            float t = ( steps / (float)numDiv ) * ( ctrlPts.size() - (subCurveOrder-1) ) + subCurveOrder-1;

            XMFLOAT3 newPoint(0,0,0);
            for(int i=1; i <= ctrlPts.size(); i++)
            {
                float weightForControl = calculateWeightForPointI(i, subCurveOrder, ctrlPts.size(), t);
				XMFLOAT3 ctrlPt = ctrlPts.at(i-1);
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

	int subCurveOrder = 4;

	XMFLOAT3 splinePoint(0,0,0);

	if(ctrlPts.size() >= subCurveOrder)
    {
        createKnotVector(subCurveOrder, ctrlPts.size()); 

		t = t * ( ctrlPts.size() - (subCurveOrder-1) ) + subCurveOrder-1;

        for(int i=1; i <= ctrlPts.size(); i++)
        {
            float weightForControl = calculateWeightForPointI(i, subCurveOrder, ctrlPts.size(), t);
			XMFLOAT3 ctrlPt = ctrlPts.at(i-1);
			XMFLOAT3 weightedCtrlPt(ctrlPt.x*weightForControl, ctrlPt.y*weightForControl, ctrlPt.z*weightForControl);
			splinePoint = XMFLOAT3(weightedCtrlPt.x+splinePoint.x, weightedCtrlPt.y+splinePoint.y, weightedCtrlPt.z+splinePoint.z);
        }
	}

	return splinePoint;
}

// simd math version of spline
std::vector<XMFLOAT3> Spline::sseMakeSpline(const std::vector<XMFLOAT3>& ctrlPts, int numDiv)
{
	std::vector<XMFLOAT3> spline;
	spline.clear();

	int subCurveOrder = 4; // = k = I want to break my curve into to cubics

    // De boor 1st attempt
    if(ctrlPts.size() >= subCurveOrder)
    {
        createKnotVector(subCurveOrder, ctrlPts.size()); 

        for(int steps=0; steps<=numDiv; steps++)
        {
            // use steps to get a 0-1 range value for progression along the curve
                    // then get that value into the range [k-1, n+1]
            // k-1 = subCurveOrder-1
            // n+1 = always the number of total control points

            float t = ( steps / (float)numDiv ) * ( ctrlPts.size() - (subCurveOrder-1) ) + subCurveOrder-1;

            XMFLOAT3 newPoint(0,0,0);
			__m128 weight;
			__m128 controlPoint;
			__m128 weightedControlPoint;
			__m128 splinePoint;
			__declspec(align(16)) float temp[4];
            for(int i=1; i <= ctrlPts.size(); i++)
            {
                float weightForControl = sseCalculateWeightForPointI(i, subCurveOrder, ctrlPts.size(), t);
				XMFLOAT3 ctrlPt = ctrlPts.at(i-1);
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

// simd math version of spline
XMFLOAT3 Spline::sseGetPointOnSpline(const std::vector<XMFLOAT3>& ctrlPts, float t)
{
	// make sure i is in bounds
	if(t < 0) t = 0;
	else if(t > 1) t = 1;

	int subCurveOrder = 4;

	XMFLOAT3 newPoint(0,0,0);

	if(ctrlPts.size() >= subCurveOrder)
    {
        createKnotVector(subCurveOrder, ctrlPts.size()); 

		t = t * ( ctrlPts.size() - (subCurveOrder-1) ) + subCurveOrder-1;

       __m128 weight;
		__m128 controlPoint;
		__m128 weightedControlPoint;
		__m128 splinePoint;
		__declspec(align(16)) float temp[4];
        for(int i=1; i <= ctrlPts.size(); i++)
        {
            float weightForControl = sseCalculateWeightForPointI(i, subCurveOrder, ctrlPts.size(), t);
			XMFLOAT3 ctrlPt = ctrlPts.at(i-1);
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

//i = the weight we're looking for, i should go from 1 to n+1, where n+1 is equal to the total number of control points.
//k = curve order = power/degree +1. eg, to break whole curve into cubics use a curve order of 4
//cps = number of total control points 
//t = current step/interp value
float Spline::calculateWeightForPointI( int i, int k, int cps, float t )
{
    //test if we've reached the bottom of the recursive call
    if( k == 1 )
    {
        if( t >= knot(i) && t < knot(i+1) )
            return 1;
        else
            return 0;
    } 

    float numeratorA = ( t - knot(i) );
    float denominatorA = ( knot(i + k-1) - knot(i) );
    float numeratorB = ( knot(i + k) - t );
    float denominatorB = ( knot(i + k) - knot(i + 1) );

    float subweightA = 0;
    float subweightB = 0;

    if( denominatorA != 0 )
        subweightA = numeratorA / denominatorA * calculateWeightForPointI(i, k-1, cps, t);
    if( denominatorB != 0 )
        subweightB = numeratorB / denominatorB * calculateWeightForPointI(i+1, k-1, cps, t);

    return subweightA + subweightB;

}

// simd math version of spline
float Spline::sseCalculateWeightForPointI( int i, int k, int cps, float t )
{
    //test if we've reached the bottom of the recursive call
    if( k == 1 )
    {
        if( t >= knot(i) && t < knot(i+1) )
            return 1;
        else
            return 0;
    } 

    float numeratorA = ( t - knot(i) );
    float denominatorA = ( knot(i + k-1) - knot(i) );
    float numeratorB = ( knot(i + k) - t );
    float denominatorB = ( knot(i + k) - knot(i + 1) );

    float subweightA = 0;
    float subweightB = 0;

    if( denominatorA != 0 )
        subweightA = numeratorA / denominatorA * calculateWeightForPointI(i, k-1, cps, t);
    if( denominatorB != 0 )
        subweightB = numeratorB / denominatorB * calculateWeightForPointI(i+1, k-1, cps, t);

    return subweightA + subweightB;

}

//returns the knot value at the passed in index 
//if i = 1 and we want Xi then we have to remember to index with i-1
float Spline::knot(int indexForKnot)
{
        // When getting the index for the knot function i remember to subtract 1 from i 
		// because of the difference caused by us counting from i=1 to n+1 and indexing a vector from 0
        return knotVector.at(indexForKnot-1);
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