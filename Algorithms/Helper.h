#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

#pragma once

#include "Polygon.h"
#include <tuple>
#include <algorithm>
#include <vector>
#include <regex>

namespace algo {

	const double EPS = 0.0001;

	static class ALGORITHMS_API Helper
	{
	public:
		static std::tuple<int, double, double> findRootsOfEquation(double a, double b, double c, double eps);

		static double crossProductLength(Point a, Point b, Point c);

		static std::tuple<double, double, double> findLineCoefficients(double x1, double y1, double x2, double y2);

		static void addValidRootsToList(std::tuple<int, double, double> r1, std::vector<double>* roots);

		// distance from p1 to new point
		static Point* findPointOnSegmentByDistance(Point* p1, Point* p2, double distance, double eps);

		static void startPermutation(std::vector<int>* a);

		static void swap(std::vector<int>* a, int i, int j);

		static bool tryNextPermutation(std::vector<int>* a);

		static std::vector<std::string> splitStringByRegex(const std::string& stringToSplit, const std::string& regexPattern);

		static bool isEqual(double a, double b);
		static bool isNotEqual(double a, double b);
		static bool isGreater(double a, double b);
		static bool isLess(double a, double b);

		/* a Point is defined by its coordinates {int x, y;}
		===================================================================
		   isLeft(): tests if a point is Left|On|Right of an infinite line.
		   Input:  three points P0, P1, and P2
		   Return: >0 for P2 left of the line through P0 and P1
		           =0 for P2  on the line
		           <0 for P2  right of the line
		   See: Algorithm 1 "Area of Triangles and Polygons"*/
		static double isLeft(Point* P0, Point* P1, Point* P2);
	};

}

#include "pch.h"