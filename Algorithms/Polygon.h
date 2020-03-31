// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the ALGORITHMS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// ALGORITHMS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

//extern ALGORITHMS_API int nAlgorithms;
//ALGORITHMS_API int fnAlgorithms(void);

#pragma once

#include <vector>
#include <exception>
#include <stdexcept>
#include <map>
#include <algorithm> 

namespace Algorithms
{
	struct ALGORITHMS_API Point
	{
		double x;
		double y;
	};

	class ALGORITHMS_API Polygon
	{
	private:
		int numOfSides;
		std::vector<Point> points;
		void isPointsCompleted();
	public:
		Polygon();
		Polygon(int numOfSides);
		Polygon(int numOfSides, std::vector<Point> points);
		int getNumOfSides();
		int getNumOfPoints();
		bool tryAddPoint(Point point);
		double getSquare();
		double getPerimeter();
		bool isConvex();
		Point getPreviousPoint();
		Point getPointAt(int index);
		double crossProductLength(Point a, Point b, Point c);
		bool isLeftTraversal;
	};

	static class ALGORITHMS_API Intersection
	{
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

	public:
		// The main function that returns true if line segment 'p1q1' 
		// and 'p2q2' intersect. 
		static bool doIntersect(Point p1, Point q1, Point p2, Point q2);
	};

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
	};
}