#pragma region includes, usings and constants

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
#include <QSpinBox>
#include <string>
#include "..\Algorithms\PolygonalMesh.h"
#include "..\Algorithms\Intersection.h"
#include "..\Application.GUI\DrawingArea.h"
#include <fstream>
#include <streambuf>

using namespace algo;

#define SLD_SCALE 10.0

#pragma endregion

class ApplicationGUI : public QWidget
{
	Q_OBJECT

private:
	algo::Polygon* polygon;

	std::pair<Point*, Point*> anglePoints;

	double polygonArea;
	double minPartArea;
	double maxPartArea;
	bool isSliderAreaInPercents;

	Ui::ApplicationGUIClass ui;
	QLabel* mouseCoordinates;
	int progressBarStep;
	int progressBarStepFreq;
	std::tuple<bool, bool, bool> bottomGroupBoxesVisibility;
	std::tuple<int, int> currentPartitionsValues;

	std::string activeGroupBox;

	Mesh* optimal;
	ConvexPartitionCharacteristics* optimalCharacteristics;

	int currentConvexPartitionsMesh;
	std::vector<std::pair<Mesh*, ConvexPartitionCharacteristics*>> convexPartitionsMeshes;
	
	int currentPartPartition;
	std::vector<std::tuple<PolygonData, PolygonData, PolygonData, PolygonData>> partPartitions;
	
public:
	ApplicationGUI(QWidget* parent = Q_NULLPTR);

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event);

	void sliderRangeChanged();
	void setSliderLabelsPosition();

private:
	void initializeControls();
	void setActiveGroupBox(std::string grb_name, bool isNext);

	void addPointToPolygon(double x, double y, bool isLogicCoords = false);
	void addPointToAngle(int x, int y);
	bool isInMeshAngleDrawingAreaBounds(int x, int y);
	PolygonData* convertPolygonToPolygonData();
	void uploadMeshFromText();

	void areaOfPartRangeChanged(int min, int max);
	void calculatePolygonProperties();

	void drawTestPolygon();
	void drawPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces, bool isNeedToClean = true);
	void drawColoredPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces, bool isNeedToClean = true, bool isColored = true);
	void drawPolygonData(DrawingArea* drawingArea, int radiusOfPoints, PolygonData data, bool isNeedToClean = true);
	void clearAllAlgoDrawingAreas();

	void tryExecuteAlgorithm();
	void splitConvexParts(Rotation r);
	std::vector<int> getConcavePoints();

	void saveResults();
	void saveMainStepsResults(QString folder);
	void savePartitionIntoConvexPartsResults(QString folder);
	void saveConvexPartPartitionsResults(QString folder);

	bool tryDrawNextConvexPartitionMesh();
	bool tryDrawPrevConvexPartitionMesh();
	bool tryDrawCurPartConvexPartitionMesh();

	bool tryDrawNextPartPartition();
	bool tryDrawPrevPartPartition();
	bool tryDrawCurPartPartition();
	void setPartPartitionControlsVisibility(bool isNotDividedPart);

	void setWidgetToScreenShot(QWidget* activeBottomWidget);
	void setWidgetToOriginalStateAfterScreenShot();
};