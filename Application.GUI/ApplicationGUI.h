#pragma once
#include <utility> 
#include <QtWidgets/QWidget>
#include <qevent.h>
#include "ui_ApplicationGUI.h"
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <string>
#include "../Algorithms/Polygon.h"
#include "..\Algorithms\PolygonData.h"
#include "..\Algorithms\PolygonalMesh.h"
#include "..\Application.GUI\DrawingArea.h"
#include <qspinbox.h>

namespace a = Algorithms;

#define SLD_SCALE 10.0

class ApplicationGUI : public QWidget
{
	Q_OBJECT

public:
	ApplicationGUI(QWidget* parent = Q_NULLPTR);

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void btn_apply_clicked(bool checked);
	void btn_reset_clicked(bool checked);
	void btn_apply_angle_clicked(bool checked);
	void btn_reset_angle_clicked(bool checked);
	void btn_saveResults(bool checked);
	void sliderRangeChanged();
	void areaOfPartRangeChanged(int min, int max);
	void calculatePolygonProperties();

private:
	Ui::ApplicationGUIClass ui;

	QLabel* mouseCoordinates;

	std::string activeGroupBox;

	a::Polygon* polygon;
	double polygonArea;
	std::pair<a::Point*, a::Point*> anglePoints;

	void initializeControls();
	void setActiveGroupBox(std::string grb_name, bool isNext);

	void addPointToPolygon(int x, int y);
	void addPointToAngle(int x, int y);
	bool isInMeshAngleDrawingAreaBounds(int x, int y);
	void mouseMoveEvent(QMouseEvent* event);

	//TO DO: Delete
	void createDefaultPolygonMesh();
	void testMethod();
	void convertToMesh();
	void convertToMeshTEST();
    ///

	void drawPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces);

	void setSliderLabelsPosition();
};
