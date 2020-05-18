#include "pch.h"
#include "Rotation.h"

namespace algo {

	Rotation::Rotation(double angleInDegrees, RotationDirection direction)
	{
		this->x0 = nullptr;
		this->y0 = nullptr;
		this->xOffset = nullptr;
		this->yOffset = nullptr;

		if (direction == Left) {
			this->angleInDegrees = angleInDegrees;
		}
		else if (direction == Right) {
			this->angleInDegrees = angleInDegrees;
		}
	}

	double Rotation::findAngleOfSegmentInDegrees(double x1, double y1, double x2, double y2)
	{
		auto angle_tg = (x1 - x2) / (y1 - y2);
		auto angle_rad = atan(angle_tg);
		auto angle_degree = angle_rad * 180 / 3.14;

		while (std::abs(angle_degree) > 180) {
			angle_degree -= 180;
		}

		if (angle_degree < 0) {
			angle_degree = 180 - std::abs(angle_degree);
		}

		return angle_degree;
	}

	void Rotation::rotateFigure90(vectorD* x, vectorD* y)
	{
		for (int i = 0; i < x->size(); ++i) {
			auto temp = (*x)[i];
			(*x)[i] = (*y)[i];
			(*y)[i] = temp;
		}
	}

	void Rotation::rotatePoint90(Point* p)
	{
		auto temp = p->x;
		p->x = p->y;
		p->y = temp;
	}

	bool Rotation::tryRotateFigure(vectorD* x, vectorD* y)
	{
		if (std::abs(angleInDegrees) < DBL_EPSILON) {
			return true;
		}

		double degree = -angleInDegrees * 3.14 / 180;
		double angleCos = cos(degree);
		double angleSin = sin(degree);

		calculateParams(*x, *y, angleSin, angleCos);

		for (int i = 0; i < x->size(); ++i) {
			auto p = rotate((*x)[i], (*y)[i], angleCos, angleSin);
			(*x)[i] = p.x;
			(*y)[i] = p.y;
		}

		return true;
	}

	bool Rotation::tryRotateFigureBack(vectorD* x, vectorD* y)
	{
		if (this->xOffset == nullptr || this->yOffset == nullptr || this->x0 == nullptr || this->y0 == nullptr)
			return false;

		if (std::abs(angleInDegrees) < DBL_EPSILON) {
			return true;
		}

		double degree = angleInDegrees * 3.14 / 180;
		double angleCos = cos(degree);
		double angleSin = sin(degree);

		for (int i = 0; i < x->size(); ++i) {
			auto p = rotate((*x)[i], (*y)[i], angleCos, angleSin);
			(*x)[i] = p.x;
			(*y)[i] = p.y;
		}

		return true;
	}

	bool Rotation::tryRotatePointBack(Point* p) {
		if (this->xOffset == nullptr || this->yOffset == nullptr || this->x0 == nullptr || this->y0 == nullptr)
			return false;

		if (std::abs(angleInDegrees) < DBL_EPSILON) {
			return true;
		}

		double degree = angleInDegrees * 3.14 / 180;
		double angleCos = cos(degree);
		double angleSin = sin(degree);


		auto newPoint = rotate(p->x, p->y, angleCos, angleSin);

		p->x = newPoint.x;
		p->y = newPoint.y;
	}

	Point Rotation::rotate(double x1, double y1, double angleCos, double angleSin)
	{
		auto x2 = (*x0 - x1) * angleCos - (*y0 - y1) * angleSin + *xOffset;
		auto y2 = (*x0 - x1) * angleSin + (*y0 - y1) * angleCos + *yOffset;

		return Point{ x2, y2 };
	}

	void Rotation::calculateParams(vectorD x, vectorD y, double sin, double cos)
	{
		double left = x[0], right = x[0], top = y[0], bottom = y[0];

		for (int i = 0; i < x.size(); ++i) {
			if (left > x[i]) {
				left = x[i];
			}
			else if (right < x[i]) {
				right = x[i];
			}
			if (bottom > y[i]) {
				bottom = y[i];
			}
			else if (top < y[i]) {
				top = y[i];
			}
		}

		calculateCentralPoint(left, right, bottom, top);
		calculateOffsets(Point{ left, bottom }, Point{ left, top }, Point{ right, bottom }, Point{ right, top }, sin, cos);
	}

	void Rotation::calculateCentralPoint(double x_min, double x_max, double y_min, double y_max)
	{
		x0 = new double(x_min + (x_max - x_min) / 2);
		y0 = new double(y_min + (y_max - y_min) / 2);
	}

	void Rotation::calculateOffsets(Point left, Point right, Point top, Point bottom, double sin, double cos)
	{
		Point borderPoints[] = { left, right, top, bottom };

		xOffset = x0;
		yOffset = y0;
	}

}