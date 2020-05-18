#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace AlgorithmTests
{
	TEST_CLASS(PolygonDataTests)
	{
	public:
#pragma region Constructors
		TEST_METHOD(PolygonData_NoArgs_SizesAreZero)
		{
			PolygonData polData = PolygonData();

			Assert::AreEqual(0, polData.getNumOfFaces());
			Assert::AreEqual(0, polData.getNumOfVertexes());
		}

#pragma endregion

#pragma region Public methods
		TEST_METHOD(TryAddPoints_ValidData_NumberOfVertex3)
		{
			PolygonData polData = PolygonData();
			int numOfPoints = 3;
			
			vectorD x(numOfPoints);
			vectorD y(numOfPoints);
			Assert::AreEqual(true, polData.tryAddPoints(x, y));

			Assert::AreEqual(0, polData.getNumOfFaces());
			Assert::AreEqual(numOfPoints, polData.getNumOfVertexes());
		}

		TEST_METHOD(TryAddPoints_DifferentSizeOfCoordinates_CannotAddPoints)
		{
			PolygonData polData = PolygonData();

			vectorD x(3);
			vectorD y(1);
			Assert::AreEqual(false, polData.tryAddPoints(x, y));

			Assert::AreEqual(0, polData.getNumOfFaces());
			Assert::AreEqual(0, polData.getNumOfVertexes());
		}

		TEST_METHOD(TryAddFace_ValidData_NumberOfFaces1)
		{
			PolygonData polData = PolygonData();
			int numOfPoints = 5;

			vectorI points(3);

			polData.tryAddPoints(vectorD(numOfPoints), vectorD(numOfPoints));
			Assert::AreEqual(true, polData.tryAddFace(points.size(), points));

			Assert::AreEqual(1, polData.getNumOfFaces());
			Assert::AreEqual(numOfPoints, polData.getNumOfVertexes());
		}

#pragma endregion
	};
}