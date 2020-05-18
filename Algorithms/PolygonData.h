#ifdef ALGORITHMS_EXPORTS
#define ALGORITHMS_API __declspec(dllexport)
#else
#define ALGORITHMS_API __declspec(dllimport)
#endif

#pragma once

#include "Polygon.h"

namespace algo {

	typedef std::vector<double> vectorD;
	typedef std::vector<int> vectorI;

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

}

#include "pch.h"