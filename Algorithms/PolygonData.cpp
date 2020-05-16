#include "pch.h"
#include "framework.h"
#include "PolygonData.h"

PolygonData::PolygonData() {
	faces.push_back(0);
}

int PolygonData::getNumOfVertexes() {
	return vertex_x.size();
}

int PolygonData::getNumOfFaces() {
	return faces.size() - 1;
}

bool PolygonData::tryAddPoints(vectorD x, vectorD y) {
	if (x.size() != y.size()) {
		return false;
	}

	for (int i = 0; i < x.size(); ++i) {
		vertex_x.push_back(x[i]);
		vertex_y.push_back(y[i]);
	}

	return true;
}

bool PolygonData::tryAddFace(int numOfSides, vectorI pointsNumbers) {
	if (numOfSides != pointsNumbers.size()) {
		return false;
	}

	for (int i = 0; i < pointsNumbers.size(); ++i) {
		if (pointsNumbers[i] >= vertex_x.size()) {
			return false;
		}
	}

	for (int i = 0; i < pointsNumbers.size(); ++i) {
		edges.push_back(pointsNumbers[i]);
	}

	faces.push_back(edges.size());

	return true;
}



point Rotation::rotate(double x1, double y1, double angleCos, double angleSin)
{
	auto x2 = (*x0 - x1) * angleCos - (*y0 - y1) * angleSin + *xOffset;
	auto y2 = (*x0 - x1) * angleSin + (*y0 - y1) * angleCos + *yOffset;

	return point{ x2, y2 };
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
	calculateOffsets(point{ left, bottom }, point{ left, top }, point{ right, bottom }, point{ right, top }, sin, cos);
}

void Rotation::calculateCentralPoint(double x_min, double x_max, double y_min, double y_max)
{
	x0 = new double(x_min + (x_max - x_min) / 2);
	y0 = new double(y_min + (y_max - y_min) / 2);
}

void Rotation::calculateOffsets(point left, point right, point top, point bottom, double sin, double cos)
{
	point borderPoints[] = { left, right, top, bottom };

	xOffset = x0;
	yOffset = y0;

	/*
	xOffset = new double(0);
	yOffset = new double(0);

	for (int i = 0; i < 4; ++i) {
		borderPoints[i] = rotate(borderPoints[i].x, borderPoints[i].y, cos, sin);
	}

	double x_min = borderPoints[0].x, y_min = borderPoints[0].y;
	for (int i = 0; i < 4; ++i) {
		if (x_min > borderPoints[i].x) {
			x_min = borderPoints[i].x;
		}
		if (y_min > borderPoints[i].y) {
			y_min = borderPoints[i].y;
		}
	}

	if (x_min < 0) {
		xOffset = new double(-x_min);
	}

	if (y_min < 0) {
		yOffset = new double(-y_min);
	}*/
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

std::pair<Algorithms::Point*, Algorithms::Point*> Rotation::findPointsOfSegmentByAngle(double angle, double width, double height)
{
	if (abs(angle) < DBL_EPSILON) {
		return { new Algorithms::Point{ width / 2, height / 10 },  new Algorithms::Point{ width / 2, height / 10 * 9} };
	}

	auto angle_rad = angle / 180 * 3.14;
	auto angle_tg = tan(angle_rad);

	auto y2 = -width / angle_tg;

	auto diff = height * width / y2;

	return { new Algorithms::Point{ diff / 2, 0 },  new Algorithms::Point{ width - diff / 2, height} };
}

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

void Rotation::rotateFigure90(vectorD* x, vectorD* y)
{
	for (int i = 0; i < x->size(); ++i) {
		auto temp = (*x)[i];
		(*x)[i] = (*y)[i];
		(*y)[i] = temp;
	}
}

void Rotation::rotatePoint90(Algorithms::Point* p)
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

bool Rotation::tryRrotateTheFigureBack(vectorD* x, vectorD* y)
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


bool Rotation::tryRotateThePointBack(Algorithms::Point* p) {
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

ConvexPartitionCharacteristics::ConvexPartitionCharacteristics() {}

ConvexPartitionCharacteristics::ConvexPartitionCharacteristics(double areaOfNotSplittedParts, double percentageOfNotSplittedParts, double optimizationFuncValue) {
	this->areaOfNotSplittedParts = areaOfNotSplittedParts;
	this->percentageOfNotSplittedParts = percentageOfNotSplittedParts;
	this->optimizationFuncValue = optimizationFuncValue;
}

double ConvexPartitionCharacteristics::getAreaOfNotSplittedParts(int prec){
	return getDoubleWithPrecision(areaOfNotSplittedParts, prec);
}

double ConvexPartitionCharacteristics::getPercentageOfNotSplittedParts(int prec){
	return getDoubleWithPrecision(percentageOfNotSplittedParts, prec);
}

double ConvexPartitionCharacteristics::getOptimizationFuncValue(int prec){
	return getDoubleWithPrecision(optimizationFuncValue, prec);
}

double ConvexPartitionCharacteristics::getDoubleWithPrecision(double number, int prec) {
	int count = prec;

	while (count > 0) {
		number *= 10;
		count--;
	}

	number = (int)number;

	count = prec;

	while (count > 0) {
		number /= 10;
		count--;
	}

	return number;
}