#include "pch.h"
#include "Helper.h"

namespace algo {

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

	double Helper::crossProductLength(Point a, Point b, Point c)
	{
		float BAx = a.x - b.x;
		float BAy = a.y - b.y;
		float BCx = c.x - b.x;
		float BCy = c.y - b.y;

		// Calculate the Z coordinate of the cross product.
		return (BAx * BCy - BAy * BCx);
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

	Point* Helper::findPointOnSegmentByDistance(Point* p1, Point* p2, double distance, double eps)
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
			Helper::addValidRootsToList(roots, validRoots);

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
			Helper::addValidRootsToList(roots, validRoots);

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

	bool Helper::tryNextPermutation(std::vector<int>* a)
	{
		int n = a->size();
		int j = n - 2;
		while (j != -1 && (*a)[j] >= (*a)[j + 1]) j--;
		if (j == -1)
			return false; // no more permutations
		int k = n - 1;
		while ((*a)[j] >= (*a)[k]) k--;
		swap(a, j, k);
		int l = j + 1, r = n - 1; // sort the rest of the sequence
		while (l < r)
			swap(a, l++, r--);
		return true;
	}

	std::vector<std::string> Helper::splitStringByRegex(const std::string& stringToSplit, const std::string& regexPattern)
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

	bool Helper::isEqual(double a, double b) {
		return abs(a - b) < EPS;
	}

	bool Helper::isNotEqual(double a, double b) {
		return abs(a - b) > EPS;
	}

	bool Helper::isGreater(double a, double b) {
		return a - b > EPS;
	}

	bool Helper::isLess(double a, double b) {
		return b - a > EPS;
	}

	inline double Helper::isLeft(Point* P0, Point* P1, Point* P2)
	{
		return ((P1->x - P0->x) * (P2->y - P0->y)
			- (P2->x - P0->x) * (P1->y - P0->y));
	}

}