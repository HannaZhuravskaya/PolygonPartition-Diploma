#pragma once
#include <utility> 
#include <QtWidgets/QWidget>
#include <qevent.h>
#include "ui_ApplicationGUI.h"
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QCheckBox>
#include <QDir>
#include <QMessageBox>
#include <string>
#include "../Algorithms/Polygon.h"
#include "..\Algorithms\PolygonData.h"
#include "..\Algorithms\PolygonalMesh.h"
#include "..\Application.GUI\DrawingArea.h"
#include <qspinbox.h>
#include <fstream>
#include <streambuf>

namespace a = Algorithms;

#define SLD_SCALE 10.0

class ApplicationGUI : public QWidget
{
	Q_OBJECT

public:
	ApplicationGUI(QWidget* parent = Q_NULLPTR);

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void btn_apply_clicked(bool checked);
	void btn_reset_clicked(bool checked);
	void btn_apply_angle_clicked(bool checked);
	void btn_reset_angle_clicked(bool checked);
	void sliderRangeChanged();
	void areaOfPartRangeChanged(int min, int max);
	void calculatePolygonProperties();

	void btn_saveAsImage(bool checked);
	void btn_drawPoly(bool checked);
	void btn_doAlgo(bool checked);
	void btn_saveMeshAsText(bool checked);
	void btn_uploadMeshFromText(bool checked);
	void saveDrawingAreaAsImage(DrawingArea* drawingAreaOfMesh);
	void saveMeshAsText(DrawingArea* drawingAreaOfMesh);
	void uploadMeshFromText(DrawingArea* drawingAreaOfMesh);

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

	PolygonData* convertPolygonToPolygonData();

	//TO DO: Delete
	void convertToSplittedMesh();
	///

	void drawPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces);

	void setSliderLabelsPosition();

	//////////////////////////////
	std::string polygonDrawingAreaMesh;
	std::string test1;
	std::string test2;
	std::string test3;
	std::string test4;


	bool isModeToSelectDrawingArea;
	std::vector<bool> controlsStatesBeforeSelectingMode;
	bool isHintForSelectDrawingAreaShowned;

	std::vector<Mesh*> meshes;
	int currentMesh;
	bool tryDrawNextMesh();
	bool tryDrawPrevMesh();

	void setControlsDependsOnSelectingMode(bool isModeToSelectDrawingArea);

Q_SIGNALS:
	void drawingAreaSelected(DrawingArea* selectedDrawingArea);

	/////////////////////////////
};
