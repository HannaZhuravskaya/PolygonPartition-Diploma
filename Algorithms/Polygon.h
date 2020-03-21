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
}