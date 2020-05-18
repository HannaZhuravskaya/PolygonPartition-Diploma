#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

#pragma once

#include "Point.h"
#include "Helper.h"
#include <vector>
#include <stdexcept>

namespace algo {

	class ALGORITHMS_API Polygon
	{
	private:
		int numOfSides;
		std::vector<Point> points;

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
		bool isLeftTraversal(double eps);

	private:
		void isPointsCompleted();
	};

}

#include "pch.h"