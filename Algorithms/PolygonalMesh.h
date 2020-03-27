#pragma region Includes and Constants

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

const double EPS = 0.0001;

#pragma endregion

#pragma region Mesh Structures

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

#pragma endregion

class ALGORITHMS_API Mesh {
public:

#pragma region Properties 

	std::vector<Edge*> E;
	std::vector<Vertex*> V;
	std::vector<Face*> F;

	std::vector<Vertex*> originalVertexes;
#pragma endregion 

#pragma region Additional Properties used in the algorithm

	std::map<int, Edge*> verticalMesh;
	std::map<int, double> verticalMax;
	std::map<int, double> verticalMin;

	double meshOuterPerimeter;
	double meshInnerPerimeter;

	Vertex* leftVertex;
	Face* leftFace;

#pragma endregion Algorithm: Division of a convex polygon into parts equal in area by the orgonal grid

#pragma region Base mesh operations

	Mesh();
	Vertex* splitEdge(Edge* e, Point* pos);
	Edge* removeVertex(Vertex*);
	std::vector<Vertex*> removeEdge(Edge* e);
	Edge* connectVertexes(Vertex* v1, Vertex* v2);
	void setEdge(Edge* edgeToSet, Vertex* v, Face* f, Edge* prev, Edge* next, Edge* sym);
	void iteratingOverTheEdgesOfFace(Face* f);

#pragma endregion

#pragma region Mesh-PolygonData Convertation 

	PolygonData convertToPolygonData();
	void convertFromPolygonData(PolygonData data);
	void convertFromPolygonDataOfConvexLeftTraversalPolygon(PolygonData data, double area);

#pragma endregion

#pragma region Computational Methods

	double areaOfFace(Face* f);
	double perimeterOfFace(Face* f);
	double getDistance(Vertex* v1, Vertex* v2);
	double crossProductLength(Point a, Point b, Point c);
	double isPointInEdge(Vertex* v1, Vertex* v2, Point* p);
	std::tuple<double, double, double> findLineCoefficients(double x1, double y1, double x2, double y2);
	std::tuple<int, double, double> findRootsOfEquation(double a, double b, double c);
	void addValidRootsToList(std::tuple<int, double, double> r1, std::vector<double>* roots);

	double findYByXAndTwoVertixes(Vertex* v1, Vertex* v2, double xPos);
	double findXByYAndTwoVertixes(Vertex* v1, Vertex* v2, double yPos);
#pragma endregion

#pragma region Methods used in the algorithm

	std::vector<std::pair<Point*, Point*>> splitFaces(double area);
	double calculateInternalPerimetrOfHorisontalPartition(std::vector<std::pair<Point*, Point*>> splitEdges, Edge* left, int numOfLeft, Edge* right, int numOfRight);
	void splitByVerticalGrid();

	void removeTempVertexesAndEdges();
	void removeEdgeWithTempVertexes(Edge* e);

	Edge* findPrevEdge(Vertex* v, Face* f);
	Edge* findNextEdge(Vertex* v, Face* f);

	double** getInternalHorizontalPerimeters();

	std::pair<double, long long> getOptimalCombinationForInternalPerimeter(double** horizontalPerimeters);
	std::vector<std::pair<Point*, Point*> > getHorizontalEdges(Edge* leftEdge, Edge* rightEdge, int numOfFacesToSplit);
	void connectPointsToEdges(std::vector<std::pair<Point*, Point*> > edges, Face* face);
	void splitMeshByMask(long long mask);

	std::pair<Point*, Point*> findPointsV(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double S);

#pragma endregion Algorithm: Division of a convex polygon into parts equal in area by the orgonal grid

#pragma region Methods used in the algorithm

void splitByVertical(double coord);
void splitHorizontal(double coord);

int isPointInPolygon(Point* P);
int isPointInPolygonTest(Point* P);

#pragma endregion Algorithm: Division of a polygon of a part along an orthogonal grid with area restrictions

};

#include "pch.h"