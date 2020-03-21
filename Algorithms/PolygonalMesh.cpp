#include "pch.h"
#include "framework.h"
#include "PolygonalMesh.h"
#include <map>

const double EPS = 0.0001;

Mesh::Mesh()
{
}

void Mesh::setEdge(Edge* edgeToSet, Vertex* v, Face* f, Edge* prev, Edge* next, Edge* sym){
	edgeToSet->v = v;
	edgeToSet->f = f;
	edgeToSet->prev = prev;
	edgeToSet->next = next;
	edgeToSet->sym = sym;
}

void Mesh::iteratingOverTheEdgesOfFace(Face * f)
{
	//Iterating over the edges of face f
	Edge* start = f->e;
	Edge* e = start;
	do {
		//visit(e);
		e = e->next; // CCW order
	} while (e != start);
	//• Iterate over vertices : visit(e->v)
	//• Iterate over neighboring faces : visit(e->sym->f)
	//• For CW order : e = e->prev
}

PolygonData Mesh::convertToPolygonData() {
	PolygonData data = PolygonData();

	vectorD* x = new vectorD();
	vectorD* y = new vectorD();

	std::map<int, int> realNumberOfVertex;

	for (int i = 0; i < V.size(); ++i) {
		if (V[i]->isDeleted) {
			continue;
		}

		realNumberOfVertex[i] = x->size();
		auto point = V[i]->pos;
		x->push_back(point->x);
		y->push_back(point->y);
	}

	data.tryAddPoints(*x, *y);

	for (int i = 0; i < F.size(); ++i) {
		if (F[i]->isDeleted)
			continue;

		vectorI points;
		std::vector<int>  points_v;

		auto firstEdge = F[i]->e;
		points_v.push_back(realNumberOfVertex[firstEdge->v->numOfVertex]);
		auto cur = firstEdge->next;

		while (cur !=  firstEdge) {
			points_v.push_back(realNumberOfVertex[cur->v->numOfVertex]);
			cur = cur->next;
		}
		
		points.resize(points_v.size());

		for (int j = 0; j < points.size(); ++j) {
			points[j] = points_v[j];
		}

		data.tryAddFace(points.size(),points);
	}
	
	return data;
}

Vertex* Mesh::splitEdge(Edge* e, Point* pos) {
	if (e->sym != nullptr) {
		//1. Allocate two new edges and a vertex.
		Edge* e1 = new Edge;
		e1->numOfEdge = E.size();
		Edge* e2 = new Edge;
		e2->numOfEdge = E.size() + 1;
		Vertex* v0 = new Vertex;
		v0->numOfVertex = V.size();
		E.push_back(e1);
		E.push_back(e2);
		V.push_back(v0);
		//2. Set new attribute information, e.g.vertex position
		v0->pos = pos;

		//	3. Set and modify ptrs, be very careful!
		v0->e = e1;

		setEdge(e1, v0, e->f, e, e->next, e->sym);
		setEdge(e2, v0, e->sym->f, e->sym, e->sym->next, e);
		e->next->prev = e1;
		e->sym->next->prev = e2;
		e->next = e1;
		e->sym->next = e2;
		e->sym->sym = e1;
		e->sym = e2;

		return v0;
	}
	else {
		//1. Allocate two new edges and a vertex.
		Edge* e0 = new Edge;
		e0->numOfEdge = E.size();
		Vertex* v0 = new Vertex;
		v0->numOfVertex = V.size();
		E.push_back(e0);
		V.push_back(v0);
		//2. Set new attribute information, e.g.vertex position
		v0->pos = pos;

		//	3. Set and modify ptrs, be very careful!
		v0->e = e0;

		setEdge(e0, v0, e->f, e, e->next, nullptr);
		e->next->prev = e0;
		e->next = e0;

		return v0;
	}
}

Edge* Mesh::removeVertex(Vertex* v) {
	//works only for boundary points

	if (v->e->sym != nullptr) {
		
	}
	else {		
		Edge* toReturn = v->e->next;
		v->e->prev->next = v->e->next;
		v->e->next->prev = v->e->prev;
		
		Edge* toDelete = v->e;
		v->e = nullptr;
		v->isDeleted = true;

		if (toDelete->f->e == toDelete) {
			toDelete->f->e = toDelete->next;
		}

		toDelete->next = nullptr;
		toDelete->prev = nullptr;
		toDelete->sym = nullptr;
		toDelete->f = nullptr;
		toDelete->v = nullptr;
		toDelete->isDeleted = true;

		return toReturn;
	}

	return nullptr;
}

std::vector<Vertex*> Mesh::removeEdge(Edge * e) {
	//works only for boundary points
	Face* face = e->f;
	face->area = face->area + e->sym->f->area;
	auto edgesLength = 2 * getDistance(e->v, e->next->v);

	e->sym->f->isDeleted = true;
	e->sym->f->area = 0;

	std::vector<Vertex*> vertexes;
	vertexes.push_back(e->v);
	vertexes.push_back(e->sym->v);

	auto firstEInFaceToDelete = e->sym->f->e;
	e->sym->f->e = nullptr;
	firstEInFaceToDelete->f = face;
	
	auto eINFaceToDelete = firstEInFaceToDelete->next;
	face->e = eINFaceToDelete;
	while (eINFaceToDelete != firstEInFaceToDelete) {
		if (eINFaceToDelete->v == leftVertex) {
			leftFace = face;
		}

		eINFaceToDelete->f = face;
		eINFaceToDelete = eINFaceToDelete->next;
	}

	if (eINFaceToDelete->v == leftVertex) {
		leftFace = face;
	}

	e->next->prev = e->sym->prev;
	e->sym->prev->next = e->next;

	e->prev->next = e->sym->next;
	e->sym->next->prev = e->prev;

	if (e->f->e == e) {
		e->f->e = e->next;
	}
	e->next = nullptr;
	e->prev = nullptr;
	e->f = nullptr;
	e->v = nullptr;
	e->isDeleted = true;

	if (e->sym->f->e == e->sym) {
		e->sym->f->e = e->sym->next;
	}
	e->sym->next = nullptr;
	e->sym->prev = nullptr;
	e->sym->f = nullptr;
	e->sym->v = nullptr;
	e->sym->isDeleted = true;

	e->sym->sym = nullptr;
	e->sym = nullptr;

	return vertexes;
}

Edge* Mesh::findPrevEdge(Vertex* v, Face* f) {

	if (v->e->v == v && v->e->f == f) {
		return v->e->prev;
	}

	auto cur = f->e;
	while (cur->v != v) {
		cur = cur->next;
	}

	return cur->prev;
}

Edge* Mesh::findNextEdge(Vertex* v, Face* f) {
	if (v->e->v == v && v->e->f == f) {
		return v->e;
	}

	auto cur = f->e;
	while (cur->v != v) {
		cur = cur->next;
	}

	return cur;
}

Edge* Mesh::connectVertexes(Vertex* v1, Vertex* v2) {
	if ((v1->e->f != v2->e->f )
		&& (v2->e->sym == nullptr || v1->e->f != v2->e->sym->f)
		&& (v1->e->sym == nullptr || v2->e->f != v1->e->sym->f)
		&& ((v1->e->sym == nullptr || v2->e->sym == nullptr )|| v2->e->sym->f != v1->e->sym->f)) {
		return nullptr;
	}

	Edge* e1 = new Edge;
	e1->numOfEdge = E.size();
	Edge* e2 = new Edge;
	e2->numOfEdge = E.size() + 1;
	Face* newFace = new Face;
	newFace->numOfFace = F.size();

	E.push_back(e1);
	E.push_back(e2);
	F.push_back(newFace);

	/*auto v1Prev = findPrevEdge(v1, face);
	auto v1Next = findNextEdge(v1, face);
	auto v2Prev = findPrevEdge(v2, face);
	auto v2Next = findNextEdge(v2, face);*/

	setEdge(e1, v2, v1->e->f, v2->e->prev, v1->e, e2);
	setEdge(e2, v1, newFace, v1->e->prev, v2->e, e1);

	v1->e->prev->next = e2;
	v1->e->prev = e1;

	v2->e->prev->next = e1;
	v2->e->prev = e2;

	newFace->e = e2;
	e1->f->e = e1;

	auto cur =newFace->e->next;

	//errror !!!!!
	while (cur != newFace->e) {
		if (cur->v == leftVertex) {
			leftFace = newFace;
		}

		cur->f = newFace;
		cur = cur->next;
	}

	if (cur->v == leftVertex) {
		leftFace = newFace;
	}

	auto newArea = areaOfFace(newFace);
	newFace->area = newArea;
	e1->f->area = e1->f->area - newArea;

	meshInnerPerimeter += 2 * getDistance(e1->v, e1->next->v);

	return e1;
}

double Mesh::areaOfFace(Face* f) {
	double area = 0;
	Edge* cur = f->e;
	Edge* next = f->e->next;

	do {
		area += (cur->v->pos->x * next->v->pos->y - cur->v->pos->y * next->v->pos->x);
		cur = next;
		next = cur->next;
	} while (cur != f->e);

	area /= 2;
	return abs(area);
}

double Mesh::perimeterOfFace(Face* f) {
	double perimetr = 0.0;
	Edge* cur = f->e;
	Edge* next = f->e->next;

	do {
		perimetr += getDistance(cur->v, next->v);
		cur = next;
		next = cur->next;
	} while (cur != f->e);

	return perimetr;
}

double Mesh::getDistance(Vertex * v1, Vertex * v2) {
	return  sqrt((v1->pos->x - v2->pos->x) * (v1->pos->x - v2->pos->x) + (v1->pos->y - v2->pos->y) * (v1->pos->y - v2->pos->y));
}

double Mesh::crossProductLength(Point a, Point b, Point c)
{
	float BAx = a.x - b.x;
	float BAy = a.y - b.y;
	float BCx = c.x - b.x;
	float BCy = c.y - b.y;

	// Calculate the Z coordinate of the cross product.
	return (BAx * BCy - BAy * BCx);
}

void Mesh::convertFromPolygonData(PolygonData data) {
	/*for (int i = 0; i < data.getNumOfVertexes(); ++i) {
		Vertex* v = new Vertex;
		v->pos = new Point{ data.vertex_x[i], data.vertex_y[i] };
		v->numOfVertex = i;
		V.push_back(v);
	}

	for (int i = 1; i < data.faces.size(); ++i) {
		Face* f = new Face;

		double* res = nullptr;
		for (int j = data.faces[i - 1]; j < data.faces[i]; ++j) {
			Edge* e = new Edge;
			E.push_back(e);

			if (res == nullptr) {
				auto r = crossProductLength(
					*V[data.edges[(j % data.faces[i]) + data.faces[i - 1]]]->pos,
					*V[data.edges[((j + 1) % data.faces[i]) + data.faces[i - 1]]]->pos,
					*V[data.edges[((j + 2) % data.faces[i]) + data.faces[i - 1]]]->pos);

				if (abs(r) > EPS) {
					res = &r;
				}
			}
		}

		f->e = E[data.faces[i - 1]];
		F.push_back(f);

		for (int j = data.faces[i - 1]; j < data.faces[i]; ++j) {
			auto numOfPrevEdgeInData = data.edges[((j - 1 + data.faces[i]) % data.faces[i]) + data.faces[i - 1]];
			auto numOfNextEdgeInData = data.edges[((j + 1) % data.faces[i]) + data.faces[i - 1]];

			if (*res > 0) {
				setEdge(
					E[j],
					V[data.edges[j]],
					f,
					E[numOfNextEdgeInData],
					E[numOfPrevEdgeInData],
					nullptr);

				if (V[data.edges[j]]->e == nullptr) {
					V[data.edges[j]]->e = E[j];
				}
			}
			else if (*res < 0) {
				setEdge(
					E[j],
					V[numOfNextEdgeInData],
					f,
					E[numOfPrevEdgeInData],
					E[numOfNextEdgeInData],
					nullptr);
				if (V[numOfNextEdgeInData]->e == nullptr) {
					V[numOfNextEdgeInData]->e = E[j];
				}
			}

		}
	}*/
}

std::tuple<double, double, double> Mesh::findCoeff(double x1, double y1, double x2, double y2) {
	return std::make_tuple(y1 - y2, x2 - x1, x1 * y2 - x2 * y1);
}

std::tuple<int, double, double> Mesh::findRoots(double a, double b, double c) {
	int numOfRoots;
	double x1, x2;

	if (abs(a) < EPS) {
		numOfRoots = 1;
		x1 = -c / b;
	}
	else {
		auto D = b * b - 4 * a * c;

		if (D < 0) {
			numOfRoots = 0;
		}
		else if (std::abs(D) < EPS) {
			numOfRoots = 1;
			x1 = (-b) / (2 * a);
		}
		else {
			numOfRoots = 2;
			auto ds = sqrt(D);
			x1 = (-b + ds) / (2 * a);
			x2 = (-b - ds) / (2 * a);
		}
	}
	return std::make_tuple(numOfRoots, x1, x2);
}

void Mesh::addRoots(std::tuple<int, double, double> r1, std::vector<double>* roots) {
	if (std::get<0>(r1) == 0)
		return;
	roots->push_back(std::get<1>(r1));

	if (std::get<0>(r1) == 1)
		return;
	roots->push_back(std::get<2>(r1));
}

std::pair<Point*,Point*> Mesh::findPointsV(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double S) {
	//1-leftBottom
	//2-leftTop
	//3-rightTop
	//4-rightBottom
	auto coeff1 = findCoeff(x4, y4, x1, y1), coeff2 = findCoeff(x2, y2, x3, y3);
	double a1 = std::get<0>(coeff1), b1 = std::get<1>(coeff1), c1 = std::get<2>(coeff1);
	double a2 = std::get<0>(coeff2), b2 = std::get<1>(coeff2), c2 = std::get<2>(coeff2);
	auto k1 = -b1;
	auto k2 = -b2;
	auto a = b1 * a2 - a1 * b2;
	auto b = b2 * c1 - b1 * c2 + y1 * k2 * b1 + x1 * k2 * a1 - x2 * k1 * a2 - y2 * k1 * b2;
	auto c = k1 * k2 * (x1 * y2 - x2 * y1) + x2 * k1 * c2 - x1 * k2 * c1;
	auto c_pos = c + 2 * S * k1 * k2;
	auto c_neg = c - 2 * S * k1 * k2;

	auto roots1 = findRoots(a, b, c_neg);
	auto roots2 = findRoots(a, b, c_pos);

	std::vector<double>* roots = new std::vector<double>();
	addRoots(roots1, roots);
	addRoots(roots2, roots);

	for (int i = 0; i < roots->size(); ++i) {
		double c3 = (*roots)[i];
		double x5 = (c3 * b2) / k2,
			y5 = (c2 - c3 * a2) / k2,
			x6 = (c3 * b1) / k1,
			y6 = (c1 - c3 * a1) / k1;

		auto check_c3 = x5 * y6 - x6 * y5;
		auto b3_check = x6 - x5;
		auto a3_check = y5 - y6;

		if (abs(b3_check) > EPS && std::abs((check_c3 / b3_check) - c3) > EPS) {
			continue;
		}
		else if (abs(a3_check) > EPS && ((x2 <= x5 && x3 <= x5) || (x2 >= x5 && x3 >= x5) || (x1 <= x6 && x4 <= x6) || (x1 >= x6 && x4 >= x6))) {
			continue;
		}
		else {
			return std::make_pair <Point*,Point* >(new Point{x5, y5}, new Point{x6, y6});

		}
	}

	return std::make_pair <Point*, Point* >(nullptr, nullptr);
}

std::vector<std::pair<Point*, Point*>> Mesh::splitFaces(double area) {
	std::vector<std::pair<Point*, Point*>> splittedEdges;
	Face* curFace = leftFace;
	double needArea = area;

	int numOfEdge = 0;
	verticalMesh[numOfEdge++] = leftVertex->e;

	auto* next = findNextEdge(leftVertex, leftFace);
	auto* prev = findPrevEdge(leftVertex, leftFace);

	while (abs(next->next->v->pos->x - leftVertex->pos->x) < EPS) {
		next = next->next;
	}

	while (abs(prev->v->pos->x - leftVertex->pos->x) < EPS) {
		prev = prev->prev;
	}
	

	while (true) {
		if (curFace->area > needArea + EPS) {
			auto points = findPointsV(
				next->v->pos->x, next->v->pos->y,
				prev->next->v->pos->x, prev->next->v->pos->y,
				prev->v->pos->x, prev->v->pos->y,
				next->next->v->pos->x, next->next->v->pos->y,
				needArea);
			splittedEdges.push_back(points);

			if (points.first != nullptr && points.second != nullptr) {
				Edge* newE = nullptr;
				if (points.first->y < points.second->y) {
					auto v1 = splitEdge(next, points.first);
					v1->isTemp = true;
					auto v2 = splitEdge(prev, points.second);
					v2->isTemp = true;
					newE = connectVertexes(v1, v2);
					verticalMax[numOfEdge] = points.first->y > points.second->y ? points.first->y : points.second->y;
					verticalMin[numOfEdge] = points.first->y < points.second->y ? points.first->y : points.second->y;
				}
				else {
					auto v1 = splitEdge(next, points.second);
					v1->isTemp = true;
					auto v2 = splitEdge(prev, points.first);
					v2->isTemp = true;
					newE = connectVertexes(v1, v2);
					verticalMax[numOfEdge] = points.first->y > points.second->y ? points.first->y : points.second->y;
					verticalMin[numOfEdge] = points.first->y < points.second->y ? points.first->y : points.second->y;
				}

				if (newE->f == verticalMesh[numOfEdge - 1]->f && newE->sym != nullptr) {
					verticalMesh[numOfEdge] = newE->sym;
				}
				else {
					verticalMesh[numOfEdge] = newE;
				}

				numOfEdge++;
				
				if (newE->next == next) {
					curFace = newE->sym->f;
					prev = newE->sym->prev;
					next = newE->sym->next;
				}
				else {
					curFace = newE->f;
					prev = newE->prev;
					next = newE->next;
				}
			}

			needArea = area;
			continue;
		}
		else if (curFace->area + EPS < needArea) {
			needArea -= curFace->area;
		}
		else {
			/*prev->next->v->isTemp = false;
			next->v->isTemp = false;
			prev->next->isTemp = false;*/

			//means we destinate border
			if (prev->prev->sym == nullptr) {
				verticalMesh[numOfEdge] = prev->prev;
				break;
			}
			else {
				splittedEdges.push_back(std::make_pair(
					new Point{ prev->prev->v->pos->x,  prev->prev->v->pos->y}, 
					new Point{ prev->v->pos->x,prev->v->pos->y }));

				prev->prev->isTemp = false;
				if (prev->prev->sym != nullptr) {
					prev->prev->sym->isTemp = false;
				}

				if (prev->prev->f == verticalMesh[numOfEdge - 1]->f && prev->prev->sym != nullptr) {
					verticalMesh[numOfEdge] = prev->prev->sym;
					verticalMax[numOfEdge] = prev->prev->v->pos->y > prev->prev->sym->v->pos->y 
						? prev->prev->v->pos->y : prev->prev->sym->v->pos->y;
					verticalMin[numOfEdge] = prev->prev->v->pos->y < prev->prev->sym->v->pos->y 
						? prev->prev->v->pos->y : prev->prev->sym->v->pos->y;
				}
				else {
					verticalMesh[numOfEdge] = prev->prev;
				}

				numOfEdge++;
			}

			/*prev->prev->sym->isTemp = false;*/
			needArea = area;
		}

		if (prev->prev->sym != nullptr) {
			curFace = prev->prev->sym->f;
			prev = prev->prev->sym->prev;
			next = next->next->sym->next;
		}
		else {
			//////
			break;
		}
	}

	removeTempVertexesAndEdges();

	return splittedEdges;
}

void Mesh::convertFromPolygonDataOfConvexLeftTraversalPolygon(PolygonData data, double area) {
	leftVertex = nullptr;

	for (int i = 0; i < data.getNumOfVertexes(); ++i) {
		Vertex* v = new Vertex;
		v->pos = new Point{ data.vertex_x[i], data.vertex_y[i] };
		v->numOfVertex = i;
		V.push_back(v);

		if (leftVertex == nullptr) {
			leftVertex = v;
		}

		if (data.vertex_x[i] < data.vertex_x[leftVertex->numOfVertex]) {
			leftVertex = V[i];;
		}
	}

	Face* f = new Face;
	f->area = area;
	f->numOfFace = 0;

	for (int j = 0; j < data.getNumOfVertexes(); ++j) {
		Edge* e = new Edge;
		e->numOfEdge = j;
		E.push_back(e);
	}

	f->e = E[0];
	F.push_back(f);

	leftFace = F[0];

	for (int j = 0; j < data.getNumOfVertexes(); ++j) {
		auto numOfPrevEdgeInData = (j- 1 + data.getNumOfVertexes()) % data.getNumOfVertexes();
		auto numOfNextEdgeInData = (j + 1) % data.getNumOfVertexes();

		setEdge(
			E[j],
			V[j],
			f,
			E[numOfPrevEdgeInData],
			E[numOfNextEdgeInData],
			nullptr);

		V[j]->e = E[j];
	}
}

void Mesh::splitByVerticalGrid() {
	Vertex* leftPoint = V[leftVertex->numOfVertex];

	auto* nextEdge = findNextEdge(leftPoint, F[0]);
	auto* prevEdge = findPrevEdge(leftPoint, F[0]);

	while (abs(nextEdge->next->v->pos->x - leftPoint->pos->x) < EPS) {
		nextEdge = nextEdge->next;
	}

	while (abs(prevEdge->v->pos->x - leftPoint->pos->x) < EPS) {
		prevEdge = prevEdge->prev;
	}

	while (nextEdge != prevEdge && nextEdge->next != prevEdge) {
		Edge* newEdge = nullptr;
		Vertex* newV = nullptr;
		if (prevEdge->v->pos->x - nextEdge->next->v->pos->x > EPS) {
			newV = splitEdge(prevEdge, new Point{ nextEdge->next->v->pos->x, findYByXAndTwoVertixes(prevEdge->v,prevEdge->next->v, nextEdge->next->v->pos->x) });
			newV->isTemp = true;
			newEdge = connectVertexes(newV, nextEdge->next->v);
			newEdge->isTemp = true;
			//verticalMesh[nextEdge->next->v->pos->x] = newEdge;
		}
		else if (nextEdge->next->v->pos->x - prevEdge->v->pos->x > EPS) {
			newV = splitEdge(nextEdge, new Point{ prevEdge->v->pos->x, findYByXAndTwoVertixes(nextEdge->v,nextEdge->next->v,prevEdge->v->pos->x) });
			newV->isTemp = true;
			newEdge = connectVertexes(newV, prevEdge->v);
			newEdge->isTemp = true;
			//verticalMesh[prevEdge->v->pos->x] = newEdge;
		}
		else {
			nextEdge = nextEdge->next;
			prevEdge = prevEdge->prev;
			leftPoint = prevEdge->v;
			continue;
		}

		if (newEdge->sym->prev->v->pos->x - newEdge->prev->v->pos->x > EPS) {
			newEdge = newEdge->sym;
		}

		nextEdge = newEdge->next;
		prevEdge = newEdge->prev;
		leftPoint = newV;
	}
}

double Mesh::findYByXAndTwoVertixes(Vertex* v1, Vertex* v2, double xPos) {
	auto k = (v1->pos->y - v2->pos->y) / (v1->pos->x - v2->pos->x);
	auto b = v1->pos->y - k * v1->pos->x;
	return k * xPos + b;
}

double Mesh::isPointInEdge(Vertex* v1, Vertex* v2, Point * p) {


	if (abs(v1->pos->x - v2->pos->x) < EPS) {
		return abs(p->x - v1->pos->x) < EPS
			&& (p->y - max(v1->pos->y, v2->pos->y)) < EPS
			&& (min(v1->pos->y, v2->pos->y) - p->y) < EPS;
	}

	if (abs(v1->pos->y - v2->pos->y) < EPS) {
		return abs(p->y - v1->pos->y) < EPS
			&& (p->x - max(v1->pos->x, v2->pos->x)) < EPS
			&& (min(v1->pos->x, v2->pos->x) - p->x) < EPS;
	}

	auto k = (v1->pos->y - v2->pos->y) / (v1->pos->x - v2->pos->x);
	auto b = v1->pos->y - k * v1->pos->x;
	return abs(p->y -( k * p->x + b)) < EPS;
}

void Mesh::removeTempVertexesAndEdges() {
	for (int i = 0; i < E.size(); ++i) {
		if (!E[i]->isTemp)
			continue;

		auto vertexes = removeEdge(E[i]);
		if (vertexes[0] != nullptr && vertexes[0]->isTemp) {
			removeVertex(vertexes[0]);
		}

		if (vertexes[1] != nullptr && vertexes[1]->isTemp) {
			removeVertex(vertexes[1]);
		}
	}
}

double ** Mesh::getInternalHorizontalPerimeters() {
	int numOfEdges = verticalMesh.size() - 2;

	double** perimeters = new double*[numOfEdges + 1];

	//how much vertical edges will be removed
	for (int i = 1; i <= numOfEdges; ++i) {
		perimeters[i] = new double[numOfEdges + 2 - i];
		//from which edge start removal
		for (int j = 1; j <= numOfEdges + 1 - i; ++j) {
			auto leftEdge = verticalMesh[j - 1];
			auto rightEdge = verticalMesh[j + i];
			if (j + i != numOfEdges + 1) {
				rightEdge = rightEdge->sym;
			}
			auto splittedEdges = getHorizontalEdges(leftEdge, rightEdge, i + 1);
			perimeters[i][j] = calculateInternalPerimetrOfHorisontalPartition(splittedEdges, leftEdge,j-1, rightEdge, j+i);
		}
	}

	return perimeters;
}

std::vector<std::pair<Point*, Point*> > Mesh::getHorizontalEdges(Edge * leftEdge, Edge * rightEdge, int numOfFacesToSplit) {
	vectorD* x = new vectorD(), * y = new vectorD();
	vectorI edgesData;

	int pointNum = 0;
	auto cur = leftEdge;
	do {
		x->push_back(cur->v->pos->x);
		y->push_back(cur->v->pos->y);
		edgesData.push_back(pointNum);
		pointNum++;

		if (cur->next->sym != nullptr && cur->next != rightEdge && cur->next != rightEdge->sym) {
			cur = cur->next->sym->next;
		}
		else {
			cur = cur->next;
		}
	} while (cur->v != leftEdge->v && cur->sym != leftEdge);

	Rotation::rotateFigure90(x, y);
	std::reverse(x->begin(), x->end());
	std::reverse(y->begin(), y->end());

	auto data = PolygonData();
	data.vertex_x = *x;
	data.vertex_y = *y;
	data.tryAddFace(edgesData.size(), edgesData);

	auto testMesh = Mesh();
	testMesh.convertFromPolygonDataOfConvexLeftTraversalPolygon(data, leftEdge->f->area * numOfFacesToSplit);
	testMesh.splitByVerticalGrid();
	auto splittedEdges = testMesh.splitFaces(leftEdge->f->area);

	for (int i = 0; i < splittedEdges.size(); ++i) {
		Rotation::rotatePoint90(splittedEdges[i].first);
		Rotation::rotatePoint90(splittedEdges[i].second);
	}

	return splittedEdges;
}

double Mesh::calculateInternalPerimetrOfHorisontalPartition(std::vector<std::pair<Point*, Point*>> splitEdges, Edge* left,int numOfLeft, Edge* right, int numOfRight) {
	double totalPer = 0.0;

	double curLeftY, curRightY, maxLeft, maxRight;
	
	if (numOfLeft != 0) {
		curLeftY = verticalMin[numOfLeft];
		maxLeft = verticalMax[numOfLeft];
	}
	if (numOfRight != verticalMesh.size() - 1) {
		curRightY = verticalMin[numOfRight];
		maxRight = verticalMax[numOfRight];
	}

	for (int i = 0; i < splitEdges.size() + 1; ++i) {
		double per = 0.0;
		
		if (i > 0) {
			per += abs(splitEdges[i - 1].first->x - splitEdges[i - 1].second->x);
		}
		if (i < splitEdges.size()) {
			per += abs(splitEdges[i].first->x - splitEdges[i].second->x);
		}

		if (numOfLeft != 0) {
			if (i < splitEdges.size()) {
				auto leftPointY = splitEdges[i].first->x < splitEdges[i].second->x ? splitEdges[i].first->y : splitEdges[i].second->y;

				if (curLeftY < leftPointY && leftPointY < maxLeft) {
					per += leftPointY - curLeftY;
					curLeftY = leftPointY;
				}
			}
			else if(curLeftY < maxLeft){
				per += (maxLeft - curLeftY);
			}
		}
		if (numOfRight != verticalMesh.size() - 1) {
			if (i < splitEdges.size()) {
				auto rightPointY = splitEdges[i].first->x > splitEdges[i].second->x ? splitEdges[i].first->y : splitEdges[i].second->y;

				if (curRightY < rightPointY && rightPointY < maxRight) {
					per += rightPointY - curRightY;
					curRightY = rightPointY;
				}
			}
			else if (curRightY < maxRight) {
				per += (maxRight - curRightY);
			}
		}

		totalPer += per * per;
	}

	return totalPer;
}

std::pair<double, long long> Mesh::getOptimalCombinationForInternalPerimeter(double** horizontalPerimeters) {
	long long numOfCombinations = pow(2, verticalMesh.size() - 2);
	int numOfEdges = verticalMesh.size() - 2;
	double optimal = DBL_MAX;
	long long optimalMask = -1;

	for (long long i = 0; i < numOfCombinations; ++i) {

		double totalPer = 0.0;

		long long num = i;
		int cur = num & 1;
		int startZeroPos = -1;
		int prev;

		if (cur == 1) {
			totalPer += (verticalMax[numOfEdges] - verticalMin[numOfEdges]) * (verticalMax[numOfEdges] - verticalMin[numOfEdges]);
		}
		else {
			startZeroPos = numOfEdges;
		}

		for (int j = numOfEdges - 1; j > 0; --j) {
			prev = cur;
			num = num >> 1;
			cur = num & 1;

			if (prev == cur && prev == 1) {
				totalPer += ((verticalMax[j] - verticalMin[j]) + (verticalMax[j + 1] - verticalMin[j + 1]))
					* ((verticalMax[j] - verticalMin[j]) + (verticalMax[j + 1] - verticalMin[j + 1]));
			}
			else if (prev == cur && prev == 0) {

			}
			else if (prev != cur && cur == 0) {
				startZeroPos = j;
			}
			else if (prev != cur && cur == 1) {
				totalPer += horizontalPerimeters[startZeroPos - j][j + 1];
			}
		}

		if (cur == 0) {
			totalPer += horizontalPerimeters[startZeroPos][1];
		}
		else if (cur == 1) {
			totalPer += (verticalMax[1] - verticalMin[1]) * (verticalMax[1] - verticalMin[1]);
		}

		if (totalPer < optimal) {
			optimal = totalPer;
			optimalMask = i;
		}
	}

	return std::make_pair(optimal, optimalMask);
}

void Mesh::splitMeshByMask(long long mask) {
	int numOfEdges = verticalMesh.size() - 2;

	long long num = mask;
	int cur = num & 1;
	int startZeroPos = -1;
	int prev;

	if (cur == 0) {
		startZeroPos = numOfEdges;
	}

	std::vector<int> edgesToRemove;
	std::vector<std::pair<std::vector<std::pair<Point*, Point*>>, int>> edgesToAdd;
	for (int i = numOfEdges - 1; i > 0; --i) {
		prev = cur;
		num = num >> 1;
		cur = num & 1;

		if (prev == cur && cur == 0) {
		}
		else if (prev != cur && cur == 0) {
			startZeroPos = i;
		}
		else if (prev != cur && cur == 1) {
			auto edges = getHorizontalEdges(verticalMesh[i], verticalMesh[startZeroPos + 1], startZeroPos - i + 1);
			edgesToAdd.push_back(std::make_pair(edges, i));
			for (int k = i + 1; k < startZeroPos + 1; ++k) {
				edgesToRemove.push_back(k);
				//	removeEdgeWithTempVertexes(verticalMesh[k]);
			}
			//connectPointsToEdges(edges, verticalMesh[i]->f);
		}
	}

	if (cur == 0) {
		auto edges = getHorizontalEdges(verticalMesh[0], verticalMesh[startZeroPos + 1], startZeroPos + 1);
		edgesToAdd.push_back(std::make_pair(edges, 0));
		for (int k = 1; k < startZeroPos + 1; ++k) {
			edgesToRemove.push_back(k);
			//	removeEdgeWithTempVertexes(verticalMesh[k]);
		}
		//connectPointsToEdges(edges, verticalMesh[0]->f);
	}
	for (int i = 0; i < edgesToRemove.size(); ++i) {
		removeEdgeWithTempVertexes(verticalMesh[edgesToRemove[i]]);
	}
	for (int i = 0; i < edgesToAdd.size(); ++i) {
		connectPointsToEdges(edgesToAdd[i].first, verticalMesh[edgesToAdd[i].second]->f);
	}
}

void Mesh::connectPointsToEdges(std::vector<std::pair<Point*, Point*> > edges, Face * face) {
	std::vector<std::pair<Vertex*, Vertex*> > vertexes = std::vector<std::pair<Vertex*, Vertex*> >();

	for (int i = 0; i < edges.size(); ++i) {
		Vertex* f = nullptr, * s = nullptr;
		auto cur = face->e;
		do {
			if (isPointInEdge(cur->v, cur->next->v, edges[i].first)) {
				f = splitEdge(cur, new Point{ edges[i].first->x ,  edges[i].first->y });
				break;
			}

			cur = cur->next;

		} while (cur->v != face->e->v);

		cur = face->e;
		do {
			if (isPointInEdge(cur->v, cur->next->v, edges[i].second)) {
				s = splitEdge(cur, new Point{ edges[i].second->x ,  edges[i].second->y });
				break;
			}

			cur = cur->next;

		} while (cur->v != face->e->v);
		
		if (f != nullptr && s != nullptr) {
			vertexes.push_back(std::make_pair(f, s));
		}
	}

	for (int i = 0; i < vertexes.size(); ++i) {
		connectVertexes(vertexes[i].first, vertexes[i].second);
	}

}

void Mesh::removeEdgeWithTempVertexes(Edge* e) {
	auto vertexes = removeEdge(e);

	if (vertexes[0]->isTemp) {
		removeVertex(vertexes[0]);
	}

	if (vertexes[1]->isTemp) {
		removeVertex(vertexes[1]);
	}
}