#include "DrawingArea.h"

DrawingArea::DrawingArea(QWidget* parent, Qt::WindowFlags f) : QLabel(parent, f) {
	setX();
	setY();
	fillByPixmap();
	setScale(10, 10);
	setGridVisibility(false);
	setHoverEffectEnabled();
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

	this->setPixmap(*pixmap);
}

void DrawingArea::clearDrawArea()
{
	pixmap->fill();
	this->setPixmap(*pixmap);

	setGrid();
}
 
void DrawingArea::drawEllipse(QPoint point, int windowRadius, bool isWidjetCoords, QColor fillColor, QColor borderColor , double borderWidth) {
	painter->setPen(QPen(borderColor, borderWidth));
	painter->setBrush(fillColor);

	if (!isWidjetCoords) {
		point = AppToWidjetCoords(point);
	}

	painter->drawEllipse(point.x() - windowRadius , point.y() - windowRadius , 2 * windowRadius, 2 * windowRadius);
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

void DrawingArea::drawArc(double ang, QPoint left, QPoint center, QPoint top, int offsetInPixels, bool isWidjetCoords, QColor fillColor,QColor borderColor, double borderWidth)
{
	painter->setPen(QPen(borderColor, borderWidth));
	painter->setBrush(fillColor);

	if (!isWidjetCoords) {
		left = AppToWidjetCoords(left);
		center = AppToWidjetCoords(center);
		top = AppToWidjetCoords(top);
	}

	painter->drawPie(center.x() - offsetInPixels, center.y() - offsetInPixels, 2 * offsetInPixels, 2 * offsetInPixels, 90 * 16, ang * 16);
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

			QTabWidget* tab = dynamic_cast<QTabWidget*>(w);
			if (tab != nullptr)
				this->xPos += 2;

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

			QTabWidget* tab = dynamic_cast<QTabWidget*>(w);
			if (tab != nullptr)
				this->yPos += 26;

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
	if (mouseCoordinates == nullptr)
		return;

	auto x = event->pos().x();
	auto y = event->pos().y();
	auto text = QString("X: %1, Y: %2")
		.arg((double)(x) / xScale)
		.arg((double)(this->height() - y) / yScale);
		mouseCoordinates->setText(text);
		mouseCoordinates->setVisible(true);
		mouseCoordinates->setGeometry(x + 40, y + 40, 100, 20);
		mouseCoordinates->setFrameShape(Box);
		mouseCoordinates->raise();
}

void DrawingArea::saveImage(char* fileName, int borderWidth)
{
	auto pixmapWithBorder = new QPixmap(pixmap->width() + borderWidth * 2, pixmap->height() + borderWidth * 2);
	
	painter = new QPainter(pixmapWithBorder);
	painter->setBrush(Qt::black);
	painter->drawRect(0, 0, pixmapWithBorder->width(), pixmapWithBorder->height());
	painter->drawPixmap(borderWidth, borderWidth, *pixmap);

	pixmapWithBorder->toImage().save(QString(fileName));
}

void DrawingArea::setHoverEffectEnabled(bool isEnable)
{
	isHoverEffectEnabled = isEnable;
	leaveEvent(nullptr);
}

void DrawingArea::deleteAllEffects()
{
	isHoverEffectEnabled = false;
	auto effect = new QGraphicsColorizeEffect();
	effect->setColor(QColor(0, 0, 0));
	setGraphicsEffect(effect);
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

void DrawingArea::enterEvent(QEvent* ev)
{
	if (isHoverEffectEnabled) {
		auto effect = new QGraphicsColorizeEffect();
		effect->setColor(QColor(0, 192, 0));
		setGraphicsEffect(effect);
	}
}

void DrawingArea::leaveEvent(QEvent* ev)
{
	if (isHoverEffectEnabled) {
		auto effect = new QGraphicsColorizeEffect();
		effect->setColor(QColor(0, 0, 192));
		setGraphicsEffect(effect);
	}
}