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
#include <DrawingArea.h>
#include <qspinbox.h>

namespace a = Algorithms;

class ApplicationGUI : public QWidget
{
	Q_OBJECT

public:
	ApplicationGUI(QWidget* parent = Q_NULLPTR);

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void btn_apply_clicked(bool checked);
	void btn_reset_clicked(bool checked);
	void btn_apply_area_clicked(bool checked);
	void btn_reset_area_clicked(bool checked);
	void btn_apply_angle_clicked(bool checked);
	void btn_reset_angle_clicked(bool checked);
	void calculatePolygonProperties();
	void calculateAreaProperties();

private:
	Ui::ApplicationGUIClass ui;

	QLabel* mouseCoordinates;

	std::string activeGroupBox;

	a::Polygon* polygon;
	double polygonArea;
	std::pair<a::Point*, a::Point*> anglePoints;

	void initializeControls();
	void setPolygonPropertiesVisibility(bool isEnabled);
	void setAreaPropertiesVisibility(bool isEnabled);
	void setDefaultPolygonProperties();
	void setDefaultAreaProperties();
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
};
