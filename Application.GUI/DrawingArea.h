#pragma once

#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QTabWidget>
#include <QGraphicsEffect>


#include <qevent.h>
class DrawingArea : public QLabel
{
	Q_OBJECT

private:
	QPainter* painter;
	QPixmap* pixmap;
	QLabel* mouseCoordinates;

	int xPos, yPos;
	int xScale, yScale;
	bool gridVisibility;

	bool isHoverEffectEnabled;
public:
	DrawingArea(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

public:
	bool isPointInBounds(int x, int y);

	void setGeometry(const QRect& rect);
	int x();
	int y();
	void setX();
	void setY();
	void setGridVisibility(bool isVisible);

	void setScale(int x, int y);

	void fillByPixmap();
	void clearDrawArea();

	QPointF PixelToLogicCoords(QPoint p, bool isWidjetCoords);
	QPoint LogicToPixelCoords(QPointF p, bool isWidjetCoords);

	void drawEllipse(QPoint point, int windowRadius, bool isWidjetCoords, QColor fillColor = Qt::black, QColor borderColor = Qt::black, double borderWidth = 1);
	void drawLine(QPoint firstPoint, QPoint secondPoint, bool isWidjetCoords, QColor fillColor = Qt::black, double borderWidth = 1);
	void drawArc(double ang, QPoint left, QPoint center, QPoint top, int offsetInPixels, bool isWidjetCoords, QColor fillColor = Qt::black, QColor borderColor = Qt::black, double borderWidth = 1);

	void setMouseTracking(bool isMouseTracking, QLabel* mouseCoordinates);
	void setMouseTracking(bool isMouseTracking);
	void mouseMoveEvent(QMouseEvent* event);

	void saveImage(char* fileName, int borderWidth = 1);

	void setHoverEffectEnabled(bool isEnable = false);
	void deleteAllEffects();

private:
	void setGrid();
	void setDefaultPen();
	QPoint AppToWidjetCoords(QPoint p);
	QPoint WidjetToAppCoordes(QPoint p);

protected:
	void enterEvent(QEvent* ev) override;
	void leaveEvent(QEvent* ev) override;
};