#include "pch.h"
#include "PolygonData.h"

namespace algo {

	PolygonData::PolygonData() {
		faces.push_back(0);
	}

	int PolygonData::getNumOfVertexes() {
		return vertex_x.size();
	}

	int PolygonData::getNumOfFaces() {
		return faces.size() - 1;
	}

	bool PolygonData::tryAddPoints(vectorD x, vectorD y) {
		if (x.size() != y.size()) {
			return false;
		}

		for (int i = 0; i < x.size(); ++i) {
			vertex_x.push_back(x[i]);
			vertex_y.push_back(y[i]);
		}

		return true;
	}

	bool PolygonData::tryAddFace(int numOfSides, vectorI pointsNumbers) {
		if (numOfSides != pointsNumbers.size()) {
			return false;
		}

		for (int i = 0; i < pointsNumbers.size(); ++i) {
			if (pointsNumbers[i] >= vertex_x.size()) {
				return false;
			}
		}

		for (int i = 0; i < pointsNumbers.size(); ++i) {
			edges.push_back(pointsNumbers[i]);
		}

		faces.push_back(edges.size());

		return true;
	}

}