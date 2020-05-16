#include "ApplicationGUI.h"

QColor ApplicationGUI::getNextColor() {
	return QColor(rand() % 256, rand() % 256, rand() % 256, 100);
}

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
}

void ApplicationGUI::initializeControls()
{
#pragma region Main Stages

	ui.progressBar->setVisible(false);

	////////////////////////////////
	currentConexPartitionsMeshe = -1;
	conexPartitionsMeshes = std::vector<std::pair<Mesh*,ConvexPartitionCharacteristics*>>();
	//3,3
	ui.drar_mainStages_rotated->setScale(3, 3);
	ui.drar_mainStages_rotated->setGridVisibility(true);
	ui.drar_mainStages_rotated->clearDrawArea();

	ui.drar_mainStages_concavePoints->setScale(3, 3);
	ui.drar_mainStages_concavePoints->setGridVisibility(true);
	ui.drar_mainStages_concavePoints->clearDrawArea();

	ui.drar_mainStages_convexParts->setScale(3, 3);
	ui.drar_mainStages_convexParts->setGridVisibility(true);
	ui.drar_mainStages_convexParts->clearDrawArea();

	ui.drar_mainStages_optimal->setScale(3, 3);
	ui.drar_mainStages_optimal->setGridVisibility(true);
	ui.drar_mainStages_optimal->clearDrawArea();

	connect(ui.btn_uploadMeshFromText, &QPushButton::clicked, this, &ApplicationGUI::btn_uploadMeshFromText);
	connect(ui.btn_drawPoly, &QPushButton::clicked, this, &ApplicationGUI::btn_drawPoly);
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
			if (anglePoints.first != nullptr && anglePoints.second != nullptr) {
				setActiveGroupBox("meshProperties", true);
				btn_doAlgo(true);
			}
			else {
				setActiveGroupBox("meshProperties", false);
			}
		});
#pragma endregion

#pragma region Mesh properties
	ui.meshAngleDrawingArea->setGridVisibility(false);
	connect(ui.btn_reset_angle, &QPushButton::clicked, this, &ApplicationGUI::btn_reset_angle_clicked);
	ui.meshAngleDrawingArea->setX();
	ui.meshAngleDrawingArea->setY();
#pragma endregion

	connect(ui.btn_allConcavePartitions, &QPushButton::clicked, this,
		[=](bool checked) {
			ui.convexPartitions->setVisible(true);
			ui.mainAlgoImages->setVisible(false);
			ui.lbl_numOfConvexPartitions->setText("Part " + QString::number(currentConexPartitionsMeshe + 1) + " / " + QString::number(conexPartitionsMeshes.size()));
		});

	connect(ui.btn_convexPartitionStages, &QPushButton::clicked, this,
		[=](bool checked) {
			ui.convexPartsPartition->setVisible(true);
			ui.mainAlgoImages->setVisible(false);
			ui.lbl_numOfConvexPart->setText("Part " + QString::number(currentPartPartition + 1) + " / " + QString::number(partPartitions.size()));
		});

	ui.btn_allConcavePartitions->setVisible(false);
	ui.btn_convexPartitionStages->setVisible(false);

#pragma endregion

#pragma region Convex Partitions

	ui.convexPartitions->setVisible(false);

	ui.drar_partitionToConvex_optimal->setScale(3, 3);
	ui.drar_partitionToConvex_optimal->setGridVisibility(true);
	ui.drar_partitionToConvex_optimal->clearDrawArea();

	ui.drar_partitionToConvex_current->setScale(3, 3);
	ui.drar_partitionToConvex_current->setGridVisibility(true);
	ui.drar_partitionToConvex_current->clearDrawArea();

	connect(ui.btn_backToMainAlgoStages_fromConvexPartitions, &QPushButton::clicked, this,
		[=](bool checked) {
			ui.convexPartitions->setVisible(false);
			ui.mainAlgoImages->setVisible(true);
		});

	connect(ui.btn_goToNextPartition, &QPushButton::clicked, this,
		[=](bool checked) {
			tryDrawNextConvexPartitionMesh();
			ui.lbl_numOfConvexPartitions->setText("Partition " + QString::number(currentConexPartitionsMeshe + 1) + " / " + QString::number(conexPartitionsMeshes.size()));
		});

	connect(ui.btn_goToPrevPartition, &QPushButton::clicked, this,
		[=](bool checked) {
			tryDrawPrevConvexPartitionMesh();
			ui.lbl_numOfConvexPartitions->setText("Partition " + QString::number(currentConexPartitionsMeshe + 1) + " / " + QString::number(conexPartitionsMeshes.size()));
		});

#pragma endregion

#pragma region Convex Parts Partitions

	ui.convexPartsPartition->setVisible(false);

	ui.drar_convexPart_part->setScale(3, 3);
	ui.drar_convexPart_part->setGridVisibility(true);
	ui.drar_convexPart_part->clearDrawArea();

	ui.drar_convexPart_splittedVert->setScale(3, 3);
	ui.drar_convexPart_splittedVert->setGridVisibility(true);
	ui.drar_convexPart_splittedVert->clearDrawArea();

	ui.drar_convexPart_splittedFaces->setScale(3, 3);
	ui.drar_convexPart_splittedFaces->setGridVisibility(true);
	ui.drar_convexPart_splittedFaces->clearDrawArea();

	ui.drar_convexPart_optimal->setScale(3, 3);
	ui.drar_convexPart_optimal->setGridVisibility(true);
	ui.drar_convexPart_optimal->clearDrawArea();

	connect(ui.btn_backToMainAlgoStages_fromConvexPartDetails, &QPushButton::clicked, this,
		[=](bool checked) {
			ui.convexPartsPartition->setVisible(false);
			ui.mainAlgoImages->setVisible(true);
		});

	connect(ui.btn_goToNextConvexPart, &QPushButton::clicked, this,
		[=](bool checked) {
			tryDrawNextPartPartition();
			ui.lbl_numOfConvexPart->setText("Part " + QString::number(currentPartPartition + 1) + " / " + QString::number(partPartitions.size()));
		});

	connect(ui.btn_goToPrevConvexPart, &QPushButton::clicked, this,
		[=](bool checked) {
			tryDrawPrevPartPartition();
			ui.lbl_numOfConvexPart->setText("Part " + QString::number(currentPartPartition + 1) + " / " + QString::number(partPartitions.size()));
		});

	partPartitions = std::vector<std::tuple<PolygonData, PolygonData, PolygonData, PolygonData>>();

#pragma endregion

	isHintForSelectDrawingAreaShowned = false;
}

void ApplicationGUI::setActiveGroupBox(std::string grb_name, bool isNext)
{
	clearAllAlgoDrawingAreas();

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
			ui.sld_areaOfPart->setMaximum(polygonArea * SLD_SCALE / 2);
			ui.sld_areaOfPart->setEnabled(true);
			sliderRangeChanged();

			ui.spin_meshAngle->setEnabled(true);
			ui.btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
			ui.meshAngleDrawingArea->clearDrawArea();

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
			ui.btn_reset_angle->setEnabled(true);

			isModeToSelectDrawingArea = false;
		}
		else {
			activeGroupBox = "meshProperties";

			ui.btn_reset->setEnabled(true);
			ui.btn_apply->setEnabled(false);
			ui.spin_numOfSides->setEnabled(false);

			ui.spin_meshAngle->setEnabled(true);
			ui.btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
			ui.meshAngleDrawingArea->clearDrawArea();

			auto data = convertPolygonToPolygonData();
			drawPolygonData(ui.polygonDrawingArea, 4, *data);

			isModeToSelectDrawingArea = false;
		}
	}
}

void ApplicationGUI::addPointToPolygon(double x, double y, bool isLogicCoords)
{
	QPointF logicPoint, pixelPoint;

	if (!isLogicCoords) {
		pixelPoint = QPointF(x, y);
	}
	else {
		pixelPoint = ui.polygonDrawingArea->LogicToPixelCoords(QPoint(x, y), false);
	}

	logicPoint = ui.polygonDrawingArea->PixelToLogicCoords(QPoint(pixelPoint.x(), pixelPoint.y()), false);

	if (polygon != nullptr && polygon->tryAddPoint({ logicPoint.x(), logicPoint.y() })) {
		ui.polygonDrawingArea->drawEllipse(QPoint(pixelPoint.x(), pixelPoint.y()), 5, false, Qt::black);

		if (polygon->getNumOfPoints() > 1)
		{
			auto prev = polygon->getPreviousPoint();
			ui.polygonDrawingArea->drawLine(ui.polygonDrawingArea->LogicToPixelCoords(QPointF(prev.x, prev.y), false), QPoint(pixelPoint.x(), pixelPoint.y()), false, Qt::black, 1);
		}

		if (polygon->getNumOfSides() == polygon->getNumOfPoints()) {
			auto firstPoint = polygon->getPointAt(0);
			ui.polygonDrawingArea->drawLine(ui.polygonDrawingArea->LogicToPixelCoords(QPointF(firstPoint.x, firstPoint.y), false), QPoint(pixelPoint.x(), pixelPoint.y()), false, Qt::black, 1);

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

	if (polygon->isLeftTraversal(EPS)) {
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

	drawPolygonMesh(ui.drar_mainStages_rotated, 2, rotatedData.vertex_x, rotatedData.vertex_y, rotatedData.edges, rotatedData.faces);

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(rotatedData);
	m.F[0]->area = polygonArea;

	m.splitByVerticalGrid();

	auto SplittedByGridData = m.convertToPolygonData();
	drawPolygonMesh(ui.drar_mainStages_concavePoints, 2, SplittedByGridData.vertex_x, SplittedByGridData.vertex_y, SplittedByGridData.edges, SplittedByGridData.faces);

	//auto splitedEdges = m.splitFaces(polygonArea / ui.spin_numOfParts->value());
	auto splitedEdges = nullptr;

	auto verticalSplittedData = m.convertToPolygonData();
	drawPolygonMesh(ui.drar_mainStages_convexParts, 2, verticalSplittedData.vertex_x, verticalSplittedData.vertex_y, verticalSplittedData.edges, verticalSplittedData.faces);

	auto perims = m.getInternalHorizontalPerimeters();
	auto optimal = m.getOptimalCombinationForInternalPerimeter(perims);
	m.splitMeshByMask(optimal.second);

	auto horisontalSplittedData = m.convertToPolygonData();
	drawPolygonMesh(ui.drar_mainStages_optimal, 2, horisontalSplittedData.vertex_x, horisontalSplittedData.vertex_y, horisontalSplittedData.edges, horisontalSplittedData.faces);

	auto result = m.convertToPolygonData();
	r.tryRrotateTheFigureBack(&result.vertex_x, &result.vertex_y);
	drawPolygonMesh(ui.polygonDrawingArea, 5, result.vertex_x, result.vertex_y, result.edges, result.faces);
}

void ApplicationGUI::drawPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces, bool isNeedToClean)
{
	drawColoredPolygonMesh(drawingArea, radiusOfPoints, x, y, edges, faces, isNeedToClean, false);
}

void ApplicationGUI::drawColoredPolygonMesh(DrawingArea* drawingArea, int radiusOfPoints, vectorD x, vectorD y, vectorI edges, vectorI faces, bool isNeedToClean, bool isColored) {
	if (isNeedToClean) {
		drawingArea->clearDrawArea();
	}

	if (isColored) {
		for (int i = 1; i < faces.size(); ++i) {
			auto p = QPolygonF();
			for (int j = faces[i - 1]; j < faces[i]; ++j) {
				p.push_back(drawingArea->LogicToPixelCoords(QPointF(x[edges[j]], y[edges[j]]), false));
			}

			drawingArea->fillPath(p, 1, false, getNextColor());
		}
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

	for (int i = 0; i < x.size(); ++i) {
		auto pixel = drawingArea->LogicToPixelCoords(QPointF(x[i], y[i]), false);
		drawingArea->drawEllipse(pixel, radiusOfPoints, false, Qt::black);
	}

}

void ApplicationGUI::drawPolygonData(DrawingArea* drawingArea, int radiusOfPoints, PolygonData data, bool isNeedToClean)
{
	drawPolygonMesh(drawingArea, radiusOfPoints, data.vertex_x, data.vertex_y, data.edges, data.faces, isNeedToClean);
}

void ApplicationGUI::setSliderLabelsPosition()
{
	auto w = ui.sld_areaOfPart->width();
	auto lbl_w = 40;
	auto lbl_h = 16;
	auto offset = 10;

	ui.sld_minPosLabel->setGeometry(offset + ((double)ui.sld_areaOfPart->minimumPosition() / ui.sld_areaOfPart->maximum() * (w - lbl_w)), 40, lbl_w, lbl_h);

	QRect d = ui.sld_maxPosLabel->geometry();
	QRect droppedRect = QRect(d.left(), 40, d.width(), d.height());

	if (droppedRect.intersects(ui.sld_minPosLabel->geometry()))
	{
		ui.sld_maxPosLabel->setGeometry(offset + ((double)ui.sld_areaOfPart->maximumPosition() / ui.sld_areaOfPart->maximum() * (w - lbl_w)), 0, lbl_w, lbl_h);
	}
	else {
		ui.sld_maxPosLabel->setGeometry(offset + ((double)ui.sld_areaOfPart->maximumPosition() / ui.sld_areaOfPart->maximum() * (w - lbl_w)), 40, lbl_w, lbl_h);
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

void ApplicationGUI::btn_reset_angle_clicked(bool checked)
{
	setActiveGroupBox("meshProperties", false);
}

void ApplicationGUI::calculatePolygonProperties()
{
	bool isSelfIntersected = false;
	int size = polygon->getNumOfPoints();

	for (int i = 0; i < size; ++i) {
		for (int j = (i + 2) % size; i != (j + 1) % size; j = (j + 1) % size) {
			if (a::Intersection::doIntersect(
				polygon->getPointAt(i), polygon->getPointAt((i + 1) % size),
				polygon->getPointAt(j), polygon->getPointAt((j + 1) % size)))
			{
				isSelfIntersected = true;
				break;
			}
		}

		if (isSelfIntersected)
			break;
	}

	if (isSelfIntersected) {
		QMessageBox::critical(this, "", "Polygon can not be self-intersecting");
	}
	else {
		polygonArea = polygon->getSquare();
		setActiveGroupBox("areaProperties", true);
		auto data = convertPolygonToPolygonData();
		Mesh m = Mesh();
		m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);
		polygonDrawingAreaMesh = m.convertToString();
	}
}

void ApplicationGUI::sliderRangeChanged()
{
	if (ui.sld_areaOfPart->maximum() > 0)
	{
		ui.sld_areaOfPart->setMinimum(10);
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
			else if (ui.drar_mainStages_rotated->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drar_mainStages_rotated);
			}
			else if (ui.drar_mainStages_concavePoints->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drar_mainStages_concavePoints);
			}
			else if (ui.drar_mainStages_convexParts->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drar_mainStages_convexParts);
			}
			else if (ui.drar_mainStages_optimal->isPointInBounds(point.x(), point.y())) {
				emit drawingAreaSelected(ui.drar_mainStages_optimal);
			}
		}
		else {
			if (ui.polygonDrawingArea->isPointInBounds(point.x(), point.y())) {
				addPointToPolygon(point.x(), point.y());
			}
			else if (isInMeshAngleDrawingAreaBounds(point.x(), point.y()) && ui.tabWidget->currentIndex() == 0) {
				addPointToAngle(point.x(), point.y());
			}
		}
	}
}

void ApplicationGUI::btn_drawPoly(bool checked)
{
	//FIGURE 1
	polygon = new a::Polygon(10);

	QPoint p;
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 50.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 45.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 40.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 15.0, 30.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 35.0, 30.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 15.0, 20.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 25.0, 15.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 15.0, 5.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 45.0, 15.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 45.0, 55.0 }, false);
	addPointToPolygon(p.x(), p.y());

	/*
	// FIGURE 2
	polygon = new a::Polygon(7);

	QPoint p;
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 35.0, 30.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 35.0, 15.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 35.0, 11.6667 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 45.0, 15.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 45.0, 55.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 35.0, 52.0 }, false);
	addPointToPolygon(p.x(), p.y());
	p = ui.polygonDrawingArea->LogicToPixelCoords({ 35.0, 40.0 }, false);
	addPointToPolygon(p.x(), p.y());*/
}

PolygonData* ApplicationGUI::convertPolygonToPolygonData() {
	vectorD* x = new vectorD(polygon->getNumOfPoints());
	vectorD* y = new vectorD(polygon->getNumOfPoints());
	vectorI* edges = new vectorI(polygon->getNumOfPoints());

	if (polygon->isLeftTraversal(EPS)) {
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

	PolygonData* data = new PolygonData();
	data->vertex_x = *x;
	data->vertex_y = *y;
	data->tryAddFace(edges->size(), *edges);

	return data;
}

bool ApplicationGUI::tryDrawNextConvexPartitionMesh() {
	if (currentConexPartitionsMeshe + 1 == conexPartitionsMeshes.size() || currentConexPartitionsMeshe + 1 < 0)
		return false;

	currentConexPartitionsMeshe++;

	drawCurPartConvexPartitionMesh();
}

bool ApplicationGUI::tryDrawPrevConvexPartitionMesh() {
	if (currentConexPartitionsMeshe - 1 < 0 || currentConexPartitionsMeshe - 1 >= conexPartitionsMeshes.size())
		return false;

	currentConexPartitionsMeshe--;

	drawCurPartConvexPartitionMesh();
}

void ApplicationGUI::drawCurPartConvexPartitionMesh() {
	auto curMesh = conexPartitionsMeshes[currentConexPartitionsMeshe].first;
	auto curCharacteristics = conexPartitionsMeshes[currentConexPartitionsMeshe].second;
	
	auto data = curMesh->convertToPolygonData();
	drawPolygonMesh(ui.drar_partitionToConvex_current, 2, data.vertex_x, data.vertex_y, data.edges, data.faces);

	ui.lbl_info_nonDividedArea->setText(QString::number(curCharacteristics->getAreaOfNotSplittedParts()));
	ui.lbl_info_notDividedPercent->setText(QString::number(curCharacteristics->getPercentageOfNotSplittedParts()) + "%");
	ui.lbl_info_optimisationFuncValue->setText(QString::number(curCharacteristics->getOptimizationFuncValue()));
}

bool ApplicationGUI::tryDrawNextPartPartition() {
	if (currentPartPartition + 1 == partPartitions.size() || currentPartPartition + 1 < 0)
		return false;

	currentPartPartition++;

	drawCurPartPartition();
}

bool ApplicationGUI::tryDrawPrevPartPartition() {
	if (currentPartPartition - 1 < 0 || currentPartPartition - 1 >= partPartitions.size())
		return false;

	currentPartPartition--;

	drawCurPartPartition();
}

void ApplicationGUI::drawCurPartPartition() {
	auto cur = partPartitions[currentPartPartition];

	drawPolygonData(ui.drar_convexPart_part, 2, std::get<0>(cur));

	if (std::get<1>(cur).vertex_x.size() == 0) {
		setPartPartitionControlsVisibility(true);
	}
	else {
		setPartPartitionControlsVisibility(false);
		ui.lbl_info_numOfSplittedParts->setText("Number of splitted parts: " + QString::number(std::get<3>(cur).getNumOfFaces()));
		drawPolygonData(ui.drar_convexPart_splittedVert, 2, std::get<1>(cur));
		drawPolygonData(ui.drar_convexPart_splittedFaces, 2, std::get<2>(cur));
		drawPolygonData(ui.drar_convexPart_optimal, 2, std::get<3>(cur));
	}
}

void ApplicationGUI::setPartPartitionControlsVisibility(bool isNotDividedPart) {
	ui.lbl_info_notDividedPart->setVisible(isNotDividedPart);
	ui.lbl_info_dividedPart_1->setVisible(!isNotDividedPart);
	ui.lbl_info_dividedPart_2->setVisible(!isNotDividedPart);
	ui.lbl_info_dividedPart_3->setVisible(!isNotDividedPart);
	ui.drar_convexPart_splittedVert->setVisible(!isNotDividedPart);
	ui.drar_convexPart_splittedFaces->setVisible(!isNotDividedPart);
	ui.drar_convexPart_optimal->setVisible(!isNotDividedPart);
	ui.lbl_info_numOfSplittedParts->setVisible(!isNotDividedPart);
}

std::vector<int> ApplicationGUI::getConcavePoints() {
	auto data = convertPolygonToPolygonData();

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);

	return m.findConcavePoints();
}

void ApplicationGUI::btn_doAlgo(bool checked)
{
	if (anglePoints.first == nullptr || anglePoints.second == nullptr)
		return;

	ui.progressBar->setVisible(true);
	ui.progressBar->setValue(0);

	ui.btn_convexPartitionStages->setVisible(true);

	auto points = getConcavePoints();
	ui.progressBar->setValue(4);

	if (points.size() > 0) {
		ui.btn_allConcavePartitions->setVisible(true);
	}

	auto data = convertPolygonToPolygonData();

	Rotation r = Rotation(ui.spin_meshAngle->value(), RotationDirection::Right);
	r.tryRotateFigure(&data->vertex_x, &data->vertex_y);
	ui.progressBar->setValue(7);

	drawPolygonMesh(ui.drar_mainStages_rotated, 2, data->vertex_x, data->vertex_y, data->edges, data->faces);

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);
	polygonDrawingAreaMesh = m.convertToString();

	auto start = std::chrono::steady_clock::now();

	auto meshesRef = *m.splitToConvexPolygons(points);
	ui.progressBar->setValue(17);
	auto meshesRefCharacteristics = std::vector<ConvexPartitionCharacteristics*>();
	auto optimalIndex = Mesh::getOptimalNumOfMeshWithMaxSquareForSplit(&meshesRef, (double)ui.sld_areaOfPart->minimumValue() / SLD_SCALE, (double)ui.sld_areaOfPart->maximumValue() / SLD_SCALE, &meshesRefCharacteristics);
	optimal = meshesRef[optimalIndex];
	optimalCharacteristics = meshesRefCharacteristics[optimalIndex];
	ui.progressBar->setValue(20);

	auto end = std::chrono::steady_clock::now();


	auto start1 = std::chrono::steady_clock::now();
	drawPolygonMesh(ui.drar_mainStages_concavePoints, 2, data->vertex_x, data->vertex_y, data->edges, data->faces);
	for (int i = 0; i < points.size(); ++i) {
		auto pixel = ui.drar_mainStages_concavePoints->LogicToPixelCoords(QPointF(m.V[points[i]]->pos->x, m.V[points[i]]->pos->y), false);
		ui.drar_mainStages_concavePoints->drawEllipse(pixel, 3, false, Qt::red);
	}
	test1 = m.convertToString();

	//////////////
	conexPartitionsMeshes.clear();
	for (int i = 0; i < meshesRef.size(); ++i) {
		conexPartitionsMeshes.push_back({ meshesRef[i], meshesRefCharacteristics[i]});
	}

	currentConexPartitionsMeshe = -1;
	ui.lbl_partitionToConvexParts_optimal->setText("Optimal partition - " + QString::number(optimalIndex + 1));
	ui.lbl_info_nonDividedArea_optimal->setText(QString::number(optimalCharacteristics->getAreaOfNotSplittedParts()));
	ui.lbl_info_notDividedPercent_optimal->setText(QString::number(optimalCharacteristics->getPercentageOfNotSplittedParts()) + "%");
	ui.lbl_info_optimisationFuncValue_optimal->setText(QString::number(optimalCharacteristics->getOptimizationFuncValue()));
	ui.lbl_info_nonDividedArea_optimal_2->setText(QString::number(optimalCharacteristics->getAreaOfNotSplittedParts()));
	ui.lbl_info_notDividedPercent_optimal_2->setText(QString::number(optimalCharacteristics->getPercentageOfNotSplittedParts()) + "%");
	ui.lbl_info_optimisationFuncValue_optimal_2->setText(QString::number(optimalCharacteristics->getOptimizationFuncValue()));
	tryDrawNextConvexPartitionMesh();
	/////////////

	test3 = optimal->convertToString();

	auto data2 = optimal->convertToPolygonData();
	drawPolygonMesh(ui.drar_mainStages_convexParts, 2, data2.vertex_x, data2.vertex_y, data2.edges, data2.faces);
	drawPolygonMesh(ui.drar_partitionToConvex_optimal, 2, data2.vertex_x, data2.vertex_y, data2.edges, data2.faces);

	auto end1 = std::chrono::steady_clock::now();

	/*QMessageBox::information(this, "",
		"Algo time: " + QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) + " ms.\n"
		+ "Draw time: " + QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1).count()) + " ms.");*/

	test(r);
}

void ApplicationGUI::test(Rotation r) {
	partPartitions.clear();
	auto meshes = std::vector<Mesh*>();
	auto nonSplittedMeshes = std::vector<Mesh*>();

	double minS = (double)ui.sld_areaOfPart->minimumValue() / SLD_SCALE;
	double maxS = (double)ui.sld_areaOfPart->maximumValue() / SLD_SCALE;

	auto progressBarStep = (int)floor(80.0 / optimal->F.size());

	for (auto face : optimal->F) {
		auto mesh = Mesh::createFromFace(face);

		if (Mesh::canBeSplittedToEqualSquareParts(face, minS, maxS) == false) {
			nonSplittedMeshes.push_back(mesh);
			continue;
		}

		auto stage1 = mesh->convertToPolygonData();

		auto partArea = Mesh::getPartAreaToSplitMeshTEST(face, minS, maxS);

		if (abs(partArea - face->area) < EPS) {
			partPartitions.push_back({ stage1, PolygonData(), PolygonData(), PolygonData() });
		}
		else {
			mesh->splitByVerticalGrid();
			auto stage2 = mesh->convertToPolygonData();

			mesh->splitFaces(partArea);
			auto stage3 = mesh->convertToPolygonData();

			auto perims = mesh->getInternalHorizontalPerimeters();
			auto optimal = mesh->getOptimalCombinationForInternalPerimeter(perims);

			mesh->splitMeshByMask(optimal.second);
			auto stage4 = mesh->convertToPolygonData();

			partPartitions.push_back({ stage1, stage2, stage3, stage4 });

			ui.progressBar->setValue(ui.progressBar->value() + progressBarStep);
		}

		meshes.push_back(mesh);
	}

	ui.drar_mainStages_optimal->clearDrawArea();
	ui.polygonDrawingArea->clearDrawArea();

	for (auto mesh : nonSplittedMeshes) {
		auto data = mesh->convertToPolygonData();
		drawPolygonMesh(ui.drar_mainStages_optimal, 2, data.vertex_x, data.vertex_y, data.edges, data.faces, false);

		r.tryRrotateTheFigureBack(&data.vertex_x, &data.vertex_y);
		drawPolygonMesh(ui.polygonDrawingArea, 0, data.vertex_x, data.vertex_y, data.edges, data.faces, false);
	}

	for (auto mesh : meshes) {
		auto data = mesh->convertToPolygonData();
		drawPolygonMesh(ui.drar_mainStages_optimal, 2, data.vertex_x, data.vertex_y, data.edges, data.faces, false);

		r.tryRrotateTheFigureBack(&data.vertex_x, &data.vertex_y);
		drawColoredPolygonMesh(ui.polygonDrawingArea, 0, data.vertex_x, data.vertex_y, data.edges, data.faces, false);
	}

	auto data = convertPolygonToPolygonData();
	for (int i = 0; i < data->vertex_x.size(); ++i) {
		auto pixel = ui.polygonDrawingArea->LogicToPixelCoords(QPointF(data->vertex_x[i], data->vertex_y[i]), false);
		ui.polygonDrawingArea->drawEllipse(pixel, 4, false, Qt::black);
	}

	ui.progressBar->setValue(100);

	currentPartPartition = -1;
	tryDrawNextPartPartition();

	ui.progressBar->setVisible(false);
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
	else if (drawingAreaOfMesh == ui.drar_mainStages_rotated) {
		toSave = test1;
	}
	else if (drawingAreaOfMesh == ui.drar_mainStages_concavePoints) {
		toSave = test2;
	}
	else if (drawingAreaOfMesh == ui.drar_mainStages_convexParts) {
		toSave = test3;
	}
	else if (drawingAreaOfMesh == ui.drar_mainStages_optimal) {
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
		ui.polygonDrawingArea->clearDrawArea();
		polygon = new a::Polygon(data.vertex_x.size());
		ui.spin_numOfSides->setValue(polygon->getNumOfSides());

		for (int i = 0; i < data.vertex_x.size(); ++i) {
			addPointToPolygon(data.vertex_x[i], data.vertex_y[i], true);
		}
	}
	else {
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

	if (file_name.isEmpty())
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
		controlsStatesBeforeSelectingMode.push_back(ui.btn_reset_angle->isEnabled());
		ui.btn_reset_angle->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.btn_uploadMeshFromText->isEnabled());
		ui.btn_uploadMeshFromText->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.btn_drawPoly->isEnabled());
		ui.btn_drawPoly->setEnabled(false);

		controlsStatesBeforeSelectingMode.push_back(ui.sld_areaOfPart->isEnabled());
		ui.sld_areaOfPart->setEnabled(false);

		ui.polygonDrawingArea->setHoverEffectEnabled(true);
		ui.drar_mainStages_rotated->setHoverEffectEnabled(true);
		ui.drar_mainStages_concavePoints->setHoverEffectEnabled(true);
		ui.drar_mainStages_convexParts->setHoverEffectEnabled(true);
		ui.drar_mainStages_optimal->setHoverEffectEnabled(true);
	}
	else {
		int cnt = 0;

		ui.btn_reset->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_apply->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.spin_numOfSides->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.spin_meshAngle->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);
		ui.btn_reset_angle->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.btn_uploadMeshFromText->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.btn_drawPoly->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.sld_areaOfPart->setEnabled(controlsStatesBeforeSelectingMode[cnt++]);

		ui.polygonDrawingArea->deleteAllEffects();
		ui.drar_mainStages_rotated->deleteAllEffects();
		ui.drar_mainStages_concavePoints->deleteAllEffects();
		ui.drar_mainStages_convexParts->deleteAllEffects();
		ui.drar_mainStages_optimal->deleteAllEffects();
	}
}

void ApplicationGUI::clearAllAlgoDrawingAreas()
{
	ui.drar_mainStages_rotated->clearDrawArea();
	ui.drar_mainStages_concavePoints->clearDrawArea();
	ui.drar_mainStages_convexParts->clearDrawArea();
	ui.drar_mainStages_optimal->clearDrawArea();

	ui.btn_allConcavePartitions->setVisible(false);
	ui.btn_convexPartitionStages->setVisible(false);

	ui.convexPartitions->setVisible(false);
	ui.convexPartsPartition->setVisible(false);
	ui.mainAlgoImages->setVisible(true);
}
