#include "ApplicationGUI.h"

ApplicationGUI::ApplicationGUI(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initializeControls();
	polygonDrawingArea->clearDrawArea();
	setActiveGroupBox("polygonProperties", false);

	polygon = nullptr;
	anglePoints = std::make_pair(nullptr, nullptr);

	setMouseTracking(true);
	ui.polygonDrawingArea->setMouseTracking(true, mouseCoordinates);
}

void ApplicationGUI::initializeControls()
{
	///////////////////////////////
	drawingArea_test = this->findChild<DrawingArea*>("drawingArea_test");
	btn_test = this->findChild<QPushButton*>("btn_test");
	connect(btn_test, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ApplicationGUI::createDefaultPolygonMesh);
	////////////////////////////////
	mouseCoordinates = new QLabel(this);
	mouseCoordinates->setStyleSheet("QLabel { background-color : white;}");

	mouseCoordinates->setGeometry(5, 5, 50, 10);

	
	polygonDrawingArea = this->findChild<DrawingArea*>("polygonDrawingArea");
	polygonDrawingArea->setGridVisibility(true);

	meshAngleDrawingArea = this->findChild<DrawingArea*>("meshAngleDrawingArea");
	meshAngleDrawingArea->setGridVisibility(false);

	

#pragma region Polygon properties
	spin_numOfSides = this->findChild<QSpinBox*>("spin_numOfSides");
	chb_isConvex = this->findChild<QCheckBox*>("chb_isConvex");
	lbl_notConvex = this->findChild<QLabel*>("lbl_notConvex");
	btn_reset = this->findChild<QPushButton*>("btn_reset");
	btn_apply = this->findChild<QPushButton*>("btn_apply");

	connect(ui.btn_apply, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ApplicationGUI::btn_apply_clicked);
	connect(ui.btn_reset, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ApplicationGUI::btn_reset_clicked);
#pragma endregion

#pragma region Area properties
	lbl_polygonArea = this->findChild<QLabel*>("lbl_polygonArea");
	lbl_areaOfPart = this->findChild<QLabel*>("lbl_areaOfPart");
	spin_numOfParts = this->findChild<QSpinBox*>("spin_numOfParts");
	btn_apply_area = this->findChild<QPushButton*>("btn_apply_area");
	btn_reset_area = this->findChild<QPushButton*>("btn_reset_area");

	connect(ui.btn_apply_area, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ApplicationGUI::btn_apply_area_clicked);
	connect(ui.btn_reset_area, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ApplicationGUI::btn_reset_area_clicked);
#pragma endregion

#pragma region Mesh properties
	spin_meshAngle = this->findChild<QDoubleSpinBox*>("spin_meshAngle");
	btn_apply_angle = this->findChild<QPushButton*>("btn_apply_angle");
	btn_reset_angle = this->findChild<QPushButton*>("btn_reset_angle");

	connect(btn_apply_angle, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ApplicationGUI::btn_apply_angle_clicked);
	connect(btn_reset_angle, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked), this, &ApplicationGUI::btn_reset_angle_clicked);
#pragma endregion
}

void ApplicationGUI::setDefaultPolygonProperties()
{
	/*lbl_notConvex->setVisible(false);
	chb_isConvex->setChecked(true);
	polygon = nullptr;
	clearDrawArea();*/
}

void ApplicationGUI::setDefaultAreaProperties()
{
	/*lbl_areaOfPart->setText("");
	
	if (polygon != nullptr && polygon->getNumOfSides() == polygon->getNumOfPoints()) {
		polygonArea = polygon->getSquare();
		lbl_polygonArea->setText(QString::number(polygonArea));
	}
	else {
		lbl_polygonArea->setText("");
		polygonArea = 0;
	}*/
}

void ApplicationGUI::setActiveGroupBox(std::string grb_name, bool isNext)
{
	if (grb_name == "polygonProperties") {
		if (isNext) {
			activeGroupBox = "areaProperties";

			btn_reset->setEnabled(true);
			btn_apply->setEnabled(false);
			spin_numOfSides->setEnabled(false);
			chb_isConvex->setEnabled(false);
			lbl_notConvex->setVisible(false);

			lbl_polygonArea->setText("");
			lbl_areaOfPart->setText("");
			spin_numOfParts->setEnabled(false);
			btn_apply_area->setEnabled(false);
			btn_reset_area->setEnabled(false);

			spin_meshAngle->setEnabled(false);
			btn_apply_angle->setEnabled(false);
			btn_reset_angle->setEnabled(false);

			polygon = new a::Polygon(spin_numOfSides->value());
			polygonArea = 0;
			anglePoints = std::make_pair(nullptr, nullptr);
		}
		else {
			activeGroupBox = "polygonProperties";

			btn_reset->setEnabled(false);
			btn_apply->setEnabled(true);
			spin_numOfSides->setEnabled(true);
			chb_isConvex->setEnabled(true);
			lbl_notConvex->setVisible(false);

			lbl_polygonArea->setText("");
			lbl_areaOfPart->setText("");
			spin_numOfParts->setEnabled(false);
			btn_apply_area->setEnabled(false);
			btn_reset_area->setEnabled(false);

			spin_meshAngle->setEnabled(false);
			btn_apply_angle->setEnabled(false);
			btn_reset_angle->setEnabled(false);
			meshAngleDrawingArea->clearDrawArea();

			chb_isConvex->setChecked(true);
			polygon = nullptr;
			polygonDrawingArea->clearDrawArea();
			anglePoints = std::make_pair(nullptr, nullptr);
		}
		
	}
	else if (grb_name == "areaProperties") {
		if (isNext) {
			activeGroupBox = "meshProperties";

			btn_reset->setEnabled(true);
			btn_apply->setEnabled(false);
			spin_numOfSides->setEnabled(false);
			chb_isConvex->setEnabled(false);
			lbl_notConvex->setVisible(false);

			lbl_areaOfPart->setText(QString::number(polygonArea/spin_numOfParts->value()));
			spin_numOfParts->setEnabled(false);
			btn_apply_area->setEnabled(false);
			btn_reset_area->setEnabled(true);

			spin_meshAngle->setEnabled(true);
			btn_apply_angle->setEnabled(true);
			btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
		}
		else {
			activeGroupBox = "areaProperties";

			btn_reset->setEnabled(true);
			btn_apply->setEnabled(false);
			spin_numOfSides->setEnabled(false);
			chb_isConvex->setEnabled(false);
			lbl_notConvex->setVisible(false);

			lbl_polygonArea->setText(QString::number(polygonArea));
			lbl_areaOfPart->setText("");
			spin_numOfParts->setEnabled(true);
			btn_apply_area->setEnabled(true);
			btn_reset_area->setEnabled(false);

			spin_meshAngle->setEnabled(false);
			btn_apply_angle->setEnabled(false);
			btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
			meshAngleDrawingArea->clearDrawArea();
		}
	}
	else if (grb_name == "meshProperties") {
		if (isNext) {
			activeGroupBox = "";

			btn_reset->setEnabled(true);
			btn_apply->setEnabled(false);
			spin_numOfSides->setEnabled(false);
			chb_isConvex->setEnabled(false);
			lbl_notConvex->setVisible(false);

			lbl_areaOfPart->setText(QString::number(polygonArea / spin_numOfParts->value()));
			spin_numOfParts->setEnabled(false);
			btn_apply_area->setEnabled(false);
			btn_reset_area->setEnabled(true);

			spin_meshAngle->setEnabled(false);
			btn_apply_angle->setEnabled(false);
			btn_reset_angle->setEnabled(true);
		}
		else {
			activeGroupBox = "meshProperties";

			btn_reset->setEnabled(true);
			btn_apply->setEnabled(false);
			spin_numOfSides->setEnabled(false);
			chb_isConvex->setEnabled(false);
			lbl_notConvex->setVisible(false);

			lbl_polygonArea->setText(QString::number(polygonArea));
			lbl_areaOfPart->setText(QString::number(polygonArea / spin_numOfParts->value()));
			spin_numOfParts->setEnabled(false);
			btn_apply_area->setEnabled(false);
			btn_reset_area->setEnabled(true);

			spin_meshAngle->setEnabled(true);
			btn_apply_angle->setEnabled(true);
			btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
			meshAngleDrawingArea->clearDrawArea();
		}
	}
}

void ApplicationGUI::setPolygonPropertiesVisibility(bool isEnabled)
{
	/*btn_reset->setEnabled(!isEnabled);
	btn_apply->setEnabled(isEnabled);

	spin_numOfSides->setEnabled(isEnabled);
	chb_isConvex->setEnabled(isEnabled);

	if (isEnabled) {
		setDefaultPolygonProperties();
	}*/
}

void ApplicationGUI::setAreaPropertiesVisibility(bool isEnabled)
{
	/*btn_reset_area->setEnabled(!isEnabled);
	btn_apply_area->setEnabled(isEnabled);

	spin_numOfParts->setEnabled(isEnabled);

	if (isEnabled) {
		setDefaultAreaProperties();
	}
	else {
		calculateAreaProperties();
	}*/
}

void ApplicationGUI::addPointToPolygon(int x, int y)
{
	auto logicPoint = polygonDrawingArea->PixelToLogicCoords(QPoint(x, y), false);
	auto pixelPoint = polygonDrawingArea->LogicToPixelCoords(logicPoint, false);

	if (polygon != nullptr && polygon->tryAddPoint({logicPoint.x(), logicPoint.y() })) {
		polygonDrawingArea->drawEllipse(QPoint(x, y), 10, false, Qt::black);

		if (polygon->getNumOfPoints() > 1)
		{
			auto prev = polygon->getPreviousPoint();
			polygonDrawingArea->drawLine(polygonDrawingArea->LogicToPixelCoords(QPointF(prev.x, prev.y), false), QPoint(x, y),  false, Qt::black, 1);
		}

		if (polygon->getNumOfSides() == polygon->getNumOfPoints()) {
			auto firstPoint = polygon->getPointAt(0);
			polygonDrawingArea->drawLine(polygonDrawingArea->LogicToPixelCoords(QPointF(firstPoint.x, firstPoint.y), false), QPoint(x, y),  false, Qt::black, 1);

			calculatePolygonProperties();
		}
	}
}

void ApplicationGUI::addPointToAngle(int x, int y)
{
	if (anglePoints.first == nullptr) {
		anglePoints.first = new a::Point{(double) x, (double)y };
		meshAngleDrawingArea->drawEllipse(QPoint(x , y ), 4, false);
	}
	else if (anglePoints.second == nullptr) {
		anglePoints.second = new a::Point{ (double)x, (double)y };
		meshAngleDrawingArea->drawEllipse(QPoint(x, y), 4, false);

		meshAngleDrawingArea->drawLine(QPoint(anglePoints.first->x, anglePoints.first->y), 
			QPoint(anglePoints.second->x, anglePoints.second->y), false);

		/*double k = (double)(anglePoints.first->y - anglePoints.second->y) / (anglePoints.first->x - anglePoints.second->x);
		double atan = std::atan(k);
		double value =atan * 180 / 3.1415;

		if (k < 0) {
			value = 270 - value;
		}
		else if (k > 0) {
			value = 90 - value;
		}

		spin_meshAngle->setValue(value);*/

		setActiveGroupBox("meshProperties", true);
	}
}

bool ApplicationGUI::isInMeshAngleDrawingAreaBounds(int x, int y)
{
	
	return activeGroupBox == "meshProperties" && meshAngleDrawingArea;
}

void ApplicationGUI::mouseMoveEvent(QMouseEvent* event)
{
	mouseCoordinates->setVisible(false);
}

void ApplicationGUI::createDefaultPolygonMesh()
{
	polygonDrawingArea->clearDrawArea();
	vectorD x(8);
	vectorD y(8);

	vectorI faces(5);
	vectorI edges(15);

	x << 10, 30, 20, 10, 20, 30, 40, 40;
	y << 10, 10, 20, 30, 40, 30, 30, 20;
	edges << 0, 1, 2, 3, 1, 2, 5, 7, 2, 3, 4, 5, 5, 6, 7;
	faces << 0, 4, 8, 12, 15;

	/*double degree = 90 * 3.14 / 180;
	double c = cos(degree);
	double s = sin(degree);
	double x0 = 25;
	double y0 = 25;


	for (int i = 0; i < x.size(); ++i) {
		double oldX = x(i);
		double oldY = y(i);
		x(i) = (x0 - oldX) * c - (y0 - oldY) * s;
		y(i) = (y0 - oldX) * s + (y0 - oldY) * c;
	}*/


	Rotation rotation = Rotation(90, Right);
	rotation.tryRotateFigure(x, y);

	drawPolygonMesh(x, y, edges, faces);
}

void ApplicationGUI::drawPolygonMesh(vectorD x, vectorD y, vectorI edges, vectorI faces)
{
	for (int i = 0; i < x.size(); ++i) {
		auto pixel = polygonDrawingArea->LogicToPixelCoords(QPointF(x(i), y(i)), false);
		polygonDrawingArea->drawEllipse(pixel, 10, false, Qt::black);
	}

	for (int i = 1; i < faces.size(); ++i) {
		for (int j = faces(i - 1); j < faces(i); ++j) {
			if (j == faces(i) - 1) {
				polygonDrawingArea->drawLine(
					polygonDrawingArea->LogicToPixelCoords(QPointF(x(edges(j)), y(edges(j))), false),
					polygonDrawingArea->LogicToPixelCoords(QPointF(x(edges(faces(i-1))), y(edges(faces(i-1)))), false),
					false, Qt::black, 1);
			}
			else {
				polygonDrawingArea->drawLine(
					polygonDrawingArea->LogicToPixelCoords(QPointF(x(edges(j)), y(edges(j))), false),
					polygonDrawingArea->LogicToPixelCoords(QPointF(x(edges(j+1)), y(edges(j+1))), false),
					false, Qt::black, 1);
			}
		}
	}
		
		//	polygonDrawingArea->drawLine(polygonDrawingArea->LogicToPixelCoords(QPointF(firstPixelPoint.x, firstPixelPoint.y), false), QPoint(x, y), false, Qt::black, 1);

	
}

void ApplicationGUI::btn_apply_clicked(bool checked)
{
	setActiveGroupBox("polygonProperties", true);
}

void ApplicationGUI::btn_reset_clicked(bool checked)
{
	setActiveGroupBox("polygonProperties", false);
}

void ApplicationGUI::btn_apply_area_clicked(bool checked)
{
	setActiveGroupBox("areaProperties", true);
}

void ApplicationGUI::btn_reset_area_clicked(bool checked)
{
	setActiveGroupBox("areaProperties", false);
}

void ApplicationGUI::btn_apply_angle_clicked(bool checked)
{
	setActiveGroupBox("meshProperties", true);
}

void ApplicationGUI::btn_reset_angle_clicked(bool checked)
{
	setActiveGroupBox("meshProperties", false);
}

void ApplicationGUI::calculatePolygonProperties()
{
	if (chb_isConvex->isChecked()) {
		if (polygon->isConvex()) {
			//setDefaultAreaProperties();
			polygonArea = polygon->getSquare();
			setActiveGroupBox("areaProperties", false);
		}
		else {

			lbl_notConvex->setVisible(true);
		}
	}
}

void ApplicationGUI::calculateAreaProperties()
{
	lbl_areaOfPart->setText(QString::number(polygonArea / spin_numOfParts->value()));
}

void ApplicationGUI::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		
		auto point = event->pos();
		
		if (polygonDrawingArea->isPointInBounds(point.x(), point.y())) {
			addPointToPolygon(point.x(), point.y());
		}
		else if (isInMeshAngleDrawingAreaBounds(point.x(), point.y())) {
			addPointToAngle(point.x(), point.y());
		}
		else if (drawingArea_test->isPointInBounds(point.x(), point.y())) {
			drawingArea_test->drawEllipse(point, 10, false);
		}
	}
}