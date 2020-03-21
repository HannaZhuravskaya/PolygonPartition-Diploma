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

#include "PolygonData.h"
#include<vector>
#include<map>

typedef  Algorithms::Point Point;
struct ALGORITHMS_API Vertex;
struct  ALGORITHMS_API Face;

struct ALGORITHMS_API Edge {
	Vertex* v;
	Face* f;
	Edge* prev, * next;
	Edge* sym;
	int numOfEdge;
	bool isDeleted = false;
	bool isTemp = false;
};

struct Vertex { 
	Edge* e;
	Point* pos;
	int numOfVertex;
	bool isDeleted = false;
	bool isTemp = false;
};

struct Face { 
	Edge* e;
	double area;
	int numOfFace;
	bool isDeleted = false;
};

class ALGORITHMS_API Mesh {
public:
	Mesh();
	std::vector<Edge*> E; 
	std::vector<Vertex*> V;
	std::vector<Face*> F;

	std::map<int, Edge*> verticalMesh;
	std::map<int, double> verticalMax;
	std::map<int, double> verticalMin;

	double meshOuterPerimeter;
	double meshInnerPerimeter;

	Vertex* leftVertex;
	Face* leftFace;

	void setEdge(Edge* edgeToSet, Vertex* v, Face* f, Edge* prev, Edge* next, Edge* sym);
	
	PolygonData convertToPolygonData();
	void convertFromPolygonData(PolygonData data);
	void convertFromPolygonDataOfConvexLeftTraversalPolygon(PolygonData data, double area);
	void iteratingOverTheEdgesOfFace(Face * f);

	Vertex* splitEdge(Edge* e, Point *pos);
	Edge* removeVertex(Vertex*);
	std::vector<Vertex*> removeEdge(Edge* e);
	Edge* connectVertexes(Vertex* v1, Vertex* v2);

	std::vector<std::pair<Point*, Point*>> splitFaces(double area);
	double calculateInternalPerimetrOfHorisontalPartition(std::vector<std::pair<Point*, Point*>> splitEdges, Edge* left,int numOfLeft, Edge* right, int numOfRight);
	void splitByVerticalGrid();

	double areaOfFace(Face* f);
	double perimeterOfFace(Face* f);
	double getDistance(Vertex * v1, Vertex * v2);
	void removeTempVertexesAndEdges();

	void removeEdgeWithTempVertexes(Edge* e);
	double isPointInEdge(Vertex* v1, Vertex* v2, Point* p);

	double crossProductLength(Point a, Point b, Point c);
	std::tuple<double, double, double> findCoeff(double x1, double y1, double x2, double y2);
	std::tuple<int, double, double> findRoots(double a, double b, double c);
	void addRoots(std::tuple<int, double, double> r1, std::vector<double>* roots);
	double findYByXAndTwoVertixes(Vertex *v1, Vertex * v2, double xPos);

	Edge* findPrevEdge(Vertex* v, Face * f);
	Edge* findNextEdge(Vertex* v, Face* f);

	double ** getInternalHorizontalPerimeters();

	std::pair<double, long long> getOptimalCombinationForInternalPerimeter(double** horizontalPerimeters);
	std::vector<std::pair<Point*, Point*> > getHorizontalEdges(Edge* leftEdge, Edge* rightEdge, int numOfFacesToSplit);
	void connectPointsToEdges(std::vector<std::pair<Point*, Point*> > edges, Face* face);
	void splitMeshByMask(long long mask);

	std::pair<Point*, Point*> findPointsV(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double S);
};

struct S
{
	double* v3;
};

#include "pch.h"