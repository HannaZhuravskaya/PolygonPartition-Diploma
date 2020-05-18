#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AlgorithmTests
{
	TEST_CLASS(PolygonMeshTests)
	{
	public:
#pragma region Constructors

#pragma endregion

#pragma region Public methods
		TEST_METHOD(convertFromPolygonDataOfConvexLeftTraversalPolygon_RightPolygonData_RightPolygonalMesh) {
#pragma region Test Picture
			//  1
			//	|\
			//	| \
			//	|__\
			//  2   0
#pragma endregion

			auto m = getTriangleMesh();
#pragma region Check Sizes
			Assert::AreEqual((int)m->F.size(), 1);
			Assert::AreEqual((int)m->E.size(), 3);
			Assert::AreEqual((int)m->V.size(), 3);
#pragma endregion

#pragma region Check Face Connections
			Assert::AreEqual(m->F[0]->area, 12.5, EPS);
			Assert::IsNotNull(m->F[0]->e);
			Assert::AreEqual(m->F[0]->numOfFace, 0);
			Assert::AreEqual(m->F[0]->isDeleted, false);
#pragma endregion

#pragma region Check Vertex Connections
			Assert::IsTrue(m->V[0]->e == m->E[0]);
			Assert::AreEqual(m->V[0]->numOfVertex, 0);
			Assert::AreEqual(m->V[0]->isDeleted, false);

			Assert::IsTrue(m->V[1]->e == m->E[1]);
			Assert::AreEqual(m->V[1]->numOfVertex, 1);
			Assert::AreEqual(m->V[1]->isDeleted, false);

			Assert::IsTrue(m->V[2]->e == m->E[2]);
			Assert::AreEqual(m->V[2]->numOfVertex, 2);
			Assert::AreEqual(m->V[2]->isDeleted, false);
#pragma endregion

#pragma region Check Edge Connections
			Assert::AreEqual(m->E[0]->numOfEdge, 0);
			Assert::IsTrue(m->E[0]->v == m->V[0]);
			Assert::IsTrue(m->E[0]->prev == m->E[2]);
			Assert::IsTrue(m->E[0]->next == m->E[1]);
			Assert::IsTrue(m->E[0]->sym == nullptr);
			Assert::IsTrue(m->E[0]->f == m->F[0]);
			Assert::IsTrue(m->E[0]->isDeleted == false);

			Assert::AreEqual(m->E[1]->numOfEdge, 1);
			Assert::IsTrue(m->E[1]->v == m->V[1]);
			Assert::IsTrue(m->E[1]->prev == m->E[0]);
			Assert::IsTrue(m->E[1]->next == m->E[2]);
			Assert::IsTrue(m->E[1]->sym == nullptr);
			Assert::IsTrue(m->E[1]->f == m->F[0]);
			Assert::IsTrue(m->E[1]->isDeleted == false);

			Assert::AreEqual(m->E[2]->numOfEdge, 2);
			Assert::IsTrue(m->E[2]->v == m->V[2]);
			Assert::IsTrue(m->E[2]->prev == m->E[1]);
			Assert::IsTrue(m->E[2]->next == m->E[0]);
			Assert::IsTrue(m->E[2]->sym == nullptr);
			Assert::IsTrue(m->E[2]->f == m->F[0]);
			Assert::IsTrue(m->E[2]->isDeleted == false);
#pragma endregion
		}

		TEST_METHOD(spliteEdge_SymEdgeIsNUllTriangleMesh_NewVertexAndOneEdgeAdded) {
			//  0
			//	|\
			//	0 \
			//	|__\
			//  0   0

			auto m = getTriangleMesh();
			auto v1 = m->splitEdge(m->E[1], new Point{ 0, 2 });
#pragma region Check Sizes
			Assert::AreEqual((int)m->F.size(), 1);
			Assert::AreEqual((int)m->E.size(), 4);
			Assert::AreEqual((int)m->V.size(), 4);
#pragma endregion

#pragma region Check Face Connections
			Assert::AreEqual(m->F[0]->area, 12.5, EPS);
			Assert::IsNotNull(m->F[0]->e);
			Assert::AreEqual(m->F[0]->numOfFace, 0);
			Assert::AreEqual(m->F[0]->isDeleted, false);
#pragma endregion

#pragma region Check Vertex Connections
			Assert::AreEqual(v1->pos->x, 0.0, EPS);
			Assert::AreEqual(v1->pos->y, 2.0, EPS);
			Assert::IsTrue(v1 == m->V[3]);

			Assert::IsTrue(m->V[0]->e == m->E[0]);
			Assert::AreEqual(m->V[0]->numOfVertex, 0);
			Assert::AreEqual(m->V[0]->isDeleted, false);

			Assert::IsTrue(m->V[1]->e == m->E[1]);
			Assert::AreEqual(m->V[1]->numOfVertex, 1);
			Assert::AreEqual(m->V[1]->isDeleted, false);

			Assert::IsTrue(m->V[2]->e == m->E[2]);
			Assert::AreEqual(m->V[2]->numOfVertex, 2);
			Assert::AreEqual(m->V[2]->isDeleted, false);

			Assert::IsTrue(m->V[3]->e == m->E[3]);
			Assert::AreEqual(m->V[3]->numOfVertex, 3);
			Assert::AreEqual(m->V[3]->isDeleted, false);
#pragma endregion

#pragma region Check Edge Connections
			Assert::AreEqual(m->E[0]->numOfEdge, 0);
			Assert::IsTrue(m->E[0]->v == m->V[0]);
			Assert::IsTrue(m->E[0]->prev == m->E[2]);
			Assert::IsTrue(m->E[0]->next == m->E[1]);
			Assert::IsTrue(m->E[0]->sym == nullptr);
			Assert::IsTrue(m->E[0]->f == m->F[0]);
			Assert::IsTrue(m->E[0]->isDeleted == false);

			Assert::AreEqual(m->E[1]->numOfEdge, 1);
			Assert::IsTrue(m->E[1]->v == m->V[1]);
			Assert::IsTrue(m->E[1]->prev == m->E[0]);
			Assert::IsTrue(m->E[1]->next == m->E[3]);
			Assert::IsTrue(m->E[1]->sym == nullptr);
			Assert::IsTrue(m->E[1]->f == m->F[0]);
			Assert::IsTrue(m->E[1]->isDeleted == false);

			Assert::AreEqual(m->E[2]->numOfEdge, 2);
			Assert::IsTrue(m->E[2]->v == m->V[2]);
			Assert::IsTrue(m->E[2]->prev == m->E[3]);
			Assert::IsTrue(m->E[2]->next == m->E[0]);
			Assert::IsTrue(m->E[2]->sym == nullptr);
			Assert::IsTrue(m->E[2]->f == m->F[0]);
			Assert::IsTrue(m->E[2]->isDeleted == false);

			Assert::AreEqual(m->E[3]->numOfEdge, 3);
			Assert::IsTrue(m->E[3]->v == m->V[3]);
			Assert::IsTrue(m->E[3]->prev == m->E[1]);
			Assert::IsTrue(m->E[3]->next == m->E[2]);
			Assert::IsTrue(m->E[3]->sym == nullptr);
			Assert::IsTrue(m->E[3]->f == m->F[0]);
			Assert::IsTrue(m->E[3]->isDeleted == false);
#pragma endregion
		}

		TEST_METHOD(spliteEdge_SymEdgeIsNUllTriangleMeshAddTwoVertexes_NewTwoVertexAndTwoEdgesAdded) {
			//  0
			//  |\
			//  | \
			//	0  \
			//	|   \
			//	|_  _\
			//  0  0  0

			auto m = getTriangleMesh();
			auto v1 = m->splitEdge(m->E[1], new Point{ 0, 2 });
			auto v2 = m->splitEdge(m->E[2], new Point{ 2, 0 });

#pragma region Check Sizes
			Assert::AreEqual((int)m->F.size(), 1);
			Assert::AreEqual((int)m->E.size(), 5);
			Assert::AreEqual((int)m->V.size(), 5);
#pragma endregion

#pragma region Check Face Connections
			Assert::AreEqual(m->F[0]->area, 12.5, EPS);
			Assert::IsNotNull(m->F[0]->e);
			Assert::AreEqual(m->F[0]->numOfFace, 0);
			Assert::AreEqual(m->F[0]->isDeleted, false);
#pragma endregion

#pragma region Check Vertex Connections
			Assert::AreEqual(v1->pos->x, 0.0, EPS);
			Assert::AreEqual(v1->pos->y, 2.0, EPS);
			Assert::IsTrue(v1 == m->V[3]);

			Assert::AreEqual(v2->pos->x, 2.0, EPS);
			Assert::AreEqual(v2->pos->y, 0.0, EPS);
			Assert::IsTrue(v2 == m->V[4]);

			Assert::IsTrue(m->V[0]->e == m->E[0]);
			Assert::AreEqual(m->V[0]->numOfVertex, 0);
			Assert::AreEqual(m->V[0]->isDeleted, false);

			Assert::IsTrue(m->V[1]->e == m->E[1]);
			Assert::AreEqual(m->V[1]->numOfVertex, 1);
			Assert::AreEqual(m->V[1]->isDeleted, false);

			Assert::IsTrue(m->V[2]->e == m->E[2]);
			Assert::AreEqual(m->V[2]->numOfVertex, 2);
			Assert::AreEqual(m->V[2]->isDeleted, false);

			Assert::IsTrue(m->V[3]->e == m->E[3]);
			Assert::AreEqual(m->V[3]->numOfVertex, 3);
			Assert::AreEqual(m->V[3]->isDeleted, false);

			Assert::IsTrue(m->V[4]->e == m->E[4]);
			Assert::AreEqual(m->V[4]->numOfVertex, 4);
			Assert::AreEqual(m->V[4]->isDeleted, false);
#pragma endregion

#pragma region Check Edge Connections
			Assert::AreEqual(m->E[0]->numOfEdge, 0);
			Assert::IsTrue(m->E[0]->v == m->V[0]);
			Assert::IsTrue(m->E[0]->prev == m->E[4]);
			Assert::IsTrue(m->E[0]->next == m->E[1]);
			Assert::IsTrue(m->E[0]->sym == nullptr);
			Assert::IsTrue(m->E[0]->f == m->F[0]);
			Assert::IsTrue(m->E[0]->isDeleted == false);

			Assert::AreEqual(m->E[1]->numOfEdge, 1);
			Assert::IsTrue(m->E[1]->v == m->V[1]);
			Assert::IsTrue(m->E[1]->prev == m->E[0]);
			Assert::IsTrue(m->E[1]->next == m->E[3]);
			Assert::IsTrue(m->E[1]->sym == nullptr);
			Assert::IsTrue(m->E[1]->f == m->F[0]);
			Assert::IsTrue(m->E[1]->isDeleted == false);

			Assert::AreEqual(m->E[2]->numOfEdge, 2);
			Assert::IsTrue(m->E[2]->v == m->V[2]);
			Assert::IsTrue(m->E[2]->prev == m->E[3]);
			Assert::IsTrue(m->E[2]->next == m->E[4]);
			Assert::IsTrue(m->E[2]->sym == nullptr);
			Assert::IsTrue(m->E[2]->f == m->F[0]);
			Assert::IsTrue(m->E[2]->isDeleted == false);

			Assert::AreEqual(m->E[3]->numOfEdge, 3);
			Assert::IsTrue(m->E[3]->v == m->V[3]);
			Assert::IsTrue(m->E[3]->prev == m->E[1]);
			Assert::IsTrue(m->E[3]->next == m->E[2]);
			Assert::IsTrue(m->E[3]->sym == nullptr);
			Assert::IsTrue(m->E[3]->f == m->F[0]);
			Assert::IsTrue(m->E[3]->isDeleted == false);

			Assert::AreEqual(m->E[4]->numOfEdge, 4);
			Assert::IsTrue(m->E[4]->v == m->V[4]);
			Assert::IsTrue(m->E[4]->prev == m->E[2]);
			Assert::IsTrue(m->E[4]->next == m->E[0]);
			Assert::IsTrue(m->E[4]->sym == nullptr);
			Assert::IsTrue(m->E[4]->f == m->F[0]);
			Assert::IsTrue(m->E[4]->isDeleted == false);
#pragma endregion
		}

		TEST_METHOD(connectVertexes_SymIsNullTriangleMesh3With4_NewFaceAndTwoEdges) {
			auto m = getTriangleMesh();
			auto v1 = m->splitEdge(m->E[1], new Point{ 0, 2 });
			auto v2 = m->splitEdge(m->E[2], new Point{ 2, 0 });
			auto e = m->connectVertexes(v1, v2);

#pragma region Check Sizes
			Assert::AreEqual((int)m->F.size(), 2);
			Assert::AreEqual((int)m->E.size(), 7);
			Assert::AreEqual((int)m->V.size(), 5);
#pragma endregion

#pragma region Check Vertex Connections
			Assert::AreEqual(v1->pos->x, 0.0, EPS);
			Assert::AreEqual(v1->pos->y, 2.0, EPS);
			Assert::IsTrue(v1 == m->V[3]);

			Assert::AreEqual(v2->pos->x, 2.0, EPS);
			Assert::AreEqual(v2->pos->y, 0.0, EPS);
			Assert::IsTrue(v2 == m->V[4]);

			checkVertex(m->V[0], m->E[0], 0, false);

			Assert::IsTrue(m->V[1]->e == m->E[1]);
			Assert::AreEqual(m->V[1]->numOfVertex, 1);
			Assert::AreEqual(m->V[1]->isDeleted, false);

			Assert::IsTrue(m->V[2]->e == m->E[2]);
			Assert::AreEqual(m->V[2]->numOfVertex, 2);
			Assert::AreEqual(m->V[2]->isDeleted, false);

			Assert::IsTrue(m->V[3]->e == m->E[3] || m->V[3]->e == m->E[5]);
			Assert::AreEqual(m->V[3]->numOfVertex, 3);
			Assert::AreEqual(m->V[3]->isDeleted, false);

			Assert::IsTrue(m->V[4]->e == m->E[4] || m->V[4]->e == m->E[6]);
			Assert::AreEqual(m->V[4]->numOfVertex, 4);
			Assert::AreEqual(m->V[4]->isDeleted, false);
#pragma endregion

#pragma region Check Edge Connections
			Assert::IsNotNull(e->sym);
			Assert::IsTrue((e == m->E[5] && e->sym == m->E[6]) || (e == m->E[6] && e->sym == m->E[5]));

			checkEdge(m->E[0], 0, m->V[0], m->E[4], m->E[1], nullptr, m->F[1], false);
			checkEdge(m->E[1], 1, m->V[1], m->E[0], m->E[6], nullptr, m->F[1], false);
			checkEdge(m->E[2], 2, m->V[2], m->E[3], m->E[5], nullptr, m->F[0], false);
			checkEdge(m->E[3], 3, m->V[3], m->E[5], m->E[2], nullptr, m->F[0], false);
			checkEdge(m->E[4], 4, m->V[4], m->E[6], m->E[0], nullptr, m->F[1], false);
			checkEdge(m->E[5], 5, m->V[4], m->E[2], m->E[3], m->E[6], m->F[0], false);
			checkEdge(m->E[6], 6, m->V[3], m->E[1], m->E[4], m->E[5], m->F[1], false);
#pragma endregion	

#pragma region Check Face Connections
			Assert::AreEqual(m->F[0]->area + m->F[1]->area, 12.5, EPS);

			Assert::AreEqual(m->F[0]->area, 2.0, EPS);
			Assert::IsNotNull(m->F[0]->e);
			Assert::AreEqual(m->F[0]->numOfFace, 0);
			Assert::AreEqual(m->F[0]->isDeleted, false);

			Assert::AreEqual(m->F[1]->area, 10.5, EPS);
			Assert::IsNotNull(m->F[1]->e);
			Assert::AreEqual(m->F[1]->numOfFace, 1);
			Assert::AreEqual(m->F[1]->isDeleted, false);

			Assert::IsTrue(m->F[0]->e != m->F[1]->e);
#pragma endregion
		}

		TEST_METHOD(connectVertexes_SymIsNullTriangleMesh4With3_NewFaceAndTwoEdges) {
			auto m = getTriangleMesh();
			auto v1 = m->splitEdge(m->E[1], new Point{ 0, 2 });
			auto v2 = m->splitEdge(m->E[2], new Point{ 2, 0 });
			auto e = m->connectVertexes(v2, v1);

#pragma region Check Sizes
			Assert::AreEqual((int)m->F.size(), 2);
			Assert::AreEqual((int)m->E.size(), 7);
			Assert::AreEqual((int)m->V.size(), 5);
#pragma endregion

#pragma region Check Vertex Connections
			Assert::AreEqual(v1->pos->x, 0.0, EPS);
			Assert::AreEqual(v1->pos->y, 2.0, EPS);
			Assert::IsTrue(v1 == m->V[3]);

			Assert::AreEqual(v2->pos->x, 2.0, EPS);
			Assert::AreEqual(v2->pos->y, 0.0, EPS);
			Assert::IsTrue(v2 == m->V[4]);

			checkVertex(m->V[0], m->E[0], 0, false);
			checkVertex(m->V[1], m->E[1], 1, false);
			checkVertex(m->V[2], m->E[2], 2, false);
			checkVertex(m->V[3], m->E[3], 3, false);
			checkVertex(m->V[4], m->E[4], 4, false);
#pragma endregion

#pragma region Check Edge Connections
			Assert::IsNotNull(e->sym);
			Assert::IsTrue((e == m->E[5] && e->sym == m->E[6]) || (e == m->E[6] && e->sym == m->E[5]));

			checkEdge(m->E[0], 0, m->V[0], m->E[4], m->E[1], nullptr, m->F[0], false);
			checkEdge(m->E[1], 1, m->V[1], m->E[0], m->E[5], nullptr, m->F[0], false);
			checkEdge(m->E[2], 2, m->V[2], m->E[3], m->E[6], nullptr, m->F[1], false);
			checkEdge(m->E[3], 3, m->V[3], m->E[6], m->E[2], nullptr, m->F[1], false);
			checkEdge(m->E[4], 4, m->V[4], m->E[5], m->E[0], nullptr, m->F[0], false);
			checkEdge(m->E[5], 5, m->V[3], m->E[1], m->E[4], m->E[6], m->F[0], false);
			checkEdge(m->E[6], 6, m->V[4], m->E[2], m->E[3], m->E[5], m->F[1], false);
#pragma endregion	

#pragma region Check Face Connections
			Assert::AreEqual(m->F[0]->area + m->F[1]->area, 12.5, EPS);

			Assert::AreEqual(m->F[0]->area, 10.5, EPS);
			Assert::IsNotNull(m->F[0]->e);
			Assert::AreEqual(m->F[0]->numOfFace, 0);
			Assert::AreEqual(m->F[0]->isDeleted, false);

			Assert::AreEqual(m->F[1]->area, 2.0, EPS);
			Assert::IsNotNull(m->F[1]->e);
			Assert::AreEqual(m->F[1]->numOfFace, 1);
			Assert::AreEqual(m->F[1]->isDeleted, false);

			Assert::IsTrue(m->F[0]->e != m->F[1]->e);
#pragma endregion
		}

		TEST_METHOD(removeEdge_TriangleMesh_Edges5And6Deleted) {
			auto m = getTriangleMesh();
			auto v1 = m->splitEdge(m->E[1], new Point{ 0, 2 });
			auto v2 = m->splitEdge(m->E[2], new Point{ 2, 0 });
			auto e = m->connectVertexes(v1, v2);
			auto vertexes = m->removeEdge(e);

#pragma region Check Sizes
			Assert::AreEqual((int)m->F.size(), 2);
			Assert::AreEqual((int)m->E.size(), 7);
			Assert::AreEqual((int)m->V.size(), 5);
#pragma endregion

#pragma region Check Vertex Connections
			checkVertex(m->V[0], m->E[0], 0, false);
			checkVertex(m->V[1], m->E[1], 1, false);
			checkVertex(m->V[2], m->E[2], 2, false);
			checkVertex(m->V[3], m->E[3], 3, false);
			checkVertex(m->V[4], m->E[4], 4, false);
#pragma endregion

#pragma region Check Edge Connections
			checkEdge(m->E[0], 0, m->V[0], m->E[4], m->E[1], nullptr, m->F[0], false);
			checkEdge(m->E[1], 1, m->V[1], m->E[0], m->E[3], nullptr, m->F[0], false);
			checkEdge(m->E[2], 2, m->V[2], m->E[3], m->E[4], nullptr, m->F[0], false);
			checkEdge(m->E[3], 3, m->V[3], m->E[1], m->E[2], nullptr, m->F[0], false);
			checkEdge(m->E[4], 4, m->V[4], m->E[2], m->E[0], nullptr, m->F[0], false);
			checkEdge(m->E[5], 5, nullptr, nullptr, nullptr, nullptr, nullptr, true);
			checkEdge(m->E[6], 6, nullptr, nullptr, nullptr, nullptr, nullptr, true);
#pragma endregion	

#pragma region Check Face Connections
			Assert::AreEqual(m->F[0]->area, 12.5, EPS);
			Assert::IsNotNull(m->F[0]->e);
			Assert::AreEqual(m->F[0]->numOfFace, 0);
			Assert::AreEqual(m->F[0]->isDeleted, false);

			Assert::AreEqual(m->F[1]->area, 0.0, EPS);
			Assert::IsNull(m->F[1]->e);
			Assert::AreEqual(m->F[1]->numOfFace, 1);
			Assert::AreEqual(m->F[1]->isDeleted, true);
#pragma endregion
		}

		TEST_METHOD(removeVertex_TriangleMesh_Vertex4AndEdge4Deleted) {
			auto m = getTriangleMesh();
			auto v1 = m->splitEdge(m->E[1], new Point{ 0, 2 });
			auto e = m->removeVertex(v1);

#pragma region Check Sizes
			Assert::AreEqual((int)m->F.size(), 1);
			Assert::AreEqual((int)m->E.size(), 4);
			Assert::AreEqual((int)m->V.size(), 4);
#pragma endregion

#pragma region Check Vertex Connections
			checkVertex(m->V[0], m->E[0], 0, false);
			checkVertex(m->V[1], m->E[1], 1, false);
			checkVertex(m->V[2], m->E[2], 2, false);
			checkVertex(m->V[3], nullptr, 3, true);
#pragma endregion

#pragma region Check Edge Connections
			checkEdge(m->E[0], 0, m->V[0], m->E[2], m->E[1], nullptr, m->F[0], false);
			checkEdge(m->E[1], 1, m->V[1], m->E[0], m->E[2], nullptr, m->F[0], false);
			checkEdge(m->E[2], 2, m->V[2], m->E[1], m->E[0], nullptr, m->F[0], false);
			checkEdge(m->E[3], 3, nullptr, nullptr, nullptr, nullptr, nullptr, true);
#pragma endregion	

#pragma region Check Face Connections
			Assert::AreEqual(m->F[0]->area, 12.5, EPS);
			Assert::IsNotNull(m->F[0]->e);
			Assert::AreEqual(m->F[0]->numOfFace, 0);
			Assert::AreEqual(m->F[0]->isDeleted, false);
#pragma endregion

		}

		TEST_METHOD(convertToPolygonData_TriangleMeshNoDeletedStructures_ReturnValidData) {
			auto m = getTriangleMesh();
			auto data = m->convertToPolygonData();

			Assert::AreEqual(data.vertex_x[0], 5.0, EPS);
			Assert::AreEqual(data.vertex_y[0], 0.0, EPS);
			Assert::AreEqual(data.vertex_x[1], 0.0, EPS);
			Assert::AreEqual(data.vertex_y[1], 5.0, EPS);
			Assert::AreEqual(data.vertex_x[2], 0.0, EPS);
			Assert::AreEqual(data.vertex_y[2], 0.0, EPS);

			Assert::AreEqual(data.edges[0], 0);
			Assert::AreEqual(data.edges[1], 1);
			Assert::AreEqual(data.edges[2], 2);

			Assert::AreEqual(data.faces[0], 0);
			Assert::AreEqual(data.faces[1], 3);
		}

		TEST_METHOD(convertToPolygonData_TriangleMeshWithDeleted3PointAnd4EdgeStructures_ReturnValidData) {
			auto m = getTriangleMesh();
			auto v = m->splitEdge(m->E[1], new Point{ 0.0, 2.0 });
			m->removeVertex(v);
			auto data = m->convertToPolygonData();

			Assert::AreEqual(data.vertex_x[0], 5.0, EPS);
			Assert::AreEqual(data.vertex_y[0], 0.0, EPS);
			Assert::AreEqual(data.vertex_x[1], 0.0, EPS);
			Assert::AreEqual(data.vertex_y[1], 5.0, EPS);
			Assert::AreEqual(data.vertex_x[2], 0.0, EPS);
			Assert::AreEqual(data.vertex_y[2], 0.0, EPS);

			Assert::AreEqual(data.edges[0], 0);
			Assert::AreEqual(data.edges[1], 1);
			Assert::AreEqual(data.edges[2], 2);

			Assert::AreEqual(data.faces[0], 0);
			Assert::AreEqual(data.faces[1], 3);
		}

		TEST_METHOD(splitFaces_Ales) {
			vectorD x = { 0, 3, 3, 0 };
			vectorD y = { 0, 0, 6, 6 };
			vectorI edges = { 0, 1, 2, 3 };

			PolygonData* data = new PolygonData();
			data->vertex_x = x;
			data->vertex_y = y;
			data->tryAddFace(edges.size(), edges);

			auto m = Mesh();
			m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);

			m.splitByVerticalGrid();
			auto edge = m.splitFaces(6);
			auto edg2 = m.verticalMesh;
			auto max = m.verticalMax;
			auto min = m.verticalMin;

			auto perims = m.getInternalHorizontalPerimeters();
			auto optimal = m.getOptimalCombinationForInternalPerimeter(perims);
			m.splitMeshByMask(optimal.second);

			Assert::AreEqual(optimal.first, 54.0, EPS);
			Assert::AreEqual(optimal.second, 0LL);
		}

#pragma endregion

	private:
#pragma region Help methods

		Mesh* getTriangleMesh() {
			Mesh* m = new Mesh();
			m->convertFromPolygonDataOfConvexLeftTraversalPolygon(*getTrianglePolygonData());

			return m;
		}

		PolygonData* getTrianglePolygonData() {
			vectorD x = { 5, 0, 0 };
			vectorD y = { 0, 5, 0 };
			vectorI edges = { 0, 1, 2 };

			PolygonData* data = new PolygonData();
			data->vertex_x = x;
			data->vertex_y = y;
			data->tryAddFace(edges.size(), edges);

			return data;
		}

		Mesh* getTestMesh() {
			Mesh* m = new Mesh();
			m->convertFromPolygonDataOfConvexLeftTraversalPolygon(*getTestPolygonData());

			return m;
		}

		PolygonData* getTestPolygonData() {
			vectorD x = { 6, 6, 4, 2, 1, 1, 1, 3 };
			vectorD y = { 1, 3, 5, 5, 4, 3, 2, 1 };
			vectorI edges = { 0, 1, 2, 3, 4, 5, 6, 7 };

			PolygonData* data = new PolygonData();
			data->vertex_x = x;
			data->vertex_y = y;
			data->tryAddFace(edges.size(), edges);

			return data;
		}

		void checkEdge(Edge* e, int numOfEdge, Vertex* vertex, Edge* prev, Edge* next, Edge* sym, Face* face, bool isDeleted) {
			Assert::AreEqual(e->numOfEdge, numOfEdge);
			Assert::IsTrue(e->v == vertex);
			Assert::IsTrue(e->prev == prev);
			Assert::IsTrue(e->next == next);
			Assert::IsTrue(e->sym == sym);
			Assert::IsTrue(e->f == face);
			Assert::IsTrue(e->isDeleted == isDeleted);
		}

		void checkVertex(Vertex* v, Edge* edge, int numOfVertex, bool isDeleted) {
			Assert::IsTrue(v->e == edge);
			Assert::AreEqual(v->numOfVertex, numOfVertex);
			Assert::AreEqual(v->isDeleted, isDeleted);
		}

#pragma endregion
	};
}