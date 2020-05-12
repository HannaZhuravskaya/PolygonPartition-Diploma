#include "pch.h"
#include "framework.h"
#include "Polygon.h"

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

	double perimeter = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));

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

	//isLeftTraversal = got_negative;

	return true;
}

bool Polygon::isLeftTraversal(double eps) {
	isPointsCompleted();

	auto numOfMinY = 0;
	for (int i = 1; i < numOfSides; i++)
	{
		if ((points.at(numOfMinY).y - points.at(i).y) > eps) {
			numOfMinY = i;
		}
	}

	auto prev = points.at((numOfMinY + numOfSides - 1) % numOfSides);
	auto next = points.at((numOfMinY + 1) % numOfSides);
	auto cur = points.at(numOfMinY);

	if (abs(prev.y - cur.y) < eps && abs(cur.y - next.y) < eps) {
		if (prev.x < next.x) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		auto d = next.x * prev.y - prev.x * next.y;
		if(d < -eps){
			return false;
		}
		else {
			return true;
		}
	}
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


bool Intersection::onSegment(Point p, Point q, Point r)
{
	if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
		q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
		return true;

	return false;
}


int Intersection::orientation(Point p, Point q, Point r)
{
	int val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 

	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

bool Intersection::doIntersect(Point p1, Point q1, Point p2, Point q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;

	// p1, q1 and q2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

double Helper::crossProductLength(Point a, Point b, Point c)
{
	float BAx = a.x - b.x;
	float BAy = a.y - b.y;
	float BCx = c.x - b.x;
	float BCy = c.y - b.y;

	// Calculate the Z coordinate of the cross product.
	return (BAx * BCy - BAy * BCx);
}

std::tuple<int, double, double> Helper::findRootsOfEquation(double a, double b, double c, double eps) {
	int numOfRoots;
	double x1, x2;

	if (abs(a) < eps) {
		numOfRoots = 1;
		x1 = -c / b;
	}
	else {
		auto D = b * b - 4 * a * c;

		if (D < 0) {
			numOfRoots = 0;
		}
		else if (std::abs(D) < eps) {
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
	return { numOfRoots, x1, x2 };
}

std::tuple<double, double, double> Helper::findLineCoefficients(double x1, double y1, double x2, double y2) {
	return { y1 - y2, x2 - x1, x1 * y2 - x2 * y1 };
}



void Helper::addValidRootsToList(std::tuple<int, double, double> r1, std::vector<double>* roots) {
	if (std::get<0>(r1) == 0)
		return;
	roots->push_back(std::get<1>(r1));

	if (std::get<0>(r1) == 1)
		return;
	roots->push_back(std::get<2>(r1));
}

Point* Algorithms::Helper::findPointOnSegmentByDistance(Point* p1, Point* p2, double distance, double eps)
{
	auto x1 = p1->x, y1 = p1->y, x2 = p2->x, y2 = p2->y;
	auto lineCoeff = findLineCoefficients(x1, y1, x2, y2);
	auto a = std::get<0>(lineCoeff), b = std::get<1>(lineCoeff), c = std::get<2>(lineCoeff);

	if (abs(a) > eps) {
		auto eqA = b * b + a * a;
		auto eqB = 2 * a * x1 * b + 2 * c * b - 2 * y1 * a * a;
		auto eqC = (a * x1 + c) * (a * x1 + c) + y1 * y1 * a * a - a * a * distance * distance;

		auto roots = findRootsOfEquation(eqA, eqB, eqC, 0.0001);

		std::vector<double>* validRoots = new std::vector<double>();
		Algorithms::Helper::addValidRootsToList(roots, validRoots);

		for (int i = 0; i < validRoots->size(); ++i) {
			double y0 = (*validRoots)[i];
			///a = 0?
			double x0 = (-c - b * y0) / a;

			if ((x0 - x1 > eps&& x0 - x2 > eps)
				|| (x1 - x0 > eps&& x2 - x0 > eps)
				|| (y0 - y1 > eps&& y0 - y2 > eps)
				|| (y1 - y0 > eps&& y2 - y0 > eps)) 
			{
				continue;
			}
			else {
				return  new Point{ x0, y0 };
			}
			
		}
	}
	else {
		auto eqA = 1;
		auto eqB = -2 * x1;
		auto eqC = x1 * x1 - distance * distance + (y1 + (c / b)) * (y1 + (c / b));

		auto roots = findRootsOfEquation(eqA, eqB, eqC, 0.0001);

		std::vector<double>* validRoots = new std::vector<double>();
		Algorithms::Helper::addValidRootsToList(roots, validRoots);

		double y0 = -c / b;

		for (int i = 0; i < validRoots->size(); ++i) {
			double x0 = (*validRoots)[i];
			
			if ((x0 - x1 > eps&& x0 - x2 > eps) || (x1 - x0 > eps&& x2 - x0 > eps))
			{
				continue;
			}
			else {
				return  new Point{ x0, y0 };
			}
		}
	}

	return nullptr;
}

void Helper::startPermutation(std::vector<int>* a) {
	std::sort(a->begin(), a->end());
}

void Helper::swap(std::vector<int>* a, int i, int j)
{
	int s = (*a)[i];
	(*a)[i] = (*a)[j];
	(*a)[j] = s;
}

bool Helper::tryNextPermutation(std::vector<int> *a)
{
	int n = a->size();
	int j = n - 2;
	while (j != -1 && (*a)[j] >= (*a)[j + 1]) j--;
	if (j == -1)
		return false; // больше перестановок нет
	int k = n - 1;
	while ((*a)[j] >= (*a)[k]) k--;
	swap(a, j, k);
	int l = j + 1, r = n - 1; // сортируем оставшуюся часть последовательности
	while (l < r)
		swap(a, l++, r--);
	return true;
}