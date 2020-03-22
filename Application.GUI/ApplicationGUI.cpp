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

	connect(ui.btn_saveResults, &QPushButton::clicked, this, &ApplicationGUI::btn_saveResults);
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
			setSliderLabelsPosition();
		});

	connect(ui.sld_areaOfPart, &RangeSlider::maximumPositionChanged, this,
		[=](int max) {
			ui.sld_maxPosLabel->setText(QString::number(max / SLD_SCALE));
			setSliderLabelsPosition();
		});
#pragma endregion

#pragma region Mesh properties
	ui.meshAngleDrawingArea->setGridVisibility(false);
	connect(ui.btn_apply_angle, &QPushButton::clicked, this, &ApplicationGUI::btn_apply_angle_clicked);
	connect(ui.btn_reset_angle, &QPushButton::clicked, this, &ApplicationGUI::btn_reset_angle_clicked);
#pragma endregion
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
		}
	}
	else if (grb_name == "areaProperties") {
		if (isNext) {
			activeGroupBox = "meshProperties";

			ui.btn_reset->setEnabled(true);
			ui.btn_apply->setEnabled(false);
			ui.spin_numOfSides->setEnabled(false);

			ui.lbl_polygonArea->setText(QString::number(polygonArea));
			ui.sld_areaOfPart->setMaximum(polygonArea);
			ui.sld_areaOfPart->setEnabled(true);
			sliderRangeChanged();

			ui.spin_meshAngle->setEnabled(true);
			ui.btn_apply_angle->setEnabled(true);
			ui.btn_reset_angle->setEnabled(false);
			anglePoints = std::make_pair(nullptr, nullptr);
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
		}
	}
}

void ApplicationGUI::addPointToPolygon(int x, int y)
{
	auto logicPoint = ui.polygonDrawingArea->PixelToLogicCoords(QPoint(x, y), false);
	auto pixelPoint = ui.polygonDrawingArea->LogicToPixelCoords(logicPoint, false);

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

		//convertToMesh();
		//convertToMeshTEST();
	}
}

void ApplicationGUI::convertToMeshTEST() {
	vectorD* x = new vectorD{ 40, 14, 10, 12, 20, 29, 37 };
	vectorD* y = new vectorD{ 45, 30, 21, 13, 10, 12, 29 };
	vectorI* edges = new vectorI{ 0, 1, 2, 3, 4, 5, 6 };

	//Rotation r = Rotation(ui.spin_meshAngle->value(), RotationDirection::Right);
	//r.tryRotateFigure(x, y);

	PolygonData rotatedData = PolygonData();
	rotatedData.vertex_x = *x;
	rotatedData.vertex_y = *y;
	rotatedData.tryAddFace(edges->size(), *edges);

	auto area = 577.0;
	auto parts = 5;

	drawPolygonMesh(ui.drawingArea_test, 2, rotatedData.vertex_x, rotatedData.vertex_y, rotatedData.edges, rotatedData.faces);

	Mesh m = Mesh();
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(rotatedData, area);
	m.F[0]->area = area;

	m.splitByVerticalGrid();

	auto SplittedByGridData = m.convertToPolygonData();
	drawPolygonMesh(ui.drawingArea_test_2, 2, SplittedByGridData.vertex_x, SplittedByGridData.vertex_y, SplittedByGridData.edges, SplittedByGridData.faces);

	auto splitedEdges = m.splitFaces(area / parts);

	auto horisontalSplittedData = m.convertToPolygonData();
	drawPolygonMesh(ui.drawingArea_test_3, 2, horisontalSplittedData.vertex_x, horisontalSplittedData.vertex_y, horisontalSplittedData.edges, horisontalSplittedData.faces);

	//r.tryRrotateTheFigureBack(&horisontalSplittedData.vertex_x, &horisontalSplittedData.vertex_y);
	drawPolygonMesh(ui.drawingArea_test_4, 2, horisontalSplittedData.vertex_x, horisontalSplittedData.vertex_y, horisontalSplittedData.edges, horisontalSplittedData.faces);

	auto vert = m.removeEdge(m.E[37]);
	m.removeVertex(vert[0]);
	m.removeVertex(vert[1]);
	vert = m.removeEdge(m.E[33]);
	m.removeVertex(vert[0]);
	m.removeVertex(vert[1]);
	vert = m.removeEdge(m.E[25]);
	m.removeVertex(vert[0]);
	m.removeVertex(vert[1]);
	auto testData = m.convertToPolygonData();

	//x = 22.690840468013054
	//auto y = 22.7;
	m.splitEdge(m.E[1], new Point{ 10.8, m.findYByXAndTwoVertixes(m.V[1], m.V[2], 10.8) });
	drawPolygonMesh(ui.polygonDrawingArea, 2, testData.vertex_x, testData.vertex_y, testData.edges, testData.faces);
}

bool ApplicationGUI::isInMeshAngleDrawingAreaBounds(int x, int y)
{
	return activeGroupBox == "meshProperties" && ui.meshAngleDrawingArea;
}

void ApplicationGUI::mouseMoveEvent(QMouseEvent* event)
{
	mouseCoordinates->setVisible(false);
}

void ApplicationGUI::createDefaultPolygonMesh()
{
	vectorD* x = new vectorD();
	vectorD* y = new vectorD();

	vectorI faces;
	vectorI edges;

	x->push_back(10);
	x->push_back(30);
	x->push_back(20);
	x->push_back(10);
	x->push_back(20);
	x->push_back(30);
	x->push_back(40);
	x->push_back(40);

	y->push_back(10);
	y->push_back(10);
	y->push_back(20);
	y->push_back(30);
	y->push_back(40);
	y->push_back(30);
	y->push_back(30);
	y->push_back(20);
	/**x << 10, 30, 20, 10, 20, 30, 40, 40;
	*y << 10, 10, 20, 30, 40, 30, 30, 20;*/
	//edges << 0, 1, 2, 3, 1, 2, 5, 7, 2, 3, 4, 5, 5, 6, 7;
	//faces << 0, 4, 8, 12, 15;

	faces.push_back(0);
	faces.push_back(4);
	faces.push_back(8);
	faces.push_back(12);
	faces.push_back(15);

	edges.push_back(0);
	edges.push_back(1);
	edges.push_back(2);
	edges.push_back(3);
	edges.push_back(1);
	edges.push_back(2);
	edges.push_back(5);
	edges.push_back(7);
	edges.push_back(2);
	edges.push_back(3);
	edges.push_back(4);
	edges.push_back(5);
	edges.push_back(5);
	edges.push_back(6);
	edges.push_back(7);

	drawPolygonMesh(ui.drawingArea_test, 3, *x, *y, edges, faces);

	Rotation rotation = Rotation(ui.spin_meshAngle->value(), Right);
	rotation.tryRotateFigure(x, y);

	//// in spin box central aligment!!!
	drawPolygonMesh(ui.polygonDrawingArea, 10, *x, *y, edges, faces);
}

void ApplicationGUI::testMethod() {
	Mesh m = Mesh();
	//1. Allocate space for mesh elements.
	m.E.resize(12);
	m.V.resize(6);
	m.F.resize(4);
	for (int i = 0; i < 12; i++)
		m.E[i] = new Edge;
	for (int i = 0; i < 6; i++)
		m.V[i] = new Vertex;
	for (int i = 0; i < 4; i++)
		m.F[i] = new Face;
	//Build Tetrahedron continued. . .
	//2. Connect all edge ptrs
	//v f prev next sym
	m.setEdge(m.E[0], m.V[0], m.F[0], m.E[2], m.E[1], m.E[11]);
	m.setEdge(m.E[1], m.V[1], m.F[0], m.E[0], m.E[2], nullptr);
	m.setEdge(m.E[2], m.V[4], m.F[0], m.E[1], m.E[0], nullptr);

	m.setEdge(m.E[3], m.V[1], m.F[1], m.E[5], m.E[4], m.E[10]);
	m.setEdge(m.E[4], m.V[2], m.F[1], m.E[3], m.E[5], nullptr);
	m.setEdge(m.E[5], m.V[5], m.F[1], m.E[4], m.E[3], nullptr);

	m.setEdge(m.E[6], m.V[0], m.F[2], m.E[8], m.E[7], nullptr);
	m.setEdge(m.E[7], m.V[3], m.F[2], m.E[6], m.E[8], nullptr);
	m.setEdge(m.E[8], m.V[2], m.F[2], m.E[7], m.E[6], m.E[9]);

	m.setEdge(m.E[9], m.V[0], m.F[3], m.E[11], m.E[10], m.E[8]);
	m.setEdge(m.E[10], m.V[2], m.F[3], m.E[9], m.E[11], m.E[3]);
	m.setEdge(m.E[11], m.V[1], m.F[3], m.E[10], m.E[9], m.E[0]);

	//3. Connect each face’s edge ptr
	m.F[0]->e = m.E[0];
	m.F[1]->e = m.E[3];
	m.F[2]->e = m.E[7];
	m.F[3]->e = m.E[9];

	//Build Tetrahedron continued. . .
		//4. Connect each vertex’s edge ptr
	m.V[0]->e = m.E[0];
	m.V[1]->e = m.E[1];
	m.V[2]->e = m.E[8];
	m.V[3]->e = m.E[7];
	m.V[4]->e = m.E[2];
	m.V[5]->e = m.E[5];

	//5. Set other attribute information
	m.V[0]->pos = new Point{ 10, 10 };
	m.V[1]->pos = new Point{ 15, 15 };
	m.V[2]->pos = new Point{ 20, 10 };
	m.V[3]->pos = new Point{ 15, 5 };
	m.V[4]->pos = new Point{ 5, 15 };
	m.V[5]->pos = new Point{ 25, 15 };

	m.V[0]->numOfVertex = 0;
	m.V[1]->numOfVertex = 1;
	m.V[2]->numOfVertex = 2;
	m.V[3]->numOfVertex = 3;
	m.V[4]->numOfVertex = 4;
	m.V[5]->numOfVertex = 5;
	//May also include colors, normals, texture coordinates, etc. . .*/* /

	//auto data = m.convertToPolygonData();

	//drawPolygonMesh(ui.drawingArea_test, 2, data.vertex_x, data.vertex_y, data.edges, data.faces);

	//m.connectVertexes(m.splitEdge(m.E[11], new Point{ 12.5, 12.5 }), m.splitEdge(m.E[2], new Point{ 7.5, 12.5 }));

	//m.connectVertexes(m.splitEdge(m.E[6], new Point{ 12.5, 7.5 }), m.splitEdge(m.E[7], new Point{ 17.5, 7.5 }));

	//auto data1 = m.convertToPolygonData();

	//drawPolygonMesh(ui.polygonDrawingArea, 2, data1.vertex_x, data1.vertex_y, data1.edges, data1.faces);

		///4 Triangle - dont work
	/*vectorD* x = new vectorD();
	vectorD* y = new vectorD();

	vectorI faces;
	vectorI edges;

	x->push_back(10);
	x->push_back(15);
	x->push_back(20);
	x->push_back(15);
	x->push_back(5);
	x->push_back(25);

	y->push_back(10);
	y->push_back(15);
	y->push_back(10);
	y->push_back(5);
	y->push_back(15);
	y->push_back(15);


	faces.push_back(0);
	faces.push_back(3);
	faces.push_back(6);
	faces.push_back(9);
	faces.push_back(12);

	edges.push_back(0);
	edges.push_back(1);
	edges.push_back(4);
	edges.push_back(0);
	edges.push_back(1);
	edges.push_back(2);
	edges.push_back(2);
	edges.push_back(1);
	edges.push_back(5);
	edges.push_back(0);
	edges.push_back(2);
	edges.push_back(3);

	PolygonData data = PolygonData();
	data.edges = edges;
	data.faces = faces;
	data.vertex_x = *x;
	data.vertex_y = *y;

	drawPolygonMesh(ui.drawingArea_test, 2, *x, *y, edges, faces);

	Mesh m = Mesh();
	m.convertFromPolygonData(data);*/

	//// 1 right traversal polygon
	/*
	polygon = new a::Polygon(4);
	polygon->tryAddPoint({ 20, 20 });
	polygon->tryAddPoint({ 10, 20 });
	polygon->tryAddPoint({ 10, 10 });
	polygon->tryAddPoint({ 20, 10 });
	*/

	//add and delete points
	/*
	//vectorD x = vectorD(polygon->getNumOfPoints());
	//vectorD y = vectorD(polygon->getNumOfPoints());
	//vectorI edges = vectorI(polygon->getNumOfPoints());


	//for (int i = 0; i < polygon->getNumOfPoints(); ++i) {
	//	auto point = polygon->getPointAt(i);
	//	x[i] = point.x;
	//	y[i] = point.y;
	//	edges[i] = i;
	//}

	//PolygonData data = PolygonData();
	//data.vertex_x = x;
	//data.vertex_y = y;
	//data.tryAddFace(edges.size(), edges);
	//drawPolygonMesh(ui.drawingArea_test, 2, data.vertex_x, data.vertex_y, data.edges, data.faces);

	//Mesh m = Mesh();
	//m.convertFromPolygonDataTEST(data);
	//m.F[0]->area = polygonArea;


	////m.splitFaces(polygonArea / ui.spin_numOfParts->value());

	//auto vToDelete = m.splitEdge(m.E[0], new Point{ 50, 50 });

	//auto data1 = m.convertToPolygonData();
	//drawPolygonMesh(ui.drawingArea_test_2, 2, data1.vertex_x, data1.vertex_y, data1.edges, data1.faces);


	//m.removeVertex(vToDelete);
	//auto data2 = m.convertToPolygonData();
	//drawPolygonMesh(ui.polygonDrawingArea, 5, data2.vertex_x, data2.vertex_y, data2.edges, data2.faces);
	*/

	/////WORKS WITH DRAWING POLYGON
	//vectorD x = vectorD(polygon->getNumOfPoints());
	//vectorD y = vectorD(polygon->getNumOfPoints());
	//vectorI edges = vectorI(polygon->getNumOfPoints());


	//for (int i = 0; i < polygon->getNumOfPoints(); ++i) {
	//	auto point = polygon->getPointAt(i);
	//	x[i] = point.x;
	//	y[i] = point.y;
	//	edges[i] = i;
	//}

	//PolygonData data = PolygonData();
	//data.vertex_x = x;
	//data.vertex_y = y;
	//data.tryAddFace(edges.size(), edges);
	//drawPolygonMesh(ui.drawingArea_test, 2, data.vertex_x, data.vertex_y, data.edges, data.faces);

	//Mesh m = Mesh();
	//m.convertFromPolygonDataOfConvexLeftTraversalPolygon(data, polygonArea);
	//m.F[0]->area = polygonArea;
	//
	//
	////m.splitFaces(polygonArea / ui.spin_numOfParts->value());
	//auto edgeToDelete = m.connectVertexes(m.splitEdge(m.E[0], new Point{ 50, 50 }), m.splitEdge(m.E[1], new Point{20, 20}), m.F[0]);
	//

	//auto data1 = m.convertToPolygonData();
	//drawPolygonMesh(ui.drawingArea_test_2, 2, data1.vertex_x, data1.vertex_y, data1.edges, data1.faces);


	//auto vertexesToDelete = m.removeEdge(edgeToDelete);
	//for (int i = 0; i < vertexesToDelete.size(); ++i) {
	//	m.removeVertex(vertexesToDelete[i]);
	//}
	//
	//auto data2 = m.convertToPolygonData();
	//drawPolygonMesh(ui.polygonDrawingArea, 5, data2.vertex_x, data2.vertex_y, data2.edges, data2.faces);


//TEST WITH SPLIT BY GRID
	/*vectorD x = { 6, 6, 4, 2, 1, 1, 1, 3 };
	vectorD y = { 1, 3, 5, 5, 4, 3, 2, 1 };
	vectorI edges = { 0, 1, 2, 3, 4, 5, 6, 7 };

	PolygonData* data = new PolygonData();
	data->vertex_x = x;
	data->vertex_y = y;
	data->tryAddFace(edges.size(), edges);
	drawPolygonMesh(ui.drawingArea_test, 2, data->vertex_x, data->vertex_y, data->edges, data->faces);

	Mesh* m = new Mesh();
	m->convertFromPolygonDataOfConvexLeftTraversalPolygon(*data, 16.5);
	m->splitByVerticalGrid();

	auto data1 = m->convertToPolygonData();
	drawPolygonMesh(ui.drawingArea_test_2, 2, data1.vertex_x, data1.vertex_y, data1.edges, data1.faces);*/

	//WORKED POLYGON FROM UI WITHOUT ROTATE

		/*vectorD x = vectorD(polygon->getNumOfPoints());
		vectorD y = vectorD(polygon->getNumOfPoints());
		vectorI edges = vectorI(polygon->getNumOfPoints());


		for (int i = 0; i < polygon->getNumOfPoints(); ++i) {
			auto point = polygon->getPointAt(i);
			x[i] = point.x;
			y[i] = point.y;
			edges[i] = i;
		}

		PolygonData data = PolygonData();
		data.vertex_x = x;
		data.vertex_y = y;
		data.tryAddFace(edges.size(), edges);

		Mesh m = Mesh();
		m.convertFromPolygonDataOfConvexLeftTraversalPolygon(data, polygonArea);
		m.F[0]->area = polygonArea;

		m.splitByVerticalGrid();

		auto data1 = m.convertToPolygonData();
		drawPolygonMesh(ui.drawingArea_test, 2, data1.vertex_x, data1.vertex_y, data1.edges, data1.faces);

		m.splitFaces(polygonArea / ui.spin_numOfParts->value());

		auto data2 = m.convertToPolygonData();
		drawPolygonMesh(ui.drawingArea_test_2, 2, data2.vertex_x, data2.vertex_y, data2.edges, data2.faces);*/
}

void ApplicationGUI::convertToMesh()
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
	m.convertFromPolygonDataOfConvexLeftTraversalPolygon(rotatedData, polygonArea);
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


	ui.sld_minPosLabel->setGeometry(10 + ((double)ui.sld_areaOfPart->minimumPosition() / ui.sld_areaOfPart->maximum() * w), 40, 30, 16);


	QRect d = ui.sld_maxPosLabel->geometry();
	QRect droppedRect = QRect(d.left(), 40, d.width(), d.height());

	if (droppedRect.intersects(ui.sld_minPosLabel->geometry()))
	{
		ui.sld_maxPosLabel->setGeometry(((double)ui.sld_areaOfPart->maximumPosition() / ui.sld_areaOfPart->maximum() * w) + 10, 0, 30, 16);
	}
	else {
		ui.sld_maxPosLabel->setGeometry(((double)ui.sld_areaOfPart->maximumPosition() / ui.sld_areaOfPart->maximum() * w) + 10, 40, 30, 16);
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

void ApplicationGUI::btn_saveResults(bool checked)
{
	ui.polygonDrawingArea->saveImage("Polygon.png", 1);
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
	}
	else {
		ui.sld_minPosLabel->setText(QString::number(min / SLD_SCALE));
		ui.sld_maxPosLabel->setText(QString::number(max / SLD_SCALE));
	}
}

void ApplicationGUI::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {

		auto point = event->pos();

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