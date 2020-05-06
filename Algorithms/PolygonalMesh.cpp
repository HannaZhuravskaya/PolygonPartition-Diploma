#include "pch.h"
#include "framework.h"
#include "PolygonalMesh.h"
#include <map>
#include <set>

#pragma region Base mesh operations

Mesh::Mesh()
{
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

std::vector<Vertex*> Mesh::removeEdge(Edge* e) {
	//works only for boundary points
	Face* face = e->f;
	face->area = face->area + e->sym->f->area;
	auto edgesLength = 2 * getDistance(e->v, e->next->v);
	face->perimeter = face->perimeter + e->sym->f->perimeter - edgesLength;

	e->sym->f->isDeleted = true;
	e->sym->f->area = 0;
	e->sym->f->perimeter = 0;

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

// Vertexes can be connected if there are edges that have beginning at these vertexes and belong to the same face.
Edge* Mesh::connectVertexes(Vertex* v1, Vertex* v2) {
	Edge* edgeOfv1 = nullptr;
	Edge* edgeOfv2 = nullptr;

#pragma region Edges from v1
	std::map<Face*, Edge*> v1Faces;

	Edge* firstEdge = nullptr; // edge which starts from v1

	// if edge in v1 starts from v1
	if (v1->e->v == v1) {
		firstEdge = v1->e;
	} // if edge in v1 ends in v1
	else if (v1->e->next->v == v1) {
		firstEdge = v1->e->next;
	}
	else {
		return nullptr;
	}

	v1Faces.insert({ firstEdge->f, firstEdge });

	//go to the right until return to the first edge or to the border of mesh
	auto curr = firstEdge->sym;
	while (curr != nullptr && curr != firstEdge->sym) {
		v1Faces.insert({ curr->f, curr->next });
		curr = curr->next->sym;
	}

	//go to the left until return to the first edge or to the border of mesh
	curr = firstEdge->prev->sym;
	while (curr != nullptr && curr != firstEdge) {
		v1Faces.insert({ curr->f, curr });
		curr = curr->prev->sym;
	}
#pragma endregion

#pragma region Edges from v2
	std::map<Face*, Edge*> v2Faces;

	// if edge in v2 starts from v2
	if (v2->e->v == v2) {
		firstEdge = v2->e;
	} // if edge in v2 ends in v2
	else if (v2->e->next->v == v2) {
		firstEdge = v2->e->next;
	}
	else {
		return nullptr;
	}

	v2Faces.insert({ firstEdge->f, firstEdge });

	//go to the right until return to the first edge or to the border of mesh
	curr = firstEdge->sym;
	while (curr != nullptr && curr != firstEdge->sym) {
		v2Faces.insert({ curr->f, curr });
		curr = curr->next->sym;
	}

	//go to the left until return to the first edge or to the border of mesh
	curr = firstEdge->prev->sym;
	while (curr != nullptr && curr != firstEdge) {
		v2Faces.insert({ curr->f, curr });
		curr = curr->prev->sym;
	}
#pragma endregion

#pragma region Find edges that have beginning at vertexes v1 and v2 and belong to the same face
	for (auto var1 : v1Faces)
	{
		if (v2Faces.find(var1.first) != v2Faces.end()) {

			edgeOfv1 = var1.second;
			edgeOfv2 = v2Faces[var1.first];
		}
	}

	if (edgeOfv1 == nullptr || edgeOfv2 == nullptr)
		return nullptr;
#pragma endregion

	Edge* e1 = new Edge;
	e1->numOfEdge = E.size();
	Edge* e2 = new Edge;
	e2->numOfEdge = E.size() + 1;
	Face* newFace = new Face;
	newFace->numOfFace = F.size();

	E.push_back(e1);
	E.push_back(e2);
	F.push_back(newFace);

	setEdge(e1, v2, edgeOfv1->f, edgeOfv2->prev, edgeOfv1, e2);
	setEdge(e2, v1, newFace, edgeOfv1->prev, edgeOfv2, e1);

	edgeOfv1->prev->next = e2;
	edgeOfv1->prev = e1;

	edgeOfv2->prev->next = e1;
	edgeOfv2->prev = e2;

	newFace->e = e2;
	e1->f->e = e1;

	// change face references for edges in newFace and calculate area of newFace.
	double area = 0;
	newFace->perimeter = 0.0;
	auto cur = newFace->e;
	do {
		area += (cur->v->pos->x * cur->next->v->pos->y - cur->v->pos->y * cur->next->v->pos->x);

		auto edgeLength = getDistance(cur->v, cur->next->v);
		if (cur == newFace->e) {
			newFace->perimeter += edgeLength;
			newFace->e->sym->f->perimeter += edgeLength;
		}
		else {
			newFace->perimeter += edgeLength;
			newFace->e->sym->f->perimeter -= edgeLength;
		}

		if (cur->v == leftVertex) {
			leftFace = newFace;
		}

		cur->f = newFace;
		cur = cur->next;
	} while (cur != newFace->e);

	area /= 2;
	area = abs(area);

	if (cur->v == leftVertex) {
		leftFace = newFace;
	}

	newFace->area = area;
	e1->f->area = abs(e1->f->area - area);

	meshInnerPerimeter += 2 * getDistance(e1->v, e1->next->v);

	return e1;
}

void Mesh::setEdge(Edge* edgeToSet, Vertex* v, Face* f, Edge* prev, Edge* next, Edge* sym) {
	edgeToSet->v = v;
	edgeToSet->f = f;
	edgeToSet->prev = prev;
	edgeToSet->next = next;
	edgeToSet->sym = sym;
}

void Mesh::iteratingOverTheEdgesOfFace(Face* f)
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

Mesh* Mesh::copy(Mesh* m) {

	Mesh* copy = new Mesh();

	for (int i = 0; i < m->V.size(); ++i) {
		copy->V.push_back(new Vertex());
		copy->V[i]->numOfVertex = i;
	}

	for (int i = 0; i < m->E.size(); ++i) {
		copy->E.push_back(new Edge());
		copy->E[i]->numOfEdge = i;
	}

	for (int i = 0; i < m->F.size(); ++i) {
		copy->F.push_back(new Face());
		copy->F[i]->numOfFace = i;
	}

	for (int i = 0; i < m->V.size(); ++i) {
		auto v = m->V[i];
		auto vCopy = copy->V[i];
		vCopy->pos = new Point{ v->pos->x, v->pos->y };
		vCopy->e = copy->E[v->e->numOfEdge];
		vCopy->isDeleted = v->isDeleted;
		vCopy->isTemp = v->isTemp;
	}

	for (int i = 0; i < m->E.size(); ++i) {
		auto e = m->E[i];
		auto eCopy = copy->E[i];
		eCopy->v = copy->V[e->v->numOfVertex];
		eCopy->f = copy->F[e->f->numOfFace];
		eCopy->prev = copy->E[e->prev->numOfEdge];
		eCopy->next = copy->E[e->next->numOfEdge];

		if(e->sym != nullptr)
			eCopy->sym = copy->E[e->sym->numOfEdge];

		eCopy->isDeleted = e->isDeleted;
		eCopy->isTemp = e->isTemp;
	}

	for (int i = 0; i < m->F.size(); ++i) {
		auto f = m->F[i];
		auto fCopy = copy->F[i];
		fCopy->e = copy->E[f->e->numOfEdge];
		fCopy->isDeleted = f->isDeleted;
		fCopy->area = f->area;
		fCopy->perimeter = f->perimeter;
	}

	for (int i = 0; i < m->originalVertexes.size(); ++i) {
		copy->originalVertexes.push_back(copy->V[m->originalVertexes[i]->numOfVertex]);
	}

	return copy;
}

#pragma endregion

#pragma region Mesh-PolygonData Convertation 

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

void Mesh::convertFromPolygonDataOfConvexLeftTraversalPolygon(PolygonData data) {
	leftVertex = nullptr;
	rightVertex = nullptr;

	double area = 0.0;
	double perimeter = 0.0;
	int size = data.getNumOfVertexes();

	for (int i = 0; i < data.getNumOfVertexes(); ++i) {
		auto x1 = data.vertex_x[i], y1 = data.vertex_y[i], x2 = data.vertex_x[(i + 1) % size], y2 = data.vertex_y[(i + 1) % size];
		area += x1 * y2 - y1 * x2;
		perimeter += sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

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

		if (rightVertex == nullptr) {
			rightVertex = v;
		}

		if (data.vertex_x[i] > data.vertex_x[rightVertex->numOfVertex]) {
			rightVertex = V[i];;
		}

		this->originalVertexes.push_back(this->V[i]);
	}

	area /= 2;
	area = abs(area);

	Face* f = new Face;
	f->area = area;
	f->perimeter = perimeter;
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
		auto numOfPrevEdgeInData = (j - 1 + data.getNumOfVertexes()) % data.getNumOfVertexes();
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

#pragma endregion

#pragma endregion

std::vector<std::string> splitString(const std::string& stringToSplit, const std::string& regexPattern)
{
	std::vector<std::string> result;

	const std::regex rgx(regexPattern);
	std::sregex_token_iterator iter(stringToSplit.begin(), stringToSplit.end(), rgx, -1);

	for (std::sregex_token_iterator end; iter != end; ++iter)
	{
		result.push_back(iter->str());
	}

	return result;
}

std::string Mesh::convertToString() {
	std::stringstream ss;

	ss << "V: " << V.size() << ", E: " << E.size() << ", F: " << F.size() << "\n";

	for (int i = 0; i < V.size(); ++i) {
		if (!V[i]->isDeleted)
			ss << "numOfVertex: " << V[i]->numOfVertex << ", (" << V[i]->pos->x << ", " << V[i]->pos->y << "), e: " << V[i]->e->numOfEdge
			<< ", isDeleted: " << V[i]->isDeleted << ", " << "isTemp: " << V[i]->isTemp << "\n";
		else {
			ss << "numOfVertex: " << V[i]->numOfVertex << ", isDeleted: " << V[i]->isDeleted << "\n";
		}
	}

	for (int i = 0; i < E.size(); ++i) {
		if (!E[i]->isDeleted) {
			ss << "numOfEdge: " << E[i]->numOfEdge << ", v: " << E[i]->v->numOfVertex << ", f: " << E[i]->f->numOfFace
				<< ", prev: " << E[i]->prev->numOfEdge << ", next: " << E[i]->next->numOfEdge;

			if (E[i]->sym != nullptr) {
				ss << ", sym: " << E[i]->sym->numOfEdge;
			}

			ss << ", isDeleted: " << E[i]->isDeleted << ", " << "isTemp: " << E[i]->isTemp << "\n";
		}
		else {
			ss << "numOfEdge: " << E[i]->numOfEdge << ", isDeleted: " << E[i]->isDeleted << "\n";
		}
	}

	for (int i = 0; i < F.size(); ++i) {
		if (!F[i]->isDeleted) {
			ss << "numOfFace: " << F[i]->numOfFace << ", e: " << F[i]->e->numOfEdge << ", isDeleted: " << F[i]->isDeleted
				<< ", area: " << F[i]->area << ", perimeter: " << F[i]->perimeter << "\n";
		}
		else {
			ss << "numOfFace: " << F[i]->numOfFace << ", isDeleted: " << F[i]->isDeleted << "\n";
		}
	}

	ss << originalVertexes.size() << "\n";
	for (int i = 0; i < originalVertexes.size(); ++i) {
		ss << originalVertexes[i]->numOfVertex;
		if (i != originalVertexes.size() - 1)
			ss << "->";
		else
			ss << "\n";
	}

	return ss.str();
}

Mesh* Mesh::convertFromString(std::string stringData) {
	try {
		std::stringstream ss(stringData);
		std::string to;
		std::vector<std::string> data;

		Mesh* mesh = new Mesh();

		std::getline(ss, to);
		data = splitString(to, "V: |, E: |, F: ");

		int numOfV = std::stoi(data.at(1));

		for (int i = 0; i < numOfV; ++i) {
			mesh->V.push_back(new Vertex());
			mesh->V[i]->numOfVertex = i;
		}

		int numOfE = std::stoi(data.at(2));
		for (int i = 0; i < numOfE; ++i) {
			mesh->E.push_back(new Edge());
			mesh->E[i]->numOfEdge = i;
		}

		int numOfF = std::stoi(data.at(3));
		for (int i = 0; i < numOfF; ++i) {
			mesh->F.push_back(new Face());
			mesh->F[i]->numOfFace = i;
		}

		for (int i = 0; i < numOfV; ++i) {
			std::getline(ss, to);
			data = splitString(to, "[^0-9.]+");
			int cnt = 1;

			if (data.size() == 7) {
				auto v = mesh->V[std::stoi(data.at(cnt++))];
				v->pos = new Point{ std::stod(data.at(cnt++)), std::stod(data.at(cnt++)) };
				v->e = mesh->E[std::stoi(data.at(cnt++))];
				v->isDeleted = std::stoi(data.at(cnt++));
				v->isTemp = std::stoi(data.at(cnt++));
			}
			else {
				auto v = mesh->V[std::stoi(data.at(cnt++))];
				v->pos = nullptr;
				v->e = nullptr;
				v->isDeleted = mesh->V[std::stoi(data.at(cnt++))];
				v->isTemp = false;
			}
		}

		for (int i = 0; i < numOfE; ++i) {
			std::getline(ss, to);
			data = splitString(to, "[^0-9]+");
			int cnt = 1;

			if (data.size() >= 8) {
				auto e = mesh->E[std::stoi(data.at(cnt++))];
				e->v = mesh->V[std::stoi(data.at(cnt++))];
				e->f = mesh->F[std::stoi(data.at(cnt++))];
				e->prev = mesh->E[std::stoi(data.at(cnt++))];
				e->next = mesh->E[std::stoi(data.at(cnt++))];

				if (data.size() == 9) {
					e->sym = mesh->E[std::stoi(data.at(cnt++))];
				}

				e->isDeleted = std::stoi(data.at(cnt++));
				e->isTemp = std::stoi(data.at(cnt++));
			}
			else {
				auto e = mesh->E[std::stoi(data.at(cnt++))];
				e->v = nullptr;
				e->f = nullptr;
				e->prev = nullptr;
				e->next = nullptr;
				e->sym = nullptr;
				e->isDeleted = std::stoi(data.at(cnt++));
				e->isTemp = false;
			}
		}

		for (int i = 0; i < numOfF; ++i) {
			std::getline(ss, to);
			data = splitString(to, "[^0-9.]+");
			int cnt = 1;

			if (data.size() == 6) {
				auto f = mesh->F[std::stoi(data.at(cnt++))];
				f->e = mesh->E[std::stoi(data.at(cnt++))];
				f->isDeleted = std::stoi(data.at(cnt++));
				f->area = std::stod(data.at(cnt++));
				f->perimeter = std::stod(data.at(cnt++));
			}
			else {
				auto f = mesh->F[std::stoi(data.at(cnt++))];
				f->e = nullptr;
				f->isDeleted = std::stoi(data.at(cnt++));
				f->area = 0.0;
				f->perimeter = 0.0;
			}
		}

		std::getline(ss, to);
		int numOfOriginalVertexes = std::stoi(to);
		std::getline(ss, to);
		data = splitString(to, "[^0-9]+");
		for (int i = 0; i < numOfOriginalVertexes; ++i) {
			mesh->originalVertexes.push_back(mesh->V[std::stoi(data.at(i))]);
		}

		return mesh;
	}
	catch (...) {
		return nullptr;
	}
}

#pragma region Mesh-string Convertation

#pragma region Computational Methods

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

double Mesh::getDistance(Vertex* v1, Vertex* v2) {
	return  sqrt((v1->pos->x - v2->pos->x) * (v1->pos->x - v2->pos->x) + (v1->pos->y - v2->pos->y) * (v1->pos->y - v2->pos->y));
}

double Mesh::isPointInEdge(Vertex* v1, Vertex* v2, Point* p) {
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
	return abs(p->y - (k * p->x + b)) < EPS;
}



double Mesh::findYByXAndTwoVertixes(Vertex* v1, Vertex* v2, double xPos) {
	auto k = (v1->pos->y - v2->pos->y) / (v1->pos->x - v2->pos->x);
	auto b = v1->pos->y - k * v1->pos->x;
	return k * xPos + b;
}

double Mesh::findXByYAndTwoVertixes(Vertex* v1, Vertex* v2, double yPos) {
	auto k = (v1->pos->y - v2->pos->y) / (v1->pos->x - v2->pos->x);
	auto b = v1->pos->y - k * v1->pos->x;
	return (yPos - b) / k;
}

#pragma endregion

#pragma region Methods used in the algorithm

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
				splittedEdges.push_back({
					new Point{ prev->prev->v->pos->x,  prev->prev->v->pos->y },
					new Point{ prev->v->pos->x,prev->v->pos->y } });

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

double Mesh::calculateInternalPerimetrOfHorisontalPartition(std::vector<std::pair<Point*, Point*>> splitEdges, Edge* left, int numOfLeft, Edge* right, int numOfRight) {
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
			else if (curLeftY < maxLeft) {
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
		//if vertexes on the same x and x not the rightest point - connect vertexes
		else if(abs(prevEdge->v->pos->x - rightVertex->pos->x) > EPS){
			newEdge = connectVertexes(prevEdge->v, nextEdge->next->v);
			newEdge->isTemp = true;
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

void Mesh::removeEdgeWithTempVertexes(Edge* e) {
	auto vertexes = removeEdge(e);

	if (vertexes[0]->isTemp) {
		removeVertex(vertexes[0]);
	}

	if (vertexes[1]->isTemp) {
		removeVertex(vertexes[1]);
	}
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

double** Mesh::getInternalHorizontalPerimeters() {
	int numOfEdges = verticalMesh.size() - 2;

	double** perimeters = new double* [numOfEdges + 1];

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
			perimeters[i][j] = calculateInternalPerimetrOfHorisontalPartition(splittedEdges, leftEdge, j - 1, rightEdge, j + i);
		}
	}

	return perimeters;
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

	return { optimal, optimalMask };
}

std::vector<std::pair<Point*, Point*> > Mesh::getHorizontalEdges(Edge* leftEdge, Edge* rightEdge, int numOfFacesToSplit) {
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
	testMesh.convertFromPolygonDataOfConvexLeftTraversalPolygon(data);
	testMesh.splitByVerticalGrid();
	auto splittedEdges = testMesh.splitFaces(leftEdge->f->area);

	for (int i = 0; i < splittedEdges.size(); ++i) {
		Rotation::rotatePoint90(splittedEdges[i].first);
		Rotation::rotatePoint90(splittedEdges[i].second);
	}

	return splittedEdges;
}

void Mesh::connectPointsToEdges(std::vector<std::pair<Point*, Point*> > edges, Face* face) {
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
			vertexes.push_back({ f, s });
		}
	}

	for (int i = 0; i < vertexes.size(); ++i) {
		connectVertexes(vertexes[i].first, vertexes[i].second);
	}

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
			edgesToAdd.push_back({ edges, i });
			for (int k = i + 1; k < startZeroPos + 1; ++k) {
				edgesToRemove.push_back(k);
				//	removeEdgeWithTempVertexes(verticalMesh[k]);
			}
			//connectPointsToEdges(edges, verticalMesh[i]->f);
		}
	}

	if (cur == 0) {
		auto edges = getHorizontalEdges(verticalMesh[0], verticalMesh[startZeroPos + 1], startZeroPos + 1);
		edgesToAdd.push_back({ edges, 0 });
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

std::pair<Point*,Point*> Mesh::findPointsV(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double S) {
	//1-leftBottom
	//2-leftTop
	//3-rightTop
	//4-rightBottom
	auto coeff1 = Algorithms::Helper::findLineCoefficients(x4, y4, x1, y1), coeff2 = Algorithms::Helper::findLineCoefficients(x2, y2, x3, y3);
	double a1 = std::get<0>(coeff1), b1 = std::get<1>(coeff1), c1 = std::get<2>(coeff1);
	double a2 = std::get<0>(coeff2), b2 = std::get<1>(coeff2), c2 = std::get<2>(coeff2);
	auto k1 = -b1;
	auto k2 = -b2;
	auto a = b1 * a2 - a1 * b2;
	auto b = b2 * c1 - b1 * c2 + y1 * k2 * b1 + x1 * k2 * a1 - x2 * k1 * a2 - y2 * k1 * b2;
	auto c = k1 * k2 * (x1 * y2 - x2 * y1) + x2 * k1 * c2 - x1 * k2 * c1;
	auto c_pos = c + 2 * S * k1 * k2;
	auto c_neg = c - 2 * S * k1 * k2;

	auto roots1 = Algorithms::Helper::findRootsOfEquation(a, b, c_neg, EPS);
	auto roots2 = Algorithms::Helper::findRootsOfEquation(a, b, c_pos, EPS);

	std::vector<double>* roots = new std::vector<double>();
	Algorithms::Helper::addValidRootsToList(roots1, roots);
	Algorithms::Helper::addValidRootsToList(roots2, roots);

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
			return { new Point{x5, y5}, new Point{x6, y6} };

		}
	}

	return { nullptr, nullptr };
}

#pragma endregion Algorithm: Division of a convex polygon into parts equal in area by the orgonal grid

#pragma region Methods used in the algorithm

bool isEqual(double a, double b) {
	return abs(a - b) < EPS;
}

bool isNotEqual(double a, double b) {
	return abs(a - b) > EPS;
}

bool isGreater(double a, double b) {
	return a - b > EPS;
}

bool isLess(double a, double b) {
	return b - a > EPS;
}

// a Point is defined by its coordinates {int x, y;}
//===================================================================
// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
//    See: Algorithm 1 "Area of Triangles and Polygons"
inline double isLeft(Point* P0, Point* P1, Point* P2)
{
	return ((P1->x - P0->x) * (P2->y - P0->y)
		- (P2->x - P0->x) * (P1->y - P0->y));
}

void rotate(Point* p)
{
	auto x = p->y;
	auto y = p->x;

	p->x = x;
	p->y = y;
}

bool tryRotateVertexes(std::vector<Point*> vertexes) {
	for (int i = 0; i < vertexes.size(); ++i) {
		rotate(vertexes[i]);
	}

	return true;
}

// winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  wn = the winding number (=0 only when P is outside)
int Mesh::isPointInPolygon(Point *P, std::vector<Vertex*> faceVertexes)
{
	int    wn = 0;    // the  winding number counter
	// loop through all edges of the polygon
	for (int i = 0; i < faceVertexes.size(); i++) {   // edge from V[i] to  V[i+1]
		if (isLess(faceVertexes[i]->pos->y, P->y) || isEqual(faceVertexes[i]->pos->y, P->y)) {          // start y <= P.y
			if (isGreater(faceVertexes[(i + 1) % faceVertexes.size()]->pos->y, P->y))      // an upward crossing
			{
				double pos = isLeft(faceVertexes[i]->pos, faceVertexes[(i + 1) % faceVertexes.size()]->pos, P);
				if (isEqual(pos, 0))
					return 0;
				if (isGreater(pos, 0))  // P left of  edge
					++wn;            // have  a valid up intersect
			}
		}
		else {                        // start y > P.y (no test needed)
			if (isLess(faceVertexes[(i + 1) % faceVertexes.size()]->pos->y, P->y) || isEqual(faceVertexes[(i + 1) % faceVertexes.size()]->pos->y, P->y))     // a downward crossing
			{
				double pos = isLeft(faceVertexes[i]->pos, faceVertexes[(i + 1) % faceVertexes.size()]->pos, P);
				if (isEqual(pos, 0))
					return 0;
				if (isLess(pos, 0))  // P right of  edge
					--wn;            // have  a valid down intersect
			}
		}
	}
	return wn;
}

int Mesh::isPointInPolygonTest(Point* P)
{
	std::vector<Point*> rotated;
	for (int i = 0; i < originalVertexes.size(); ++i) {
		rotated.push_back(new Point{ originalVertexes[i]->pos->x, originalVertexes[i]->pos->y });
	}

	tryRotateVertexes(rotated);
	rotate(P);

	int    wn = 0;    // the  winding number counter
	// loop through all edges of the polygon
	for (int i = 0; i < rotated.size(); i++) {   // edge from V[i] to  V[i+1]
		if (isLess(rotated[i]->y, P->y) || isEqual(rotated[i]->y, P->y)) {          // start y <= P.y
			if (isGreater(rotated[(i + 1) % rotated.size()]->y, P->y))      // an upward crossing
			{
				double pos = isLeft(rotated[i], rotated[(i + 1) % rotated.size()], P);
				if (isEqual(pos, 0))
					return 0;
				if (isGreater(pos, 0))  // P left of  edge
					++wn;            // have  a valid up intersect
			}
		}
		else {                        // start y > P.y (no test needed)
			if (isLess(rotated[(i + 1) % rotated.size()]->y, P->y) || isEqual(rotated[(i + 1) % rotated.size()]->y, P->y))     // a downward crossing
			{
				double pos = isLeft(rotated[i], rotated[(i + 1) % rotated.size()], P);
				if (isEqual(pos, 0))
					return 0;
				if (isLess(pos, 0))  // P right of  edge
					--wn;            // have  a valid down intersect
			}
		}
	}
	return wn;
}

std::vector<int> Mesh::findConcavePoints()
{
	std::vector<int> numOfConcaveVertexes;
	int size = this->V.size();

	for (int i = 0; i < size; ++i) {
		auto prev = this->V[(i - 1 + size) % size]->pos;
		auto cur = this->V[i]->pos;
		auto next = this->V[(i + 1) % size]->pos;

		if ((isEqual(prev->x, cur->x) && isEqual(cur->x, next->x))
			|| (isEqual(prev->y, cur->y) && isEqual(cur->y, next->y)))
			continue;

		auto nearPrev = Algorithms::Helper::findPointOnSegmentByDistance(cur, prev, 0.1, EPS);
		auto nearNext = Algorithms::Helper::findPointOnSegmentByDistance(cur, next, 0.1, EPS);
		auto midOfSegment = new Point{ (nearPrev->x + nearNext->x) / 2,(nearPrev->y + nearNext->y) / 2 };

		if (!isPointInPolygon(midOfSegment, originalVertexes)) {
			numOfConcaveVertexes.push_back(this->V[i]->numOfVertex);
		}
	}

	return numOfConcaveVertexes;
}

void splitToConvexPolygonsByPermutation(std::vector<int> permutation, int n, std::set<int> usedVertexes, bool isVertical, Mesh* mesh, std::vector<Mesh*> * allMeshes)
{
	if (usedVertexes.find(permutation[n]) == usedVertexes.end()) {
		auto v = mesh->V[permutation[n]];
		//split only for border vertexes, no need to search another edge
		auto f = v->e->f;

		std::vector<Vertex*> vertexes;
		if (isVertical) {
			vertexes = mesh->splitByVerticalInFace(f, v->pos->x);
		}
		else {
			vertexes = mesh->splitHorizontalInFace(f, v->pos->y);
		}

		if (vertexes.size() != 1) {
			//.insert(a.end(), b.begin(), b.end()); insert all vert in set  IS WORKING FOR NOT EMPT?????
			//std::copy(vertexes.begin(), vertexes.end(), std::back_inserter(usedVertexes), [](Vertex* v) {return v->numOfVertex; });

			std::transform(vertexes.begin(), vertexes.end(), std::inserter(usedVertexes, usedVertexes.end()),
				[](Vertex* v) {
					return v->numOfVertex;
				});
		}
		else {
			usedVertexes.insert(v->numOfVertex);
		}
	}

	if (n + 1 != permutation.size())
	{
		std::set<int> usedVertexesVertical;
		std::set<int> usedVertexesHorizontal;

		std::copy(usedVertexes.begin(), usedVertexes.end(), std::inserter(usedVertexesVertical, usedVertexesVertical.end()));
		std::copy(usedVertexes.begin(), usedVertexes.end(), std::inserter(usedVertexesHorizontal, usedVertexesHorizontal.end()));

		++n;
		splitToConvexPolygonsByPermutation(permutation, n, usedVertexesVertical, true, Mesh::copy(mesh), allMeshes);
		splitToConvexPolygonsByPermutation(permutation, n, usedVertexesHorizontal, false, Mesh::copy(mesh), allMeshes);
	}
	else {
		allMeshes->push_back(mesh);
	}
	//send with new n
}

Mesh* Mesh::createFromFace(Face* face) {
	vectorD* x = new vectorD();
	vectorD* y = new vectorD();
	vectorI* edges = new vectorI();

	auto cur = face->e;
	int cnt = 0;

	do {
		(*x).push_back(cur->v->pos->x);
		(*y).push_back(cur->v->pos->y);
		(*edges).push_back(cnt++);

		cur = cur->next;
	} while (cur != face->e);


	PolygonData data = PolygonData();
	data.vertex_x = *x;
	data.vertex_y = *y;
	data.tryAddFace(edges->size(), *edges);

	Mesh* mesh = new Mesh();
	mesh->convertFromPolygonDataOfConvexLeftTraversalPolygon(data);
	return mesh;
}

//std::vector<Mesh*> Mesh::getOptimalMesh(std::vector<Mesh*>* meshes) {
//	//std::pair<std::vector<std::pair<Mesh*, double>> *, Mesh*> Mesh::getOptimalMesh(std::vector<Mesh*>* meshes) {
//
//
//	double min = DBL_MAX;
//	std::vector<Mesh*> optimal;
//	//std::vector<std::pair<Mesh*, double> * meshesWithFunc = new std::vector<std::pair<Mesh*, double>>();
//
//	for (auto mesh : *meshes) {
//		double func = 0.0;
//		std::vector<Mesh*>* meshByFaces = new std::vector<Mesh*>();
//
//		for (auto face : mesh->F) {
//			auto cur = Mesh::createFromFace(face);
//			cur->splitByVerticalGrid();
//			auto splitedEdges = cur->splitFaces(cur->F[0]->area / 5.0);
//			auto perims = cur->getInternalHorizontalPerimeters();
//			auto optimal = cur->getOptimalCombinationForInternalPerimeter(perims);
//			cur->splitMeshByMask(optimal.second);
//
//			meshByFaces->push_back(cur);
//
//			for (auto newFace : cur->F) {
//				func += newFace->perimeter * newFace->perimeter / newFace->area;
//			}
//		}
//
//		//
//		///is it nessesary
//		/*for (auto face: mesh->F) {
//			func += face->perimeter * face->perimeter / face->area;
//		}*/
//
//		if (min > func){
//			min = func;
//			optimal = *meshByFaces;
//		}
//	}
//
//	return optimal;
//}

Mesh* Mesh::getOptimalMesh(std::vector<Mesh*>* meshes) {
	double min = DBL_MAX;
	Mesh* optimal = nullptr;

	for (auto mesh : *meshes) {
		double func = 0.0;
		for (auto face : mesh->F) {
			func += face->perimeter * face->perimeter / face->area;
		}

		if (min > func) {
			min = func;
			optimal = mesh;
		}
	}

	return optimal;
}

std::vector<Mesh*>* Mesh::splitToConvexPolygons()
{
	auto v = findConcavePoints();

	if (v.size() == 0)
		return new std::vector<Mesh*>(1, this);

	Algorithms::Helper::startPermutation(&v);

	std::set<int> usedVertexes;
	std::vector<Mesh*>* allMeshes = new std::vector<Mesh*>();

	do {
//#pragma omp parallel
//		{
//#pragma omp task
			splitToConvexPolygonsByPermutation(v, 0, usedVertexes, true, Mesh::copy(this), allMeshes);

//#pragma omp task
			//splitToConvexPolygonsByPermutation(v, 0, usedVertexes, false, Mesh::copy(this), allMeshes);
		//} 
		////blocks at end of parallel block to wait for tasks to finish
	} while (Algorithms::Helper::tryNextPermutation(&v));

	return allMeshes;
}

void Mesh::splitByVertical(double coord) {
	std::map<Vertex*, int> vertexesToJoinMap;
	std::vector<std::pair<Vertex*, int> > vertexesToJoin;
	std::vector<std::pair<Edge*, Point*>> vertexesToAdd;
	std::vector<std::pair<Vertex*, Vertex*>> edgesToAdd;

	std::set<Edge*> visitedEdges;

	for (int i = 0; i < this->E.size(); ++i) {
		if (visitedEdges.find(this->E[i]) != visitedEdges.end())
			continue;

		auto startV = this->E[i]->v;
		auto endV = this->E[i]->next->v;

		if ((isLess(startV->pos->x, coord) && isGreater(endV->pos->x, coord)) || (isGreater(startV->pos->x, coord) && isLess(endV->pos->x, coord))) {

			if (isEqual(startV->pos->y, endV->pos->y)) {

				vertexesToAdd.push_back({ this->E[i], new Point{ coord, startV->pos->y } });
			}
			else {

				vertexesToAdd.push_back({ this->E[i], new Point{ coord, this->findYByXAndTwoVertixes(startV, endV, coord) } });
			}
		}

		if (isEqual(startV->pos->x, coord) && vertexesToJoinMap.find(startV) == vertexesToJoinMap.end())
			vertexesToJoinMap.insert({ startV, 0 });
		if (isEqual(endV->pos->x, coord) && vertexesToJoinMap.find(endV) == vertexesToJoinMap.end())
			vertexesToJoinMap.insert({ endV, 0 });

		if (isEqual(startV->pos->x, coord) && isEqual(endV->pos->x, coord)) {
			vertexesToJoinMap[startV] = ++vertexesToJoinMap[startV];
			vertexesToJoinMap[endV] = ++vertexesToJoinMap[endV];
		}

		visitedEdges.insert(this->E[i]);

		if (this->E[i]->sym != nullptr)
			visitedEdges.insert(this->E[i]->sym);
	}

	for (int i = 0; i < vertexesToAdd.size(); ++i) {
		vertexesToJoinMap.insert({ this->splitEdge(vertexesToAdd[i].first, vertexesToAdd[i].second), 0 });
	}

	std::copy(vertexesToJoinMap.begin(), vertexesToJoinMap.end(), back_inserter(vertexesToJoin));
	sort(vertexesToJoin.begin(), vertexesToJoin.end(), [](const std::pair<Vertex*, int>& p1, const std::pair<Vertex*, int>& p2)
		{ return p1.first->pos->y > p2.first->pos->y; });

	if (vertexesToJoin.size() > 0) {

		vertexesToJoin[0].second = ++vertexesToJoin[0].second;
		vertexesToJoin[vertexesToJoin.size() - 1].second = ++vertexesToJoin[vertexesToJoin.size() - 1].second;
	}

	for (int i = 1; i < vertexesToJoin.size(); ++i) {
		auto cur = vertexesToJoin[i].first;
		auto prev = vertexesToJoin[i - 1].first;

		if (isPointInPolygon(new Point{ cur->pos->x,  cur->pos->y + ((prev->pos->y - cur->pos->y) / 2 )}, originalVertexes) != 0) {
			edgesToAdd.push_back({ prev, cur });
		}
	}

	for (int i = 0; i < edgesToAdd.size(); ++i) {
		this->connectVertexes(edgesToAdd[i].first, edgesToAdd[i].second);
	}
}

std::vector<Vertex*>  Mesh::splitByVerticalInFace(Face* f, double coord) {
	//if i split and get other vertex in list?

	std::set<Vertex*> vertexesToJoinMap;
	std::vector<Vertex*> vertexesToJoin;
	std::map<Edge*, Point*> vertexesToAdd;
	std::vector<Vertex*> faceVertexes;

	auto cur = f->e;
	do {
		auto startV = cur->v;
		auto endV = cur->next->v;
		faceVertexes.push_back(cur->v);

		if ((isLess(startV->pos->x, coord) && isGreater(endV->pos->x, coord)) || (isGreater(startV->pos->x, coord) && isLess(endV->pos->x, coord))) {

			if (isEqual(startV->pos->y, endV->pos->y)) {

				vertexesToAdd.insert({ cur, new Point{ coord, startV->pos->y } });
			}
			else {

				vertexesToAdd.insert({ cur, new Point{ coord, this->findYByXAndTwoVertixes(startV, endV, coord) } });
			}
		}

		if (isEqual(startV->pos->x, coord) && vertexesToJoinMap.find(startV) == vertexesToJoinMap.end())
			vertexesToJoinMap.insert(startV);
		if (isEqual(endV->pos->x, coord) && vertexesToJoinMap.find(endV) == vertexesToJoinMap.end())
			vertexesToJoinMap.insert(endV);

		cur = cur->next;

	} while (f->e != cur);

	std::copy(vertexesToJoinMap.begin(), vertexesToJoinMap.end(), back_inserter(vertexesToJoin));

	return splitByListOfVertexes(vertexesToAdd, vertexesToJoin, faceVertexes);
}

std::vector<Vertex*>  Mesh::splitHorizontalInFace(Face* f, double coord) {
	////if i split and get other vertex in list?

	std::set<Vertex*> vertexesToJoinMap;
	std::vector<Vertex*> vertexesToJoin;
	std::map<Edge*, Point*> vertexesToAdd;
	std::vector<Vertex*> faceVertexes;

	auto cur = f->e;
	do {
		auto startV = cur->v;
		auto endV = cur->next->v;
		faceVertexes.push_back(cur->v);

		if ((isLess(startV->pos->y, coord) && isGreater(endV->pos->y, coord)) || (isGreater(startV->pos->y, coord) && isLess(endV->pos->y, coord))) {
			if (isEqual(startV->pos->x, endV->pos->x)) {
				vertexesToAdd.insert({cur, new Point{ startV->pos->x, coord } });
			}
			else {
				vertexesToAdd.insert({ cur, new Point{ this->findXByYAndTwoVertixes(startV, endV, coord), coord } });
			}
		}

		if (isEqual(startV->pos->y, coord) && vertexesToJoinMap.find(startV) == vertexesToJoinMap.end()) 
			vertexesToJoinMap.insert(startV);
		if (isEqual(endV->pos->y, coord) && vertexesToJoinMap.find(endV) == vertexesToJoinMap.end()) 
			vertexesToJoinMap.insert(endV);


		cur = cur->next;

	} while (f->e != cur);

	std::copy(vertexesToJoinMap.begin(), vertexesToJoinMap.end(), back_inserter(vertexesToJoin));

	return splitByListOfVertexes(vertexesToAdd, vertexesToJoin, faceVertexes);
}

std::vector<Vertex*> Mesh::splitByListOfVertexes(std::map<Edge*, Point*> toAddAndJoin, std::vector<Vertex*> toJoin, std::vector<Vertex*> faceVertexes)
{
	std::vector<std::pair<Vertex*, Vertex*>> edgesToAdd;

	for (auto toAdd: toAddAndJoin) {
		toJoin.push_back(this->splitEdge(toAdd.first, toAdd.second));
	}

	sort(toJoin.begin(), toJoin.end(), [](const Vertex* p1, const Vertex* p2)
		{ return isGreater(p1->pos->y, p2->pos->y) || isLess(p1->pos->x, p2->pos->x); });


	for (int i = 1; i < toJoin.size(); ++i) {
		auto cur = toJoin[i];
		auto prev = toJoin[i - 1];

		if (isPointInPolygon(new Point{ (prev->pos->x + cur->pos->x) / 2,  (prev->pos->y + cur->pos->y) / 2 }, faceVertexes) != 0) {
			edgesToAdd.push_back({ prev, cur });
		}
	}

	for (int i = 0; i < edgesToAdd.size(); ++i) {
		this->connectVertexes(edgesToAdd[i].first, edgesToAdd[i].second);
	}

	return toJoin;
}

void Mesh::splitHorizontal(double coord) {
	std::map<Vertex*, int> vertexesToJoinMap;
	std::vector<std::pair<Vertex*, int> > vertexesToJoin;
	std::vector<std::pair<Edge*, Point*>> vertexesToAdd;
	std::vector<std::pair<Vertex*, Vertex*>> edgesToAdd;

	std::set<Edge*> visitedEdges;

	for (int i = 0; i < this->E.size(); ++i) {
		if (visitedEdges.find(this->E[i]) != visitedEdges.end())
			continue;

		auto startV = this->E[i]->v;
		auto endV = this->E[i]->next->v;

		if ((isLess(startV->pos->y, coord) && isGreater(endV->pos->y, coord)) || (isGreater(startV->pos->y, coord) && isLess(endV->pos->y, coord))) {
			if (isEqual(startV->pos->x, endV->pos->x)) {
				vertexesToAdd.push_back({ this->E[i], new Point{ startV->pos->x, coord } });
			}
			else {
				vertexesToAdd.push_back({ this->E[i], new Point{ this->findXByYAndTwoVertixes(startV, endV, coord), coord } });
			}
		}

		if (isEqual(startV->pos->y, coord) && vertexesToJoinMap.find(startV) == vertexesToJoinMap.end()) vertexesToJoinMap.insert({ startV, 0 });
		if (isEqual(endV->pos->y, coord) && vertexesToJoinMap.find(endV) == vertexesToJoinMap.end()) vertexesToJoinMap.insert({ endV, 0 });

		if (isEqual(startV->pos->y, coord) && isEqual(endV->pos->y, coord)) {
			vertexesToJoinMap[startV] = ++vertexesToJoinMap[startV];
			vertexesToJoinMap[endV] = ++vertexesToJoinMap[endV];
		}

		visitedEdges.insert(this->E[i]);

		if (this->E[i]->sym != nullptr)
			visitedEdges.insert(this->E[i]->sym);
	}

	for (int i = 0; i < vertexesToAdd.size(); ++i) {
		vertexesToJoinMap.insert({ this->splitEdge(vertexesToAdd[i].first, vertexesToAdd[i].second), 0 });
	}

	std::copy(vertexesToJoinMap.begin(), vertexesToJoinMap.end(), back_inserter(vertexesToJoin));
	sort(vertexesToJoin.begin(), vertexesToJoin.end(), [](const std::pair<Vertex*, int>& p1, const std::pair<Vertex*, int>& p2)
		{ return p1.first->pos->x < p2.first->pos->x; });

	if (vertexesToJoin.size() > 0) {

		vertexesToJoin[0].second = ++vertexesToJoin[0].second;
		vertexesToJoin[vertexesToJoin.size() - 1].second = ++vertexesToJoin[vertexesToJoin.size() - 1].second;
	}

	for (int i = 1; i < vertexesToJoin.size(); ++i) {
		auto cur = vertexesToJoin[i].first;
		auto prev = vertexesToJoin[i - 1].first;

		if (isPointInPolygonTest(new Point{ prev->pos->x + ((cur->pos->x - prev->pos->x) / 2), cur->pos->y }) != 0) {
			edgesToAdd.push_back({ prev, cur });
		}
	}

	for (int i = 0; i < edgesToAdd.size(); ++i) {
		this->connectVertexes(edgesToAdd[i].first, edgesToAdd[i].second);
	}
}

#pragma endregion Algorithm: Division of a polygon of a part along an orthogonal grid with area restrictions