#include "pch.h"

namespace AlgorithmTests
{
	TEST_CLASS(PolygonTests)
	{
	public:
#pragma region Constructors
		TEST_METHOD(Polygon_NoArgs_SizesAreZero)
		{
			Polygon pol = Polygon();

			Assert::AreEqual(0, pol.getNumOfSides());
			Assert::AreEqual(0, pol.getNumOfPoints());
		}

		TEST_METHOD(Polygon_InvalidNumOfSides_InvalidArgumentException)
		{
			auto func = [this] {
				int numOfSides = 2;
				Polygon pol = Polygon(numOfSides);
			};

			Assert::ExpectException<std::invalid_argument>(func);
		}

		TEST_METHOD(Polygon_ValidNumOfSides_NumOfSidesNotZeroPointsZero)
		{
			int numOfSides = 3;
			Polygon pol = Polygon(numOfSides);

			Assert::AreEqual(numOfSides, pol.getNumOfSides());
			Assert::AreEqual(0, pol.getNumOfPoints());
		}

		TEST_METHOD(Polygon_InvalidNumOfPoints_NumOfSidesNotZeroPointsZero)
		{
			int numOfSides = 3;
			Polygon pol = Polygon(numOfSides);

			Assert::AreEqual(numOfSides, pol.getNumOfSides());
			Assert::AreEqual(0, pol.getNumOfPoints());
		}

		TEST_METHOD(Polygon_InvalidNumOfSidesWithPoint_InvalidArgumentException)
		{
			auto func = [this] {
				int numOfSides = 2;
				std::vector<Point> points;
				Polygon pol = Polygon(numOfSides, points);
			};

			Assert::ExpectException<std::invalid_argument>(func);
		}

		TEST_METHOD(Polygon_InvalidNumPoints_InvalidArgumentException)
		{
			auto func = [this] {
				int numOfSides = 3;
				std::vector<Point> points;
				Polygon pol = Polygon(numOfSides, points);
			};

			Assert::ExpectException<std::invalid_argument>(func);
		}

		TEST_METHOD(Polygon_ValidNumOfSidesAndNumOfPoints_NumOdSidesAndNumOfPointsEqual)
		{
			int numOfSides = 3;
			std::vector<Point> points;
			points.push_back({ 1,1 });
			points.push_back({ 1,1 });
			points.push_back({ 1,1 });

			Polygon pol = Polygon(numOfSides, points);

			Assert::AreEqual(numOfSides, pol.getNumOfSides());
			Assert::AreEqual(numOfSides, pol.getNumOfPoints());
		}
#pragma endregion
#pragma region Public methods
		TEST_METHOD(getSquare_InvalidNumberOfPoints_LogicErrorException)
		{
			auto func = [this]() {
				Polygon pol = Polygon(3);
				pol.tryAddPoint({ 1,1 });
				pol.getSquare();
			};

			Assert::ExpectException<std::logic_error>(func);
		}
		TEST_METHOD(getSquare_SquareOfTriangle_SquareIs6)
		{
			int numOfSides = 3;

			std::vector<Point> points;
			points.push_back({ 0,0 });
			points.push_back({ 3,0 });
			points.push_back({ 0,4 });

			Polygon pol = Polygon(numOfSides, points);

			Assert::AreEqual((double)6, pol.getSquare(), DBL_EPSILON);
		}
		TEST_METHOD(getSquare_SquareOfSquare_SquareIs4)
		{
			int numOfSides = 4;

			std::vector<Point> points;
			points.push_back({ 0,0 });
			points.push_back({ 2,0 });
			points.push_back({ 2,2 });
			points.push_back({ 0,2 });

			Polygon pol = Polygon(numOfSides, points);

			Assert::AreEqual((double)4, pol.getSquare(), DBL_EPSILON);
		}

		TEST_METHOD(getPerimeter_PerimeterOfSquare_SquareIs8)
		{
			int numOfSides = 4;

			std::vector<Point> points;
			points.push_back({ 0,0 });
			points.push_back({ 2,0 });
			points.push_back({ 2,2 });
			points.push_back({ 0,2 });

			Polygon pol = Polygon(numOfSides, points);

			Assert::AreEqual(8.0, pol.getPerimeter(), DBL_EPSILON);
		}

		TEST_METHOD(getSquare_SquareOfIrregularPolygon_SquareIs9)
		{
			int numOfSides = 5;

			std::vector<Point> points;
			points.push_back({ 1,1 });
			points.push_back({ 2,2 });
			points.push_back({ 5,7 });
			points.push_back({ 4,8 });
			points.push_back({ 1,3 });

			Polygon pol = Polygon(numOfSides, points);

			Assert::AreEqual((double)9, pol.getSquare(), DBL_EPSILON);
		}

		TEST_METHOD(isConvex_InvalidNumberOfPoints_LogicErrorException)
		{
			auto func = [this]() {
				Polygon pol = Polygon(3);
				pol.tryAddPoint({ 1,1 });
				pol.isConvex();
			};

			Assert::ExpectException<std::logic_error>(func);
		}

		TEST_METHOD(isConvex_Square_ConvexPolygon)
		{
			int numOfSides = 4;

			std::vector<Point> points;
			points.push_back({ 0,0 });
			points.push_back({ 2,0 });
			points.push_back({ 2,2 });
			points.push_back({ 0,2 });

			Polygon pol = Polygon(numOfSides, points);

			Assert::AreEqual(true, pol.isConvex());
		}

		TEST_METHOD(isConvex_NotConvexPolygon_NotConvexPolygon)
		{
			int numOfSides = 4;

			std::vector<Point> points;
			points.push_back({ 0,0 });
			points.push_back({ 2,0 });
			points.push_back({ 1,1 });
			points.push_back({ 0,2 });

			Polygon pol = Polygon(numOfSides, points);

			Assert::AreEqual(true, pol.isConvex());
		}
#pragma endregion
	};
}