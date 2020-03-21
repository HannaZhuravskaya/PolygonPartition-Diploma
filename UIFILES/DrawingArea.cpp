#include "DrawingArea.h"

DrawingArea::DrawingArea(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f) {
	setX();
	setY();
	fillByPixmap();
	setScale(10, 10);
	setGridVisibility(false);
}

bool DrawingArea::isPointInBounds(int x, int y) {
	return this->x() <= x
		&& this->y() <= y
		&& this->x() + this->width() >= x
		&& this->y() + this->height() >= y;
}

void DrawingArea::setGeometry(const QRect& rect) {
	QLabel::setGeometry(rect);
	setX();
	setY();
	fillByPixmap();
}

void DrawingArea::fillByPixmap() {
	pixmap = new QPixmap(this->width(), this->height());
	painter = new QPainter(pixmap);
	
	setDefaultPen();

	pixmap->fill(Qt::red);
	this->setPixmap(*pixmap);
}

void DrawingArea::clearDrawArea()
{
	pixmap->fill();
	this->setPixmap(*pixmap);

	setGrid();
}



//QPoint DrawingArea::convertToDrawingAreaCoordinates(QPoint  windowPoint) {
//	return QPoint((double)(windowPoint.x) / xScale, (double)(this->height() - windowPoint.y) / yScale);
//}
//
//QPoint  DrawingArea::convertToWindowCoordinates(QPoint  drawingAreaPoint) {
//	///////
//	auto text = QString("X: %1, Y: %2").arg((double)(x) / xScale).arg((double)(this->height() - y) / yScale);
//	return drawingAreaPoint;
//}
 
void DrawingArea::drawEllipse(QPoint point, int windowRadius, bool isWidjetCoords, QColor fillColor, QColor borderColor , double borderWidth) {
	painter->setPen(QPen(borderColor, borderWidth));
	painter->setBrush(fillColor);

	if (!isWidjetCoords) {
		point = AppToWidjetCoords(point);
	}

	painter->drawEllipse(point.x() - (windowRadius / 2), point.y() - (windowRadius / 2), windowRadius, windowRadius);
	this->setPixmap(*pixmap);

	setDefaultPen();
}

void DrawingArea::drawLine(QPoint firstPoint, QPoint secondPoint, bool isWidjetCoords, QColor fillColor, double borderWidth) {
	painter->setPen(QPen(fillColor, borderWidth));

	if (!isWidjetCoords) {
		firstPoint = AppToWidjetCoords(firstPoint);
		secondPoint = AppToWidjetCoords(secondPoint);
	}

	painter->drawLine(firstPoint.x(), firstPoint.y(), secondPoint.x(), secondPoint.y());
	this->setPixmap(*pixmap);

	setDefaultPen();
}

int DrawingArea::x() {
	return this->xPos;
}

int DrawingArea::y() {
	return this->yPos;
}

void DrawingArea::setX()
{
	this->xPos = QLabel::x();

	QWidget* w = (QWidget*)this->parent();
	while (w->parent() != nullptr) {
		try {
			this->xPos += w->x();
			w = (QWidget*)(w->parent());
		}
		catch (std::exception) {
			break;
		}
	}
}

void DrawingArea::setY()
{
	this->yPos = QLabel::y();

	QWidget* w = (QWidget*)this->parent();
	while (w->parent() != nullptr) {
		try {
			this->yPos += w->y();
			w = (QWidget*)(w->parent());
		}
		catch (std::exception) {
			break;
		}
	}
}

void DrawingArea::setGridVisibility(bool isVisible)
{
	gridVisibility = isVisible;
	clearDrawArea();
}

void DrawingArea::setScale(int x, int y)
{
	xScale = x;
	yScale = y;
}

void DrawingArea::setMouseTracking(bool isMouseTracking, QLabel* mouseCoordinates)
{
	QLabel::setMouseTracking(isMouseTracking);
	this->mouseCoordinates = mouseCoordinates;
}

void DrawingArea::setMouseTracking(bool isMouseTracking)
{
	QLabel::setMouseTracking(isMouseTracking);
}

void  DrawingArea::mouseMoveEvent(QMouseEvent* event)
{
	auto x = event->pos().x();
	auto y = event->pos().y();
	auto text = QString("X: %1, Y: %2; \nwX: %3, wY: %4; \naX: %5, aY: %6; ")
		.arg((double)(x) / xScale)
		.arg((double)(this->height() - y) / yScale)
		.arg(x)
		.arg(y)
		.arg(WidjetToAppCoordes(event->pos()).x())
		.arg(WidjetToAppCoordes(event->pos()).y());
	mouseCoordinates->setText(text);
	mouseCoordinates->setVisible(true);
	mouseCoordinates->setGeometry(x + 40,y +40, 100, 50);
	mouseCoordinates->setFrameShape(Box);
	mouseCoordinates->raise();
}

void DrawingArea::setGrid()
{
	if (gridVisibility) {
		for (int row = yScale; row < this->height(); row += yScale) {
			if ((this->height() - row) / yScale % 5 == 0) {
				drawLine(QPoint(0, row), QPoint(this->width(), row), true, Qt::gray, 0.5);
			}
			else {
				drawLine(QPoint(0, row), QPoint(this->width(), row), true, Qt::gray, 0.1);
			}
		}

		for (int col = xScale; col < this->width(); col += xScale) {
			if (col / xScale % 5 == 0) {
				drawLine(QPoint(col, 0), QPoint(col, this->height()), true, Qt::gray, 0.5);
			}
			else {
				drawLine(QPoint(col, 0), QPoint(col, this->height()), true, Qt::gray, 0.1);
			}
		}
	}
}

void DrawingArea::setDefaultPen()
{
	painter->setPen(QPen(Qt::black, 1));
	painter->setBrush(Qt::black);
}

QPoint DrawingArea::AppToWidjetCoords(QPoint p)
{
	return QPoint(p.x() - xPos, p.y() - yPos);
}

QPoint DrawingArea::WidjetToAppCoordes(QPoint p)
{
	return QPoint(p.x() + xPos, p.y() + yPos);
}

QPointF DrawingArea::PixelToLogicCoords(QPoint p, bool isWidjetCoords)
{
	if (!isWidjetCoords) {
		p = AppToWidjetCoords(p);
	}

	return QPointF((double)(p.x()) / xScale, (double)(this->height() - p.y()) / yScale);
}

QPoint DrawingArea::LogicToPixelCoords(QPointF p, bool isWidjetCoords)
{
	auto pixelPoint = QPoint(p.x() * xScale, this->height() - p.y() * yScale);

	if (!isWidjetCoords) {
		pixelPoint = WidjetToAppCoordes(pixelPoint);
	}

	return pixelPoint;
}