#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

#pragma once

#include "Point.h"

namespace algo {

	static class ALGORITHMS_API Intersection
	{
	public:
		// The main function that returns true if line segment 'p1q1' 
		// and 'p2q2' intersect. 
		static bool doIntersect(Point p1, Point q1, Point p2, Point q2);

	private:
		// Given three colinear points p, q, r, the function checks if 
		// point q lies on line segment 'pr' 
		static bool onSegment(Point p, Point q, Point r);

		// To find orientation of ordered triplet (p, q, r). 
		// The function returns following values 
		// 0 --> p, q and r are colinear 
		// 1 --> Clockwise 
		// 2 --> Counterclockwise 
		static int orientation(Point p, Point q, Point r);
	};

}

#include "pch.h"