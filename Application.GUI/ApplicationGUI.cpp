#include "ApplicationGUI.h"

ApplicationGUI::ApplicationGUI(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	initializeControls();
	ui.polygonDrawingArea->clearDrawArea();
	setActiveGroupBox("polygonProperties", false);

	polygon = nullptr;
	anglePoints = std::make_pair(nullptr, nullptr);

	setMouseTracking(true);
	ui.polygonDrawingArea->setMouseTracking(true, mouseCoordinates);
	//ui.meshAngleDrawingArea->setMouseTracking(false);
}

void ApplicationGUI::initializeControls()
{
	////////////////////////////////

	//3,3
	ui.drawingArea_test->setScale(3, 3);
	ui.drawingArea_test->setGridVisibility(true);
	ui.drawingArea_test->clearDrawArea();

	ui.drawingArea_test_2->setScale(3, 3);
	ui.drawingArea_test_2->setGridVisibility(true);
	ui.drawingArea_test_2->clearDrawArea();

	ui.drawingArea_test_3->setScale(3, 3);
	ui.drawingArea_test_3->setGridVisibility(true);
	ui.drawingArea_test_3->clearDrawArea();

	ui.drawingArea_test_4->setScale(3, 3);
	ui.drawingArea_test_4->setGridVisibility(true);
	ui.drawingArea_test_4->clearDrawArea();

	connect(ui.btn_saveMeshAsText, &QPushButton::clicked, this, &ApplicationGUI::btn_saveMeshAsText);
	connect(ui.btn_uploadMeshFromText, &QPushButton::clicked, this, &ApplicationGUI::btn_uploadMeshFromText);
	connect(ui.btn_saveImages, &QPushButton::clicked, this, &ApplicationGUI::btn_saveAsImage);
	connect(ui.btn_drawPoly, &QPushButton::clicked, this, &ApplicationGUI::btn_drawPoly);
	connect(ui.btn_doAlgo, &QPushButton::clicked, this, &ApplicationGUI::btn_doAlgo);
	////////////////////////////////
	mouseCoordinates = new QLabel(this);
	mouseCoordinates->setStyleSheet("QLabel { background-color : white;}");

	mouseCoordinates->setGeometry(5, 5, 50, 10);

#pragma region Polygon properties
	ui.polygonDrawingArea->setGridVisibility(true);
	connect(ui.btn_apply, &QPushButton::clicked, this, &ApplicationGUI::btn_apply_clicked);
	connect(ui.btn_reset, &QPushButton::clicked, this, &ApplicationGUI::btn_reset_clicked);
#pragma endregion

#pragma region Area properties
	connect(ui.sld_areaOfPart, &RangeSlider::minimumPositionChanged, this,
		[=](int min) {
			ui.sld_minPosLabel->setText(QString::number(min / SLD_SCALE));
			ui.lbl_minAreaOfPart->setText(QString::number(min / SLD_SCALE));
			setSliderLabelsPosition();
		});

	connect(ui.sld_areaOfPart, &RangeSlider::maximumPositionChanged, this,
		[=](int max) {
			ui.sld_maxPosLabel->setText(QString::number(max / SLD_SCALE));
			ui.lbl_maxAreaOfPart->setText(QString::number(max / SLD_SCALE));
			setSliderLabelsPosition();
		});

	connect(ui.sld_areaOfPart, &RangeSlider::sliderRealesed, this,
		[=]() {
			btn_doAlgo(true);
		});
#pragma endregion

#pragma region Mesh properties
	ui.meshAngleDrawingArea->setGridVisibility(false);
	connect(ui.btn_apply_angle, &QPushButton::clicked, this, &ApplicationGUI::btn_apply_angle_clicked);
	connect(ui.btn_reset_angle, &QPushButton::clicked, this, &ApplicationGUI::btn_reset_angle_clicked);
	ui.meshAngleDrawingArea->setX();
	ui.meshAngleDrawingArea->setY();
#pragma endregion

	isHintForSelectDrawingAreaShowned = false;
}

void ApplicationGUI::setActiveGroupBox(std::string grb_name, bool isNext)
{
	if (grb_name == "polygonProperties") {
		if (isNext) {
			activeGroupBox = "areaProperties";

			ui.btn_reset->setEnabled(true);
			ui.btn_apply->setEnabled(false);
			ui.spin_numOfSides->setEnabled(false);

			ui.lbl_polygonArea->setText("");
			ui.sld_areaOfPart->setEnabled(false);
			ui.sld_areaOfPart->setMaximum(0);
			sliderRangeChanged();

			ui.spin_meshAngle->setEnabled(false);
			ui.btn_apply_angle->setEnabled(false);
			ui.btn_reset_angle->setEnabled(false);

			polygon = new a::Polygon(ui.spin_numOfSides->value());
			polygonArea = 0;
			anglePoints = std::make_pair(nullptr, nullptr);

			isModeToSelectDrawingArea = false;
		}
		else {
			activeGroupBox = "polygonProperties";

			ui.btn_reset->setEnabled(false);
			ui.btn_apply->setEnabled(true);
			ui.spin_numOfSides->setEnabled(true);

			ui.lbl_polygonArea->setText("");
			ui.sld_areaOfPart->setEnabled(false);
			ui.sld_areaOfPart->setMaximum(0);
			sliderRangeChanged();

			ui.spin_meshAngle->setEnabled(false);
			ui.btn_apply_angle->setEnabled(false);
			ui.btn_reset_angle->setEnabled(false);
			ui.meshAngleDrawingArea->clearDrawArea();

			polygon = nullptr;
			ui.polygonDrawingArea->clearDrawArea();
			anglePoints = std::make_pair(nullptr, nullptr);

			isModeToSelectDrawingArea = false;
		}
	}
	else if (grb_name == "areaProperties") {
		if (isNext) {
			activeGroupBox = "meshProperties";

			ui.btn_reset->setEnabled(true);
			ui.btn_apply->setEnabled(false);
			ui.spin_numOfSides->setEnabled(false);

			ui.lbl_polygonArea->setText(QString::number(polygonArea));
			ui.sld_areaOfPart->setMaximum(polygonArea * 10);
			ui.sld_areaOfPart->setEnabled(true);
			sliderRangeChanged();

			ui.spin_meshAngle->setEnabled(true);
			ui.btn_apply_angle->setEnabled(true);
			ui.btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);

			isModeToSelectDrawingArea = false;
		}
	}
	else if (grb_name == "meshProperties") {
		if (isNext) {
			activeGroupBox = "";

			ui.btn_reset->setEnabled(true);
			ui.btn_apply->setEnabled(false);
			ui.spin_numOfSides->setEnabled(false);

			ui.spin_meshAngle->setEnabled(false);
			ui.btn_apply_angle->setEnabled(false);
			ui.btn_reset_angle->setEnabled(true);

			isModeToSelectDrawingArea = false;
		}
		else {
			activeGroupBox = "meshProperties";

			ui.btn_reset->setEnabled(true);
			ui.btn_apply->setEnabled(false);
			ui.spin_numOfSides->setEnabled(false);

			ui.spin_meshAngle->setEnabled(true);
			ui.btn_apply_angle->setEnabled(true);
			ui.btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
			ui.meshAngleDrawingArea->clearDrawArea();

			isModeToSelectDrawingArea = false;
		}
	}
}

void ApplicationGUI::addPointToPolygon(int x, int y)
{
	auto logicPoint = ui.polygonDrawingArea->PixelToLogicCoords(QPoint(x, y), false);

	if (polygon != nullptr && polygon->tryAddPoint({ logicPoint.x(), logicPoint.y() })) {
		ui.polygonDrawingArea->drawEllipse(QPoint(x, y), 5, false, Qt::black);

		if (polygon->getNumOfPoints() > 1)
		{
			auto prev = polygon->getPreviousPoint();
			ui.polygonDrawingArea->drawLine(ui.polygonDrawingArea->LogicToPixelCoords(QPointF(prev.x, prev.y), false), QPoint(x, y), false, Qt::black, 1);
		}

		if (polygon->getNumOfSides() == polygon->getNumOfPoints()) {
			auto firstPoint = polygon->getPointAt(0);
			ui.polygonDrawingArea->drawLine(ui.polygonDrawingArea->LogicToPixelCoords(QPointF(firstPoint.x, firstPoint.y), false), QPoint(x, y), false, Qt::black, 1);

			calculatePolygonProperties();
		}
	}
}

void ApplicationGUI::addPointToAngle(int x, int y)
{
	if (anglePoints.first == nullptr) {
		anglePoints.first = new a::Point{ (double)x, (double)y };
		ui.meshAngleDrawingArea->drawEllipse(QPoint(x, y), 4, false);
	}
	else if (anglePoints.second == nullptr) {
		if ((double)x < anglePoints.first->x) {
			anglePoints.second = new a::Point{ anglePoints.first->x, anglePoints.first->y };
			anglePoints.first = new a::Point{ (double)x, (double)y };
		}
		else
		{
			anglePoints.second = new a::Point{ (double)x, (double)y };
		}

		ui.spin_meshAngle->setValue(Rotation::findAngleOfSegmentInDegrees(anglePoints.first->x, anglePoints.first->y, anglePoints.second->x, anglePoints.second->y));

		auto xPos = std::abs(anglePoints.first->x - anglePoints.second->x) / 2 + min(anglePoints.first->x, anglePoints.second->x);

		ui.meshAngleDrawingArea->drawLine(QPoint(xPos, ui.meshAngleDrawingArea->y()), QPoint(xPos, ui.meshAngleDrawingArea->height() + ui.meshAngleDrawingArea->y()), false, Qt::red);

		QPoint center = QPoint(xPos, std::abs(anglePoints.first->y - anglePoints.second->y) / 2 + min(anglePoints.first->y, anglePoints.second->y)); // intersection of lines
		QPoint left = QPoint(anglePoints.first->x, anglePoints.first->y); // end point of horizontal line
		QPoint top = QPoint(xPos, ui.meshAngleDrawingArea->y()); // end point of other line
		ui.meshAngleDrawingArea->drawArc(ui.spin_meshAngle->value(), left, center, top, 10, false, Qt::red, Qt::red);

		ui.meshAngleDrawingArea->drawEllipse(QPoint(x, y), 4, false);
		ui.meshAngleDrawingArea->drawLine(QPoint(anglePoints.first->x, anglePoints.first->y),
			QPoint(anglePoints.second->x, anglePoints.second->y), false);

		setActiveGroupBox("meshProperties", true);

		btn_doAlgo(true);
		//convertToMesh();
	}
}

bool ApplicationGUI::isInMeshAngleDrawingAreaBounds(int x, int y)
{
	return activeGroupBox == "meshProperties" && ui.meshAngleDrawingArea;
}

void ApplicationGUI::mouseMoveEvent(QMouseEvent* event)
{
	mouseCoordinates->setVisible(false);
}

void ApplicationGUI::convertToSplittedMesh()
{
	vectorD* x = new vectorD(polygon->getNumOfPoints());
	vectorD* y = new vectorD(polygon->getNumOfPoints());
	vectorI* edges = new vectorI(polygon->getNumOfPoints());

	if (polygon->isLeftTraversal) {
		for (int i = 0; i < polygon->getNumOfPoints(); ++i) {
			auto point = polygon->getPointAt(i);
			(*x)[i] = point.x;
			(*y)[i] = point.y;
			(*edges)[i] = i;
		}
	}
	else {
		for (int i = 0; i < polygon->getNumOfPoints(); ++i) {
			auto point = polygon->getPointAt(polygon->getNumOfPoints() - 1 - i);
			(*x)[i] = point.x;
			(*y)[i] = point.y;
			(*edges)[i] = polygon->getNumOfPoints() - 1 - i;
		}
	}

	Rotation r = Rotation(ui.spin_meshAngle->value(), RotationDirection::Right);
	r.tryRotateFigure(x, y);

	PolygonData rotatedData = PolygonData();
	rotatedData.vertex_x = *x;
	rotatedData.vertex_y = *y;
	rotatedData.tryAddFace(edges->size(), *edges);

	drawPolygonMesh(ui.drawingArea_test, 2, rotatedData.vertex_x, rotatedData.vertex_y, rotatedData.edges, rotatedData.faces);

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(rotatedData);
	m.F[0]->area = polygonArea;

	m.splitByVerticalGrid();

	auto SplittedByGridData = m.convertToPolygonData();
	drawPolygonMesh(ui.drawingArea_test_2, 2, SplittedByGridData.vertex_x, SplittedByGridData.vertex_y, SplittedByGridData.edges, SplittedByGridData.faces);

	//auto splitedEdges = m.splitFaces(polygonArea / ui.spin_numOfParts->value());
	auto splitedEdges = nullptr;

	auto verticalSplittedData = m.convertToPolygonData();
	drawPolygonMesh(ui.drawingArea_test_3, 2, verticalSplittedData.vertex_x, verticalSplittedData.vertex_y, verticalSplittedData.edges, verticalSplittedData.faces);

	auto perims = m.getInternalHorizontalPerimeters();
	auto optimal = m.getOptimalCombinationForInternalPerimeter(perims);
	m.splitMeshByMask(optimal.second);

	auto horisontalSplittedData = m.convertToPolygonData();
	drawPolygonMesh(ui.drawingArea_test_4, 2, horisontalSplittedData.vertex_x, horisontalSplittedData.vertex_y, horisontalSplittedData.edges, horisontalSplittedData.faces);

	auto result = m.convertToPolygonData();
	r.tryRrotateTheFigureBack(&result.vertex_x, &result.vertex_y);
	drawPolygonMesh(ui.polygonDrawingArea, 5, result.vertex_x, result.vertex_y, result.edges, result.faces);
}

void ApplicationGUI::drawPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces)
{
	drawingArea->clearDrawArea();

	for (int i = 0; i < x.size(); ++i) {
		auto pixel = drawingArea->LogicToPixelCoords(QPointF(x[i], y[i]), false);
		drawingArea->drawEllipse(pixel, radiusOfPoints, false, Qt::black);
	}

	for (int i = 1; i < faces.size(); ++i) {
		for (int j = faces[i - 1]; j < faces[i]; ++j) {
			if (j == faces[i] - 1) {
				drawingArea->drawLine(
					drawingArea->LogicToPixelCoords(QPointF(x[edges[j]], y[edges[j]]), false),
					drawingArea->LogicToPixelCoords(QPointF(x[edges[faces[i - 1]]], y[edges[faces[i - 1]]]), false),
					false, Qt::black, 1);
			}
			else {
				drawingArea->drawLine(
					drawingArea->LogicToPixelCoords(QPointF(x[edges[j]], y[edges[j]]), false),
					drawingArea->LogicToPixelCoords(QPointF(x[edges[j + 1]], y[edges[j + 1]]), false),
					false, Qt::black, 1);
			}
		}
	}

	//	ui.polygonDrawingArea->drawLine(ui.polygonDrawingArea->LogicToPixelCoords(QPointF(firstPixelPoint.x, firstPixelPoint.y), false), QPoint(x, y), false, Qt::black, 1);
}

void ApplicationGUI::setSliderLabelsPosition()
{
	auto w = ui.sld_areaOfPart->width();
	auto lbl_w = 40;
	auto lbl_h = 16;
	auto offset = 10;

	ui.sld_minPosLabel->setGeometry(offset + ((double)ui.sld_areaOfPart->minimumPosition() / ui.sld_areaOfPart->maximum() * (w- lbl_w)), 40, lbl_w, lbl_h);

	QRect d = ui.sld_maxPosLabel->geometry();
	QRect droppedRect = QRect(d.left(), 40, d.width(), d.height());

	if (droppedRect.intersects(ui.sld_minPosLabel->geometry()))
	{
		ui.sld_maxPosLabel->setGeometry(offset + ((double)ui.sld_areaOfPart->maximumPosition() / ui.sld_areaOfPart->maximum() * (w- lbl_w)), 0, lbl_w, lbl_h);
	}
	else {
		ui.sld_maxPosLabel->setGeometry(offset + ((double)ui.sld_areaOfPart->maximumPosition() / ui.sld_areaOfPart->maximum() * (w- lbl_w)), 40, lbl_w, lbl_h);
	}
}

void ApplicationGUI::btn_apply_clicked(bool checked)
{
	setActiveGroupBox("polygonProperties", true);
}

void ApplicationGUI::btn_reset_clicked(bool checked)
{
	setActiveGroupBox("polygonProperties", false);
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
	polygonArea = polygon->getSquare();
	setActiveGroupBox("areaProperties", true);
}

void ApplicationGUI::sliderRangeChanged()
{
	if (ui.sld_areaOfPart->maximum() > 0)
	{
		ui.sld_areaOfPart->setMinimum(1);
	}

	ui.sld_areaOfPart->setMinimumPosition(0);
	ui.sld_areaOfPart->setMaximumPosition(ui.sld_areaOfPart->maximum());

	areaOfPartRangeChanged(ui.sld_areaOfPart->minimumPosition(), ui.sld_areaOfPart->maximumPosition());
}

void ApplicationGUI::areaOfPartRangeChanged(int min, int max)
{
	if (min == max && min == 0) {
		ui.sld_minPosLabel->setText("");
		ui.sld_maxPosLabel->setText("");
		ui.lbl_minAreaOfPart->setText("");
		ui.lbl_maxAreaOfPart->setText("");
	}
	else {
		ui.sld_minPosLabel->setText(QString::number(min / SLD_SCALE));
		ui.sld_maxPosLabel->setText(QString::number(max / SLD_SCALE));
		ui.lbl_minAreaOfPart->setText(QString::number(min / SLD_SCALE));
		ui.lbl_maxAreaOfPart->setText(QString::number(max / SLD_SCALE));
		setSliderLabelsPosition();
	}
}

void ApplicationGUI::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {

		auto point = event->pos();
		if (isModeToSelectDrawingArea) {
			if (ui.polygonDrawingArea->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.polygonDrawingArea);
			}
			else if (ui.drawingArea_test->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drawingArea_test);
			}
			else if (ui.drawingArea_test_2->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drawingArea_test_2);
			}
			else if (ui.drawingArea_test_3->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drawingArea_test_3);
			}
			else if (ui.drawingArea_test_4->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drawingArea_test_4);
			}
		}
		else {
			if (ui.polygonDrawingArea->isPointInBounds(point.x(), point.y())) {
				addPointToPolygon(point.x(), point.y());
			}
			else if (isInMeshAngleDrawingAreaBounds(point.x(), point.y())) {
				addPointToAngle(point.x(), point.y());
			}
			else if (ui.drawingArea_test->isPointInBounds(point.x(), point.y())) {
				ui.drawingArea_test->drawEllipse(point, 10, false);
			}
		}
	}
}

void ApplicationGUI::btn_drawPoly(bool checked)
{
	polygon = new a::Polygon(10);

	QPoint p;
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 55.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 50.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 45.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 15.0, 35.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 35.0, 35.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 15.0, 25.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 15.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 15.0, 5.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 45.0, 15.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 45.0, 60.0 }, false);
	addPointToPolygon(p.x(), p.y());
}

PolygonData* ApplicationGUI::convertPolygonToPolygonData() {
	vectorD* x = new vectorD(polygon->getNumOfPoints());
	vectorD* y = new vectorD(polygon->getNumOfPoints());
	vectorI* edges = new vectorI(polygon->getNumOfPoints());

	if (polygon->isLeftTraversal) {
		for (int i = 0; i < polygon->getNumOfPoints(); ++i) {
			auto point = polygon->getPointAt(i);
			(*x)[i] = point.x;
			(*y)[i] = point.y;
			(*edges)[i] = i;
		}
	}
	else {
		for (int i = 0; i < polygon->getNumOfPoints(); ++i) {
			auto point = polygon->getPointAt(polygon->getNumOfPoints() - 1 - i);
			(*x)[i] = point.x;
			(*y)[i] = point.y;
			(*edges)[i] = polygon->getNumOfPoints() - 1 - i;
		}
	}

	PolygonData * data = new PolygonData();
	data->vertex_x = *x;
	data->vertex_y = *y;
	data->tryAddFace(edges->size(), *edges);

	return data;
}

void ApplicationGUI::btn_doAlgo(bool checked)
{
	if (anglePoints.first == nullptr || anglePoints.second == nullptr)
		return;

	auto data = convertPolygonToPolygonData();

	drawPolygonMesh(ui.drawingArea_test, 2, data->vertex_x, data->vertex_y, data->edges, data->faces);

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);
	polygonDrawingAreaMesh = m.convertToString();

	for (int i = 5; i <= 100; i += 10) {
		m.splitByVertical(i);
		m.splitHorizontal(i);
	}

	test2 = m.convertToString();

	auto SplittedByGridData = m.convertToPolygonData();
	drawPolygonMesh(ui.drawingArea_test_2, 2, SplittedByGridData.vertex_x, SplittedByGridData.vertex_y, SplittedByGridData.edges, SplittedByGridData.faces);
}

void ApplicationGUI::btn_saveMeshAsText(bool checked) {
	if (!isHintForSelectDrawingAreaShowned) {
		isHintForSelectDrawingAreaShowned = true;
		QMessageBox::information(this, "", "Select the area...");
	}
	setControlsDependsOnSelectingMode(true);
	connect(this, &ApplicationGUI::drawingAreaSelected, this, &ApplicationGUI::saveMeshAsText);
}

void ApplicationGUI::saveMeshAsText(DrawingArea* drawingAreaOfMesh) {
	disconnect(this, &ApplicationGUI::drawingAreaSelected, this, &ApplicationGUI::saveMeshAsText);
	setControlsDependsOnSelectingMode(false);

	QString file_name = QFileDialog::getSaveFileName(this, "Save Text File", QDir::homePath(), "Text files(*.txt)");
	
	if (file_name.isEmpty()) {
		QMessageBox::critical(this, "Error", "File name can not be empty");
		return;
	}

	std::string toSave = "";

	if (drawingAreaOfMesh == ui.polygonDrawingArea) {
		toSave = polygonDrawingAreaMesh;
	}
	else if (drawingAreaOfMesh == ui.drawingArea_test) {
		toSave = test1;
	}
	else if (drawingAreaOfMesh == ui.drawingArea_test_2) {
		toSave = test2;
	}
	else if (drawingAreaOfMesh == ui.drawingArea_test_3) {
		toSave = test3;
	}
	else if (drawingAreaOfMesh == ui.drawingArea_test_4) {
		toSave = test4;
	}

	if (toSave == "") {
		QMessageBox::critical(this, "Error", "Can not save mesh in file.");
		return;
	}

	std::ofstream out(file_name.toStdString());
	out << toSave;
	out.close();
}

void ApplicationGUI::btn_uploadMeshFromText(bool checked) {
	if (!isHintForSelectDrawingAreaShowned) {
		isHintForSelectDrawingAreaShowned = true;
		QMessageBox::information(this, "", "Select the area...");
	}
	setControlsDependsOnSelectingMode(true);
	connect(this, &ApplicationGUI::drawingAreaSelected, this, &ApplicationGUI::uploadMeshFromText);
}

void ApplicationGUI::uploadMeshFromText(DrawingArea* drawingAreaOfMesh) {
	disconnect(this, &ApplicationGUI::drawingAreaSelected, this, &ApplicationGUI::uploadMeshFromText);
	setControlsDependsOnSelectingMode(false);
	
	QString file_name = QFileDialog::getOpenFileName(this, "Choose PolygonalMesh source", QDir::homePath());

	std::ifstream in(file_name.toStdString());
	std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	auto mesh = Mesh::convertFromString(str);
	if (mesh == nullptr) {
		QMessageBox::critical(this, "Error", "Can not be converted into polygonal mesh");
		return;
	}

	auto data = mesh->convertToPolygonData();
	if (drawingAreaOfMesh == ui.polygonDrawingArea) {
		drawPolygonMesh(drawingAreaOfMesh, 4, data.vertex_x, data.vertex_y, data.edges, data.faces);
	}
	else  {
		drawPolygonMesh(drawingAreaOfMesh, 2, data.vertex_x, data.vertex_y, data.edges, data.faces);
	}
}

void ApplicationGUI::btn_saveAsImage(bool checked)
{
	if (!isHintForSelectDrawingAreaShowned) {
		isHintForSelectDrawingAreaShowned = true;
		QMessageBox::information(this, "", "Select the area...");
	}
	setControlsDependsOnSelectingMode(true);
	connect(this, &ApplicationGUI::drawingAreaSelected, this, &ApplicationGUI::saveDrawingAreaAsImage);
}

void ApplicationGUI::saveDrawingAreaAsImage(DrawingArea* drawingAreaOfMesh)
{
	disconnect(this, &ApplicationGUI::drawingAreaSelected, this, &ApplicationGUI::saveDrawingAreaAsImage);
	setControlsDependsOnSelectingMode(false);

	QString file_name = QFileDialog::getSaveFileName(this, "Save Image File", QDir::homePath(), "Images (*.png)");
	
	if(file_name.isEmpty())
		QMessageBox::critical(this, "Error", "File name can not be empty");
	else
		drawingAreaOfMesh->saveImage(file_name, 1);
}

void ApplicationGUI::setControlsDependsOnSelectingMode(bool isModeToSelectDrawingArea)
{
	this->isModeToSelectDrawingArea = isModeToSelectDrawingArea;

	if (isModeToSelectDrawingArea) {
		controlsStatesBeforeSelectingMode.clear();

		controlsStatesBeforeSelectingMode.push_back(ui.btn_reset->isEnabled());
		ui.btn_reset->setEnabled(false);
		controlsStatesBeforeSelectingMode.push_back(ui.btn_apply->isEnabled());
		ui.btn_apply->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.spin_numOfSides->isEnabled());
		ui.spin_numOfSides->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.spin_meshAngle->isEnabled());
		ui.spin_meshAngle->setEnabled(false);
		controlsStatesBeforeSelectingMode.push_back(ui.btn_apply_angle->isEnabled());
		ui.btn_apply_angle->setEnabled(false);
		controlsStatesBeforeSelectingMode.push_back(ui.btn_reset_angle->isEnabled());
		ui.btn_reset_angle->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.btn_saveImages->isEnabled());
		ui.btn_saveImages->setEnabled(false);
		controlsStatesBeforeSelectingMode.push_back(ui.btn_saveMeshAsText->isEnabled());
		ui.btn_saveMeshAsText->setEnabled(false);
		controlsStatesBeforeSelectingMode.push_back(ui.btn_uploadMeshFromText->isEnabled());
		ui.btn_uploadMeshFromText->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.btn_drawPoly->isEnabled());
		ui.btn_drawPoly->setEnabled(false);
		controlsStatesBeforeSelectingMode.push_back(ui.btn_doAlgo->isEnabled());
		ui.btn_doAlgo->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.sld_areaOfPart->isEnabled());
		ui.sld_areaOfPart->setEnabled(false);

		ui.polygonDrawingArea->setHoverEffectEnabled(true);
		ui.drawingArea_test->setHoverEffectEnabled(true);
		ui.drawingArea_test_2->setHoverEffectEnabled(true);
		ui.drawingArea_test_3->setHoverEffectEnabled(true);
		ui.drawingArea_test_4->setHoverEffectEnabled(true);
	}
	else {
		int cnt = 0;

		ui.btn_reset->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_apply->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.spin_numOfSides->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.spin_meshAngle->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_apply_angle->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_reset_angle->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.btn_saveImages->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_saveMeshAsText->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_uploadMeshFromText->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.btn_drawPoly->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_doAlgo->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.sld_areaOfPart->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.polygonDrawingArea->deleteAllEffects();
		ui.drawingArea_test->deleteAllEffects();
		ui.drawingArea_test_2->deleteAllEffects();
		ui.drawingArea_test_3->deleteAllEffects();
		ui.drawingArea_test_4->deleteAllEffects();
	}
}