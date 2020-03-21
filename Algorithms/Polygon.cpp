#include "pch.h"
#include "framework.h"
#include "Polygon.h"

// This is an example of an exported variable
//ALGORITHMS_API int nAlgorithms=0;
//
//// This is an example of an exported function.
//ALGORITHMS_API int fnAlgorithms(void)
//{
//    return 5;
//}
using namespace Algorithms;

Polygon::Polygon() {
	numOfSides = 0;
}

Polygon::Polygon(int numOfSides) {
	if (numOfSides < 3) {
		throw std::invalid_argument("Number of polygon sides must not be less than 3.");
	}

	this->numOfSides = numOfSides;
}

Polygon::Polygon(int numOfSides, std::vector<Point> points) : Polygon(numOfSides) {
	if (points.size() != numOfSides) {
		throw std::invalid_argument("Number of polygon points must be equal to number of polygon sides.");
	}

	for (int i = 0; i < points.size(); ++i) {
		this->points.push_back({ points.at(i).x, points.at(i).y });
	}
}

int Polygon::getNumOfSides() {
	return this->numOfSides;
}

int Polygon::getNumOfPoints()
{
	return this->points.size();
}

bool Polygon::tryAddPoint(Point point)
{
	if (this->points.size() >= this->numOfSides) {
		return false;
	}

	this->points.push_back({ point.x, point.y });
}

double Polygon::getSquare()
{
	isPointsCompleted();

	Point p1, p2;
	p1 = points.at(numOfSides - 1);
	p2 = points.at(0);

	double square = (double)p1.x * p2.y - (double)p1.y * p2.x;

	for (int i = 0; i < numOfSides - 1; ++i) {
		p1 = points.at(i);
		p2 = points.at(i + 1);

		square += (double)p1.x * p2.y - (double)p1.y * p2.x;
	}

	return std::abs(square / 2);
}

double Polygon::getPerimeter()
{
	isPointsCompleted();

	Point p1, p2;
	p1 = points.at(numOfSides - 1);
	p2 = points.at(0);

	double perimeter = sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));

	for (int i = 0; i < numOfSides - 1; ++i) {
		p1 = points.at(i);
		p2 = points.at(i + 1);

		perimeter += sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
	}

	return perimeter;
}


void Polygon::isPointsCompleted() {
	if (this->numOfSides != this->points.size()) {
		std::logic_error("Number of sides must be equal to number of points.");
	}
}

//////////////////////////
double Polygon::crossProductLength(Point a, Point b, Point c)
{
	float BAx = a.x - b.x;
	float BAy = a.y - b.y;
	float BCx = c.x - b.x;
	float BCy = c.y - b.y;

	// Calculate the Z coordinate of the cross product.
	return (BAx * BCy - BAy * BCx);
}
//////////////////////

///do not work

bool Polygon::isConvex()
{
	isPointsCompleted();

	bool got_negative = false;
	bool got_positive = false;

	for (int i = 0; i < numOfSides; i++)
	{
		double cross_product = crossProductLength(
			points.at(i), 
			points.at((i + 1) % numOfSides), 
			points.at((i + 2) % numOfSides));

		if (cross_product < 0)
		{
			got_negative = true;
		}
		else if (cross_product > 0)
		{
			got_positive = true;
		}

		if (got_negative && got_positive) {
			return false;
		}
	}

	isLeftTraversal = got_negative;

	return true;
}

Point Polygon::getPreviousPoint() {
	if (points.size() > 1) {
		return points.at(points.size() - 2);
	}

	return { -1,-1 };
}

Point Polygon::getPointAt(int index)
{
	if (index < points.size() && index >= 0) {
		return points.at(index);
	}

	return { -1,-1 };
}

