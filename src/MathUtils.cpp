#include "MathUtils.h"
#include <float.h>
#include <cmath>

const float MathUtils::Infinity = FLT_MAX;
const float MathUtils::Pi       = 3.1415926535f;
const float MathUtils::TwoPi	 = Pi * 2.0f;
const float MathUtils::PiOver2	 = Pi / 2.0f;
const float MathUtils::PiOver4	 = Pi / 4.0f;

float MathUtils::AngleFromXY(float x, float y)
{
	float theta = 0.0f;
 
	// Quadrant I or IV
	if(x >= 0.0f) 
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(y / x); // in [-pi/2, +pi/2]

		if(theta < 0.0f)
			theta += 2.0f*Pi; // in [0, 2*pi).
	}

	// Quadrant II or III
	else      
		theta = atanf(y/x) + Pi; // in [0, 2*pi).

	return theta;
}