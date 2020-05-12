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

#pragma once

#include "D:\Programming Projects\Visual Studio\eigen\Eigen\Dense"
#include "Polygon.h"

typedef std::vector<double> vectorD;
typedef std::vector<int> vectorI;
typedef  Algorithms::Point point;

class ALGORITHMS_API PolygonData
{
public:

	vectorD vertex_x;
	vectorD vertex_y;
	vectorI edges;
	vectorI faces;

	PolygonData();

	int getNumOfVertexes();
	int getNumOfFaces();

	bool tryAddPoints(vectorD x, vectorD y);
	bool tryAddFace(int numOfSides, vectorI pointsNumbers);
};

enum ALGORITHMS_API RotationDirection {
	Left,
	Right
};

class ALGORITHMS_API Rotation
{
private:
	double angleInDegrees;
	double *x0, *y0;
	double *xOffset, *yOffset;

	point rotate(double x, double y, double angleCos, double angleSin);
	void calculateParams(vectorD x, vectorD y, double sin, double cos);
	void calculateCentralPoint(double x_min, double x_max, double y_min, double y_max);
	void calculateOffsets(point left, point right, point top, point bottom, double sin, double cos);


public:
	static double findAngleOfSegmentInDegrees(double x1, double y1, double x2, double y2);
	static std::pair<Algorithms::Point*, Algorithms::Point*> findPointsOfSegmentByAngle(double angle, double width, double height);
	static void rotateFigure90(vectorD* x, vectorD* y);
	static void rotatePoint90(Algorithms::Point * p);

	Rotation(double angleInDegrees, RotationDirection direction);

	bool tryRotateFigure(vectorD *x, vectorD *y);
	bool tryRrotateTheFigureBack(vectorD *x, vectorD *y);
	bool tryRotateThePointBack(Algorithms::Point* p);
};




#include "pch.h"