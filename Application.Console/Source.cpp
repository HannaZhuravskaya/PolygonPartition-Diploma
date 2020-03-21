#include <iostream>
#include <tuple>
#include <vector>

using namespace std;

std::tuple<double, double, double> findCoeff(double x1, double y1, double x2, double y2) {
	return std::make_tuple(y1 - y2, x2 - x1, x1 * y2 - x2 * y1);
}

std::tuple<int, double, double> findRoots(double a, double b, double c) {
	int numOfRoots;
	double x1, x2;

	if (abs(a) < DBL_EPSILON) {
		numOfRoots = 1;
		x1 = -c / b;
	}
	else {
		auto D = b * b - 4 * a * c;
		
		if (D < 0) {
			numOfRoots = 0;
		}
		else if (std::abs(D) < DBL_EPSILON) {
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

void addRoots(std::tuple<int, double, double> r1, vector<double> *roots) {
	if (std::get<0>(r1) == 0)
		return;
	roots->push_back(std::get<1>(r1));

	if (std::get<0>(r1) == 1)
		return;
	roots->push_back(std::get<2>(r1));
}

void findPoints(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double S, double a3, double b3) {
	auto coeff1 = findCoeff(x4, y4, x1, y1), coeff2 = findCoeff(x2, y2, x3, y3);
	double a1 = std::get<0>(coeff1), b1 = std::get<1>(coeff1), c1 = std::get<2>(coeff1);
	double a2 = std::get<0>(coeff2), b2 = std::get<1>(coeff2), c2 = std::get<2>(coeff2);
	auto k1 = a1 * b3 - a3 * b1;
	auto k2 = a2 * b3 - a3 * b2;
	auto a = b1 * a2 - a1 * b2;
	auto b = (b2 * c1 - b1 * c2) * a3 +
		(a1 * c2 - a2 * c1) * b3 +
		y1 * k2 * b1 + x1 * k2 * a1 - x2 * k1 * a2 - y2 * k1 * b2;
	auto c = k1 * k2 * (x1 * y2 - x2 * y1) +
		x2 * k1 * c2 * a3 - x1 * k2 * c1 * a3 +
		y2 * k1 * c2 * b3 - y1 * k2 * c1 * b3;
	auto c_pos = c + 2 * S * k1 * k2;
	auto c_neg = c - 2 * S * k1 * k2;

	auto roots1 = findRoots(a, b, c_neg);
	auto roots2 = findRoots(a, b, c_pos);

	vector<double>* roots = new vector<double>();
	addRoots(roots1, roots);
	addRoots(roots2, roots);

	for (int i = 0; i < roots->size(); ++i) {
		double c3 = (*roots)[i];
		double x5 = (c3 * b2 - c2 * b3) / k2,
			y5 = (c2 * a3 - c3 * a2) / k2,
			x6 = (c3 * b1 - c1 * b3) / k1,
			y6 = (c1 * a3 - c3 * a1) / k1;

		auto check_c3 = x5 * y6 - x6 * y5;
		auto b3_check = x6 - x5;
		auto a3_check = y5 - y6;

		if (abs(b3_check) > DBL_EPSILON && std::abs((check_c3 / b3_check) - c3) > DBL_EPSILON) {
			cout << "WRONG(c3 not prop) c3: " << c3 << endl;
		}
		else if (abs(a3_check) > DBL_EPSILON && ((x2 <= x5 && x3 <= x5) || (x2 >= x5 && x3 >= x5) || (x1 <= x6 && x4 <= x6) || (x1 >= x6 && x4 >= x6))) {
			cout << "WRONG(not in bounds) c3: " << c3 << endl;
		}
		else {
			cout << "Good answer" << endl;
			cout << "c3: " << c3 << endl;
			cout << "x5: " << x5 << ", y5: " << y5 << endl;
			cout << "x6: " << x6 << ", y6: " << y6 << endl;
		}
	}
}

void findPointsV(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double S) {
	auto coeff1 = findCoeff(x4, y4, x1, y1), coeff2 = findCoeff(x2, y2, x3, y3);
	double a1 = std::get<0>(coeff1), b1 = std::get<1>(coeff1), c1 = std::get<2>(coeff1);
	double a2 = std::get<0>(coeff2), b2 = std::get<1>(coeff2), c2 = std::get<2>(coeff2);
	auto k1 =  -  b1;
	auto k2 =  - b2;
	auto a = b1 * a2 - a1 * b2;
	auto b = b2 * c1 - b1 * c2 + y1 * k2 * b1 + x1 * k2 * a1 - x2 * k1 * a2 - y2 * k1 * b2;
	auto c = k1 * k2 * (x1 * y2 - x2 * y1) + x2 * k1 * c2  - x1 * k2 * c1;
	auto c_pos = c + 2 * S * k1 * k2;
	auto c_neg = c - 2 * S * k1 * k2;

	auto roots1 = findRoots(a, b, c_neg);
	auto roots2 = findRoots(a, b, c_pos);

	vector<double>* roots = new vector<double>();
	addRoots(roots1, roots);
	addRoots(roots2, roots);

	for (int i = 0; i < roots->size(); ++i) {
		double c3 = (*roots)[i];
		double x5 = (c3 * b2) / k2,
			y5 = (c2  - c3 * a2) / k2,
			x6 = (c3 * b1) / k1,
			y6 = (c1  - c3 * a1) / k1;

		auto check_c3 = x5 * y6 - x6 * y5;
		auto b3_check = x6 - x5;
		auto a3_check = y5 - y6;

		if (abs(b3_check) > DBL_EPSILON&& std::abs((check_c3 / b3_check) - c3) > DBL_EPSILON) {
			cout << "WRONG(c3 not prop) c3: " << c3 << endl;
		}
		else if (abs(a3_check) > DBL_EPSILON && ((x2 <= x5 && x3 <= x5) || (x2 >= x5 && x3 >= x5) || (x1 <= x6 && x4 <= x6) || (x1 >= x6 && x4 >= x6))) {
			cout << "WRONG(not in bounds) c3: " << c3 << endl;
		}
		else {
			cout << "Good answer" << endl;
			cout << "c3: " << c3 << endl;
			cout << "x5: " << x5 << ", y5: " << y5 << endl;
			cout << "x6: " << x6 << ", y6: " << y6 << endl;
		}
	}
}

void main() {
	//findPoints(
	//	2, 5,
	//	5, 6,
	//	8, 1,
	//	1, 2,
	//	7,
	//	-0.1064, 1);

	//findPoints(
	//	0, 0,
	//	1, 2,
	//	4, 2,
	//	3, 0,
	//	3,
	//	1, 0);

	//findPointsV(
	//	0, 0,
	//	1, 2,
	//	4, 2,
	//	3, 0,
	//	3);

	
}