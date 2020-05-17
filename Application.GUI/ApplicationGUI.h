#pragma once
#include <utility> 
#include <QtWidgets/QWidget>
#include <qevent.h>
#include "ui_ApplicationGUI.h"
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QFileDialog>
#include <QCheckBox>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QFrame>
#include <string>
#include "../Algorithms/Polygon.h"
#include "..\Algorithms\PolygonData.h"
#include "..\Algorithms\PolygonalMesh.h"
#include "..\Application.GUI\DrawingArea.h"
#include <qspinbox.h>
#include <fstream>
#include <streambuf>
#include <chrono>

namespace a = Algorithms;

#define SLD_SCALE 10.0

class ApplicationGUI : public QWidget
{
	Q_OBJECT

public:
	ApplicationGUI(QWidget* parent = Q_NULLPTR);

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void sliderRangeChanged();
	void areaOfPartRangeChanged(int min, int max);
	void calculatePolygonProperties();

	void btn_drawPoly(bool checked);
	void btn_doAlgo(bool checked);
	void saveResults();
	void saveMainStepsResults(QString folder);
	void savePartitionIntoConvexPartsResults(QString folder);
	void saveConvexPartPartitionsResults(QString folder);

	void saveMeshAsText(DrawingArea* drawingAreaOfMesh);
	void uploadMeshFromText();

private:
	Ui::ApplicationGUIClass ui;

	QLabel* mouseCoordinates;

	std::string activeGroupBox;

	a::Polygon* polygon;
	double polygonArea;
	std::pair<a::Point*, a::Point*> anglePoints;

	void initializeControls();
	void setActiveGroupBox(std::string grb_name, bool isNext);

	void addPointToPolygon(double x, double y, bool isLogicCoords = false);
	void addPointToAngle(int x, int y);
	bool isInMeshAngleDrawingAreaBounds(int x, int y);
	void mouseMoveEvent(QMouseEvent* event);

	PolygonData* convertPolygonToPolygonData();

	void convertToSplittedMesh();

	void drawPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces, bool isNeedToClean = true);
	void drawColoredPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces, bool isNeedToClean = true, bool isColored = true);
	void drawPolygonData(DrawingArea* drawingArea, int radiusOfPoints, PolygonData data, bool isNeedToClean = true);

	void setSliderLabelsPosition();

	std::string polygonDrawingAreaMesh;
	std::string test1;
	std::string test2;
	std::string test3;
	std::string test4;

	std::vector<std::pair<Mesh*, ConvexPartitionCharacteristics*>> convexPartitionsMeshes;
	int currentConvexPartitionsMesh;
	bool tryDrawNextConvexPartitionMesh();
	bool tryDrawPrevConvexPartitionMesh();
	bool tryDrawCurPartConvexPartitionMesh();

	std::vector<std::tuple<PolygonData, PolygonData, PolygonData, PolygonData>> partPartitions;
	int currentPartPartition;
	bool tryDrawNextPartPartition();
	bool tryDrawPrevPartPartition();
	bool tryDrawCurPartPartition();
	void setPartPartitionControlsVisibility(bool isNotDividedPart);

	void clearAllAlgoDrawingAreas();
	std::vector<int> getConcavePoints();

	Mesh* optimal;
	ConvexPartitionCharacteristics* optimalCharacteristics;
	void test(Rotation r);

	void setWidgetToScreenShot(QWidget* activeBottomWidget);
	void setWidgetToOriginalState();
	int progressBarStep;
	int progressBarStepFreq;

	std::tuple<bool, bool, bool> bottomGroupBoxesVisibility;
	std::tuple<int, int> currentPartitionsValues;

Q_SIGNALS:
	void drawingAreaSelected(DrawingArea* selectedDrawingArea);
};
