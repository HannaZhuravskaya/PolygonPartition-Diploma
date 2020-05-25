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
}

void ApplicationGUI::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		auto point = event->pos();

		if (ui.polygonDrawingArea->isPointInBounds(point.x(), point.y())) {
			addPointToPolygon(point.x(), point.y());
		}
		else if (isInMeshAngleDrawingAreaBounds(point.x(), point.y()) && ui.tabWidget->currentIndex() == 0) {
			addPointToAngle(point.x(), point.y());
		}
	}
}

void ApplicationGUI::mouseMoveEvent(QMouseEvent* event)
{
	mouseCoordinates->setVisible(false);
}

void ApplicationGUI::sliderRangeChanged()
{
	if (ui.sld_areaOfPart->maximum() > 0)
	{
		ui.sld_areaOfPart->setMinimum(isSliderAreaInPercents ? 1 : 10);
	}

	ui.sld_areaOfPart->setMinimumPosition(0);
	ui.sld_areaOfPart->setMaximumPosition(ui.sld_areaOfPart->maximum());

	areaOfPartRangeChanged(ui.sld_areaOfPart->minimumPosition(), ui.sld_areaOfPart->maximumPosition());
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

void ApplicationGUI::initializeControls()
{
#pragma region Main Stages

	ui.progressBar->setVisible(false);

	currentConvexPartitionsMesh = -1;
	convexPartitionsMeshes = std::vector<std::pair<Mesh*, ConvexPartitionCharacteristics*>>();
	
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

	connect(ui.btn_uploadMeshFromText, &QPushButton::clicked, this, [=](bool checked) {uploadMeshFromText(); });
	connect(ui.btn_drawPoly, &QPushButton::clicked, this, &ApplicationGUI::drawTestPolygon);
	
	mouseCoordinates = new QLabel(this);
	mouseCoordinates->setStyleSheet("QLabel { background-color : white;}");

	mouseCoordinates->setGeometry(5, 5, 50, 10);

#pragma region Polygon properties

	ui.polygonDrawingArea->setGridVisibility(true);
	connect(ui.btn_apply, &QPushButton::clicked, this, [=](bool checked) {setActiveGroupBox("polygonProperties", true); });
	connect(ui.btn_reset, &QPushButton::clicked, this, [=](bool checked) {setActiveGroupBox("polygonProperties", false); });

#pragma endregion

#pragma region Area properties
	connect(ui.sld_areaOfPart, &RangeSlider::minimumPositionChanged, this,
		[=](int min) {
			minPartArea = isSliderAreaInPercents ? (double)((int)(polygonArea * min)) / 100 : min / SLD_SCALE;
			ui.sld_minPosLabel->setText(isSliderAreaInPercents ? QString::number(min) + "%" : QString::number(minPartArea));
			ui.lbl_minAreaOfPart->setText(QString::number(minPartArea));
			setSliderLabelsPosition();
		});

	connect(ui.sld_areaOfPart, &RangeSlider::maximumPositionChanged, this,
		[=](int max) {
			maxPartArea = isSliderAreaInPercents ? (double)((int)(polygonArea * max)) / 100 : max / SLD_SCALE;
			ui.sld_maxPosLabel->setText(isSliderAreaInPercents ? QString::number(max) + "%" : QString::number(maxPartArea));
			ui.lbl_maxAreaOfPart->setText(QString::number(maxPartArea));
			setSliderLabelsPosition();
		});

	connect(ui.sld_areaOfPart, &RangeSlider::sliderRealesed, this,
		[=]() {
			if (anglePoints.first != nullptr && anglePoints.second != nullptr) {
				setActiveGroupBox("meshProperties", true);
				tryExecuteAlgorithm();
			}
			else {
				setActiveGroupBox("meshProperties", false);
			}
		});

	connect(ui.chb_isAreaRestrictionsInPercent, &QCheckBox::stateChanged, this,
		[=](int state) {
			isSliderAreaInPercents = ui.chb_isAreaRestrictionsInPercent->isChecked();

			if (ui.sld_areaOfPart->maximumPosition() == 0) {
				ui.sld_areaOfPart->setMaximum(0);
			}
			else {
				ui.sld_areaOfPart->setMaximum(isSliderAreaInPercents ? 50 : polygonArea * SLD_SCALE / 2);
			}

			ui.sld_areaOfPart->setMinimum(0);

			sliderRangeChanged();

			if (ui.sld_areaOfPart->maximumPosition() != 0) {
				if (anglePoints.first != nullptr && anglePoints.second != nullptr) {
					setActiveGroupBox("meshProperties", true);
					tryExecuteAlgorithm();
				}
				else {
					setActiveGroupBox("meshProperties", false);
				}
			}
		});

	isSliderAreaInPercents = ui.chb_isAreaRestrictionsInPercent->isChecked();

#pragma endregion

#pragma region Mesh properties
	ui.meshAngleDrawingArea->setGridVisibility(false);
	connect(ui.btn_reset_angle, &QPushButton::clicked, this, [=](bool checked) {setActiveGroupBox("meshProperties", false); });
	ui.meshAngleDrawingArea->setX();
	ui.meshAngleDrawingArea->setY();
#pragma endregion

	connect(ui.btn_allConcavePartitions, &QPushButton::clicked, this,
		[=](bool checked) {
			ui.convexPartitions->setVisible(true);
			ui.mainAlgoImages->setVisible(false);
			ui.lbl_numOfConvexPartitions->setText("Part " + QString::number(currentConvexPartitionsMesh + 1) + " / " + QString::number(convexPartitionsMeshes.size()));
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

	connect(ui.btn_goToNextPartition, &QPushButton::clicked, this, [=](bool checked) {tryDrawNextConvexPartitionMesh(); });

	connect(ui.btn_goToPrevPartition, &QPushButton::clicked, this, [=](bool checked) {tryDrawPrevConvexPartitionMesh(); });

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

	connect(ui.btn_goToNextConvexPart, &QPushButton::clicked, this, [=](bool checked) {tryDrawNextPartPartition(); });

	connect(ui.btn_goToPrevConvexPart, &QPushButton::clicked, this, [=](bool checked) {tryDrawPrevPartPartition(); });

	partPartitions = std::vector<std::tuple<PolygonData, PolygonData, PolygonData, PolygonData>>();

#pragma endregion

	connect(ui.btn_help, &QPushButton::clicked, this,
		[=](bool clicked) {
			QString link = "https://github.com/HannaZhuravskaya/PolygonPartition-Diploma";
			QDesktopServices::openUrl(QUrl(link));
		});

	connect(ui.btn_saveResults, &QPushButton::clicked, this, [=](bool clicked) {saveResults(); });
	ui.qrb_processingResults->setEnabled(false);

	ui.btn_algoResults_show->setVisible(false);
	ui.btn_algoResults_hide->setVisible(true);

	connect(ui.btn_algoResults_show, &QPushButton::clicked, this,
		[=](bool clicked) {
			ui.btn_algoResults_show->setVisible(false);
			ui.frm_algoResults->setGeometry(27, 23, 201, 150);
		});

	connect(ui.btn_algoResults_hide, &QPushButton::clicked, this,
		[=](bool clicked) {
			ui.btn_algoResults_show->setVisible(true);
			ui.frm_algoResults->setGeometry(26, 22, 20, 20);
		});
}

void ApplicationGUI::setActiveGroupBox(std::string grb_name, bool isNext)
{
	clearAllAlgoDrawingAreas();

	ui.frm_algoResults->setVisible(false);

	ui.qrb_processingResults->setEnabled(false);

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

			polygon = new algo::Polygon(ui.spin_numOfSides->value());
			polygonArea = 0;
			anglePoints = std::make_pair(nullptr, nullptr);
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
		}
	}
	else if (grb_name == "areaProperties") {
		if (isNext) {
			activeGroupBox = "meshProperties";

			ui.btn_reset->setEnabled(true);
			ui.btn_apply->setEnabled(false);
			ui.spin_numOfSides->setEnabled(false);

			ui.lbl_polygonArea->setText(QString::number(polygonArea));
			ui.sld_areaOfPart->setMaximum(isSliderAreaInPercents ? 50 : polygonArea * SLD_SCALE / 2);
			ui.sld_areaOfPart->setEnabled(true);
			sliderRangeChanged();

			ui.spin_meshAngle->setEnabled(true);
			ui.btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
			ui.meshAngleDrawingArea->clearDrawArea();
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
		}

		auto data = convertPolygonToPolygonData();
		drawPolygonData(ui.polygonDrawingArea, 4, *data);
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
		ui.polygonDrawingArea->drawEllipse(QPoint(pixelPoint.x(), pixelPoint.y()), 4, false, Qt::black);

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
		anglePoints.first = new Point{ (double)x, (double)y };
		ui.meshAngleDrawingArea->drawEllipse(QPoint(x, y), 4, false);
	}
	else if (anglePoints.second == nullptr) {
		if ((double)x < anglePoints.first->x) {
			anglePoints.second = new  Point{ anglePoints.first->x, anglePoints.first->y };
			anglePoints.first = new  Point{ (double)x, (double)y };
		}
		else
		{
			anglePoints.second = new  Point{ (double)x, (double)y };
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

		tryExecuteAlgorithm();
	}
}

bool ApplicationGUI::isInMeshAngleDrawingAreaBounds(int x, int y)
{
	return activeGroupBox == "meshProperties" && ui.meshAngleDrawingArea;
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

void ApplicationGUI::uploadMeshFromText() {
	QString file_name = QFileDialog::getOpenFileName(this, "Choose PolygonalMesh source", QDir::homePath());

	if (file_name == "")
		return;

	std::ifstream in(file_name.toStdString());
	std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

	auto mesh = Mesh::convertFromString(str);
	if (mesh == nullptr) {
		QMessageBox::critical(this, "", "Cannot be converted into polygonal mesh");
		return;
	}

	if (mesh->F.size() != 1) {
		QMessageBox::critical(this, "", "Only polygonal mesh with one face can be loaded. Your mesh contains " + QString::number(mesh->F.size()) + " faces.");
		return;
	}

	auto data = mesh->convertToPolygonData();

	ui.polygonDrawingArea->clearDrawArea();
	polygon = new  algo::Polygon(data.vertex_x.size());
	ui.spin_numOfSides->setValue(polygon->getNumOfSides());

	for (int i = 0; i < data.vertex_x.size(); ++i) {
		addPointToPolygon(data.vertex_x[i], data.vertex_y[i], true);
	}
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
		minPartArea = isSliderAreaInPercents ? (double)((int)(polygonArea * min)) / 100 : min / SLD_SCALE;
		maxPartArea = isSliderAreaInPercents ? (double)((int)(polygonArea * max)) / 100 : max / SLD_SCALE;
		ui.sld_minPosLabel->setText(isSliderAreaInPercents ? QString::number(min) + "%" : QString::number(minPartArea));
		ui.sld_maxPosLabel->setText(isSliderAreaInPercents ? QString::number(max) + "%" : QString::number(maxPartArea));
		ui.lbl_minAreaOfPart->setText(QString::number(minPartArea));
		ui.lbl_maxAreaOfPart->setText(QString::number(maxPartArea));
		setSliderLabelsPosition();
	}
}

void ApplicationGUI::calculatePolygonProperties()
{
	bool isSelfIntersected = false;
	int size = polygon->getNumOfPoints();

	for (int i = 0; i < size; ++i) {
		for (int j = (i + 2) % size; i != (j + 1) % size; j = (j + 1) % size) {
			if (Intersection::doIntersect(
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
		QMessageBox::critical(this, "", "Polygon cannot be self-intersecting");
	}
	else {
		polygonArea = polygon->getSquare();
		setActiveGroupBox("areaProperties", true);
	}
}

void ApplicationGUI::drawTestPolygon()
{
	ui.polygonDrawingArea->clearDrawArea();

	polygon = new  algo::Polygon(10);
	ui.spin_numOfSides->setValue(polygon->getNumOfSides());

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

			drawingArea->fillPath(p, 1, false, QColor(rand() % 256, rand() % 256, rand() % 256, 100));
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

void ApplicationGUI::tryExecuteAlgorithm()
{
	if (anglePoints.first == nullptr || anglePoints.second == nullptr)
		return;

	ui.progressBar->setVisible(true);
	ui.progressBar->setValue(0);

	ui.btn_convexPartitionStages->setVisible(true);

	auto data = convertPolygonToPolygonData();

	Rotation r = Rotation(ui.spin_meshAngle->value(), RotationDirection::Right);
	r.tryRotateFigure(&data->vertex_x, &data->vertex_y);
	
	drawPolygonData(ui.drar_mainStages_rotated, 2, *data);

	ui.progressBar->setValue(3);

	auto points = getConcavePoints();

	if (points.size() > 0) {
		ui.btn_allConcavePartitions->setVisible(true);
	}

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);

	drawPolygonData(ui.drar_mainStages_concavePoints, 2, *data);
	for (int i = 0; i < points.size(); ++i) {
		auto pixel = ui.drar_mainStages_concavePoints->LogicToPixelCoords(QPointF(m.V[points[i]]->pos->x, m.V[points[i]]->pos->y), false);
		ui.drar_mainStages_concavePoints->drawEllipse(pixel, 3, false, Qt::red);
	}

	ui.progressBar->setValue(7);

	if (points.size() > 6) {
		ui.progressBar->setVisible(false);
		ui.btn_convexPartitionStages->setVisible(false);
		ui.btn_allConcavePartitions->setVisible(false);
		QMessageBox::critical(this, "", "Polygon cannot contain more than 6 concave points. Your polygon contains - " + QString::number(points.size()) + ".");
		return;
	}

	auto meshesRef = *m.splitToConvexPolygons(points);
	ui.progressBar->setValue(17);
	auto meshesRefCharacteristics = std::vector<ConvexPartitionCharacteristics*>();
	auto optimalIndex = Mesh::getOptimalNumOfMeshWithMaxSquareForSplit(&meshesRef, minPartArea, maxPartArea, &meshesRefCharacteristics);
	optimal = meshesRef[optimalIndex];
	optimalCharacteristics = meshesRefCharacteristics[optimalIndex];
	ui.progressBar->setValue(20);

	convexPartitionsMeshes.clear();
	for (int i = 0; i < meshesRef.size() && meshesRef.size() != 1; ++i) {
		convexPartitionsMeshes.push_back({ meshesRef[i], meshesRefCharacteristics[i] });
	}

	currentConvexPartitionsMesh = -1;
	ui.lbl_partitionToConvexParts_optimal->setText("Optimal partition - " + QString::number(optimalIndex + 1));
	ui.lbl_info_nonDividedArea_optimal->setText(QString::number(optimalCharacteristics->getAreaOfNotSplittedParts()));
	ui.lbl_info_notDividedPercent_optimal->setText(QString::number(optimalCharacteristics->getPercentageOfNotSplittedParts()) + "%");
	ui.lbl_info_optimisationFuncValue_optimal->setText(QString::number(optimalCharacteristics->getOptimizationFuncValue()));
	ui.lbl_info_nonDividedArea_optimal_2->setText(QString::number(optimalCharacteristics->getAreaOfNotSplittedParts()));
	ui.lbl_info_notDividedPercent_optimal_2->setText(QString::number(optimalCharacteristics->getPercentageOfNotSplittedParts()) + "%");
	ui.lbl_info_optimisationFuncValue_optimal_2->setText(QString::number(optimalCharacteristics->getOptimizationFuncValue()));
	ui.frm_algoResults->setVisible(true);
	tryDrawNextConvexPartitionMesh();

	ui.qrb_processingResults->setEnabled(true);

	auto data2 = optimal->convertToPolygonData();
	drawPolygonData(ui.drar_mainStages_convexParts, 2, data2);
	drawPolygonData(ui.drar_partitionToConvex_optimal, 2, data2);

	splitConvexParts(r);
}

void ApplicationGUI::splitConvexParts(Rotation r) {
	partPartitions.clear();
	auto meshes = std::vector<Mesh*>();
	auto nonSplittedMeshes = std::vector<Mesh*>();

	auto progressBarStep = (int)floor(80.0 / optimal->F.size());

	for (auto face : optimal->F) {
		auto mesh = Mesh::createFromFace(face);

		if (Mesh::canBeSplittedToEqualSquareParts(face, minPartArea, maxPartArea) == false) {
			nonSplittedMeshes.push_back(mesh);
			continue;
		}

		auto stage1 = mesh->convertToPolygonData();

		auto partArea = Mesh::getPartAreaToSplitFace(face, minPartArea, maxPartArea);

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

	if (partPartitions.size() == 0) {
		ui.btn_convexPartitionStages->setVisible(false);
	}

	for (auto mesh : nonSplittedMeshes) {
		auto data = mesh->convertToPolygonData();
		drawPolygonData(ui.drar_mainStages_optimal, 2, data, false);

		r.tryRotateFigureBack(&data.vertex_x, &data.vertex_y);
		drawPolygonMesh(ui.polygonDrawingArea, 0, data.vertex_x, data.vertex_y, data.edges, data.faces, false);
	}

	for (auto mesh : meshes) {
		auto data = mesh->convertToPolygonData();
		drawPolygonData(ui.drar_mainStages_optimal, 2, data, false);

		r.tryRotateFigureBack(&data.vertex_x, &data.vertex_y);
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

std::vector<int> ApplicationGUI::getConcavePoints() {
	auto data = convertPolygonToPolygonData();

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);

	return m.findConcavePoints();
}

void ApplicationGUI::saveResults()
{
	QString directory = QFileDialog::getExistingDirectory(this, "Open Directory", "/home", QFileDialog::ShowDirsOnly);

	if (directory == "")
		return;

	if (!ui.chb_isSave_mainSteps->isChecked() && !ui.chb_isSave_partitionIntoConvexParts->isChecked() && !ui.chb_isSave_convexPartPartition->isChecked()) {
		QMessageBox::warning(this, "", "To save the results of the algorithm, select at least one of the possible save options");
		return;
	}

	if (ui.chb_isSave_partitionIntoConvexParts->isChecked() && convexPartitionsMeshes.size() > 100) {
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "",
			"This polygon has " + QString::number(convexPartitionsMeshes.size()) + " possible divisions into convex parts. It may take a long time to save them. Are you sure you want to continue?",
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::No) {
			return;
		}
	}

	int count = 1;
	directory += "/Polygon partition results_";

	while (QDir(directory + QString::number(count)).exists()) {
		count++;
	}

	directory += QString::number(count);

	QDir().mkdir(directory);

	QString subDir = "";

	int pgb_partitionIntoConvexParts = ui.chb_isSave_partitionIntoConvexParts->isChecked() ? 90 : 0;
	int pgb_mainStep = (100 - pgb_partitionIntoConvexParts) / 2;
	int pgb_partPartition = pgb_mainStep;

	ui.progressBar->setValue(0);
	ui.progressBar->setVisible(true);

	if (ui.chb_isSave_mainSteps->isChecked()) {
		ui.progressBar->setVisible(false);
		progressBarStep = pgb_mainStep;
		saveMainStepsResults(directory);
		ui.progressBar->setVisible(true);
	}

	if (ui.chb_isSave_partitionIntoConvexParts->isChecked() && convexPartitionsMeshes.size() > 0) {

		if (pgb_partitionIntoConvexParts >= convexPartitionsMeshes.size() && pgb_partitionIntoConvexParts != 0) {
			pgb_partitionIntoConvexParts = 80 / convexPartitionsMeshes.size();
			progressBarStepFreq = 1;
		}
		else if (pgb_partitionIntoConvexParts < convexPartitionsMeshes.size() && pgb_partitionIntoConvexParts != 0) {
			pgb_partitionIntoConvexParts = 1;
			progressBarStepFreq = convexPartitionsMeshes.size() / 80;
		}

		progressBarStep = pgb_partitionIntoConvexParts;
		subDir = directory + "/Partition into convex parts";
		QDir().mkdir(subDir);
		savePartitionIntoConvexPartsResults(subDir);
	}

	if (ui.chb_isSave_convexPartPartition->isChecked() && partPartitions.size() > 0) {
		progressBarStep = pgb_partPartition;
		subDir = directory + "/Convex part partitions";
		QDir().mkdir(subDir);
		saveConvexPartPartitionsResults(subDir);
	}

	ui.progressBar->setVisible(false);

	QMessageBox::information(this, "", "Results saved");
}

void ApplicationGUI::saveMainStepsResults(QString folder) {
#pragma region app screenshot
	setWidgetToScreenShot(ui.mainAlgoImages);

	QPixmap pixmap(this->size());
	this->render(&pixmap, QPoint(), QRegion(this->rect()));

	pixmap.toImage().save(folder + "/1. results.png");

	setWidgetToOriginalStateAfterScreenShot();
#pragma endregion

#pragma region original polygon image
	auto copy = ui.polygonDrawingArea->copyDrawingArea();

	auto data = convertPolygonToPolygonData();
	drawPolygonData(copy, 4, *data);
	copy->saveImage(folder + "/2. original.png");

	copy->close();
	copy->parentWidget()->setParent(NULL);
	copy->deleteLater();
#pragma endregion copy polygonDrawingArea, draw original polygon, delete drawingArea

#pragma region splitted polygon image
	ui.polygonDrawingArea->saveImage(folder + "/3. splitted.png");
#pragma endregion

#pragma region original polygon as text
	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(*data);
	auto meshAsText = m.convertToString();

	std::ofstream out(folder.toStdString() + "/4. original.txt");
	out << meshAsText;
	out.close();
#pragma endregion

#pragma region results as text
	auto resultsAsText = QString();
	resultsAsText.append("Algorithm results\n");
	resultsAsText.append("Unused area: " + ui.lbl_info_nonDividedArea_optimal->text() + "\n");
	resultsAsText.append("Percentage of unused area: " + ui.lbl_info_notDividedPercent_optimal->text() + "\n");
	resultsAsText.append("Optimization function value: " + ui.lbl_info_optimisationFuncValue_optimal->text() + "\n");

	std::ofstream out2(folder.toStdString() + "/5. results.txt");
	out2 << resultsAsText.toStdString();
	out2.close();
#pragma endregion

	ui.progressBar->setValue(ui.progressBar->value() + progressBarStep);
}

void ApplicationGUI::savePartitionIntoConvexPartsResults(QString folder) {
#pragma region app screenshot
	setWidgetToScreenShot(ui.convexPartitions);

	QPixmap pixmap(ui.convexPartitions->size());

	int freq = 0;

	while (tryDrawNextConvexPartitionMesh()) {
		ui.convexPartitions->render(&pixmap, QPoint(), ui.convexPartitions->rect());

		pixmap.toImage().save(folder + "/" + QString::number(currentConvexPartitionsMesh + 1) + ".png");

		freq++;

		if (freq == progressBarStepFreq) {
			freq = 0;
			ui.progressBar->setValue(ui.progressBar->value() + progressBarStep);
		}
	}

	setWidgetToOriginalStateAfterScreenShot();
#pragma endregion save all convex partition results
}

void ApplicationGUI::saveConvexPartPartitionsResults(QString folder) {
#pragma region app screenshot
	setWidgetToScreenShot(ui.convexPartsPartition);

	QPixmap pixmap(ui.convexPartsPartition->size());

	while (tryDrawNextPartPartition()) {
		ui.convexPartsPartition->render(&pixmap, QPoint(), ui.convexPartsPartition->rect());

		pixmap.toImage().save(folder + "/" + QString::number(currentPartPartition + 1) + ".png");
	}

	setWidgetToOriginalStateAfterScreenShot();
#pragma endregion save all convex part partition results

	ui.progressBar->setValue(ui.progressBar->value() + progressBarStep);
}

bool ApplicationGUI::tryDrawNextConvexPartitionMesh() {
	if (currentConvexPartitionsMesh + 1 == convexPartitionsMeshes.size() || currentConvexPartitionsMesh + 1 < 0)
		return false;

	currentConvexPartitionsMesh++;
	return tryDrawCurPartConvexPartitionMesh();
}

bool ApplicationGUI::tryDrawPrevConvexPartitionMesh() {
	if (currentConvexPartitionsMesh - 1 < 0 || currentConvexPartitionsMesh - 1 >= convexPartitionsMeshes.size())
		return false;

	currentConvexPartitionsMesh--;
	return tryDrawCurPartConvexPartitionMesh();
}

bool ApplicationGUI::tryDrawCurPartConvexPartitionMesh() {
	if (currentConvexPartitionsMesh < 0 || currentConvexPartitionsMesh >= convexPartitionsMeshes.size())
		return false;

	auto curMesh = convexPartitionsMeshes[currentConvexPartitionsMesh].first;
	auto curCharacteristics = convexPartitionsMeshes[currentConvexPartitionsMesh].second;

	auto data = curMesh->convertToPolygonData();
	drawPolygonMesh(ui.drar_partitionToConvex_current, 2, data.vertex_x, data.vertex_y, data.edges, data.faces);

	ui.lbl_info_nonDividedArea->setText(QString::number(curCharacteristics->getAreaOfNotSplittedParts()));
	ui.lbl_info_notDividedPercent->setText(QString::number(curCharacteristics->getPercentageOfNotSplittedParts()) + "%");
	ui.lbl_info_optimisationFuncValue->setText(QString::number(curCharacteristics->getOptimizationFuncValue()));

	ui.lbl_numOfConvexPartitions->setText("Partition " + QString::number(currentConvexPartitionsMesh + 1) + " / " + QString::number(convexPartitionsMeshes.size()));

	return true;
}

bool ApplicationGUI::tryDrawNextPartPartition() {
	if (currentPartPartition + 1 == partPartitions.size() || currentPartPartition + 1 < 0)
		return false;

	currentPartPartition++;
	return tryDrawCurPartPartition();
}

bool ApplicationGUI::tryDrawPrevPartPartition() {
	if (currentPartPartition - 1 < 0 || currentPartPartition - 1 >= partPartitions.size())
		return false;

	currentPartPartition--;
	return tryDrawCurPartPartition();
}

bool ApplicationGUI::tryDrawCurPartPartition() {
	if (currentPartPartition >= partPartitions.size() || currentPartPartition < 0)
		return false;

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

	ui.lbl_numOfConvexPart->setText("Part " + QString::number(currentPartPartition + 1) + " / " + QString::number(partPartitions.size()));

	return true;
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

void ApplicationGUI::setWidgetToScreenShot(QWidget* activeBottomWidget) {
	ui.tabWidget->setCurrentIndex(0);

	mouseCoordinates->setVisible(false);

	bottomGroupBoxesVisibility = { ui.mainAlgoImages->isVisible(), ui.convexPartitions->isVisible(), ui.convexPartsPartition->isVisible() };
	currentPartitionsValues = { currentPartPartition, currentConvexPartitionsMesh };

	ui.btn_goToNextPartition->setVisible(false);
	ui.btn_goToPrevPartition->setVisible(false);
	ui.btn_goToNextConvexPart->setVisible(false);
	ui.btn_goToPrevConvexPart->setVisible(false);
	ui.btn_backToMainAlgoStages_fromConvexPartDetails->setVisible(false);
	ui.btn_backToMainAlgoStages_fromConvexPartitions->setVisible(false);

	if (activeBottomWidget == ui.mainAlgoImages) {
		ui.mainAlgoImages->setVisible(true);
		ui.convexPartitions->setVisible(false);
		ui.convexPartsPartition->setVisible(false);
	}
	else if (activeBottomWidget == ui.convexPartitions) {
		ui.mainAlgoImages->setVisible(false);
		ui.convexPartitions->setVisible(true);
		ui.convexPartsPartition->setVisible(false);

		currentConvexPartitionsMesh = -1;
	}
	else if (activeBottomWidget == ui.convexPartsPartition) {
		ui.mainAlgoImages->setVisible(false);
		ui.convexPartitions->setVisible(false);
		ui.convexPartsPartition->setVisible(true);

		currentPartPartition = -1;
	}
}

void ApplicationGUI::setWidgetToOriginalStateAfterScreenShot() {
	ui.tabWidget->setCurrentIndex(1);

	ui.btn_goToNextPartition->setVisible(true);
	ui.btn_goToPrevPartition->setVisible(true);
	ui.btn_goToNextConvexPart->setVisible(true);
	ui.btn_goToPrevConvexPart->setVisible(true);
	ui.btn_backToMainAlgoStages_fromConvexPartDetails->setVisible(true);
	ui.btn_backToMainAlgoStages_fromConvexPartitions->setVisible(true);

	ui.mainAlgoImages->setVisible(std::get<0>(bottomGroupBoxesVisibility));
	ui.convexPartitions->setVisible(std::get<1>(bottomGroupBoxesVisibility));
	ui.convexPartsPartition->setVisible(std::get<2>(bottomGroupBoxesVisibility));

	currentPartPartition = std::get<0>(currentPartitionsValues);
	currentConvexPartitionsMesh = std::get<1>(currentPartitionsValues);

	tryDrawCurPartPartition();
	tryDrawCurPartConvexPartitionMesh();
}