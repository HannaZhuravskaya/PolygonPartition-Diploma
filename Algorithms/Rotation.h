#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

#pragma once

#include "Point.h"
#include <vector>

namespace algo {

	typedef std::vector<double> vectorD;

	enum ALGORITHMS_API RotationDirection {
		Left,
		Right
	};

	class ALGORITHMS_API Rotation
	{
	private:
		double angleInDegrees;
		double* x0, * y0;
		double* xOffset, * yOffset;

	public:
		Rotation(double angleInDegrees, RotationDirection direction);

		static double findAngleOfSegmentInDegrees(double x1, double y1, double x2, double y2);
		static void rotateFigure90(vectorD* x, vectorD* y);
		static void rotatePoint90(Point* p);

		bool tryRotateFigure(vectorD* x, vectorD* y);
		bool tryRotateFigureBack(vectorD* x, vectorD* y);
		bool tryRotatePointBack(Point* p);

	private:
		Point rotate(double x, double y, double angleCos, double angleSin);
		void calculateParams(vectorD x, vectorD y, double sin, double cos);
		void calculateCentralPoint(double x_min, double x_max, double y_min, double y_max);
		void calculateOffsets(Point left, Point right, Point top, Point bottom, double sin, double cos);
	};

}

#include "pch.h"