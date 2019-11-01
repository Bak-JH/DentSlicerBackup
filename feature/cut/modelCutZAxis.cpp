#include "modelCutZAxis.h"
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;
Hix::Features::Cut::ZAxisCutTask::ZAxisCutTask(const Mesh* originalMesh, Mesh* botMesh, Mesh* topMesh, float z, bool fillCutSurface):
	_origMesh(originalMesh),_bottomMesh(botMesh), _topMesh(topMesh), _cuttingPlane(z), _fillCuttingSurface(fillCutSurface)
{
	divideTriangles();
	generateCutContour();
	if (fillCutSurface)
	{
		generateCaps();
	}
	for (auto& contour : _contours)
	{
		for (auto& seg : contour.segments)
		{
			fillOverlap(seg);
		}
	}


}

tf::Taskflow& Hix::Features::Cut::ZAxisCutTask::getFlow()
{
	// TODO: insert return statement here
	return _flow;
}

void Hix::Features::Cut::ZAxisCutTask::divideTriangles()
{
	// Uniform Slicing O(n)
	auto& faces = _origMesh->getFaces();
	for (auto mf = faces.cbegin(); mf != faces.cend(); ++mf)
	{
		auto sortedZ = mf.sortZ();
		float z_min = sortedZ[0];
		float z_mid = sortedZ[1];
		float z_max = sortedZ[2];


		if (z_max < _cuttingPlane)
		{
			_botFaces.insert(mf);
		}
		else if (z_min > _cuttingPlane)
		{
			_topFaces.insert(mf);
		}
		else
		{
			_overlapFaces.insert(mf);
		}
	}
	//add non cut faces to each halves
	for (auto& each : _botFaces)
	{
		_bottomMesh->addFace(each);
	}
	for (auto& each : _topFaces)
	{
		_topMesh->addFace(each);
	}
}

void Hix::Features::Cut::ZAxisCutTask::generateCutContour()
{
	ContourBuilder contourBuilder(_origMesh, _overlapFaces, _cuttingPlane);
	_contours = contourBuilder.buildContours();
}

void Hix::Features::Cut::ZAxisCutTask::generateCaps()
{
	//need to map float contours with int path contours to nullify float error
	std::unordered_map<IntPoint, QVector2D> fIntPtMap;
	std::vector<Path> pathCont;
	pathCont.reserve(_contours.size());
	for (auto& fContour : _contours)
	{
		std::vector<QVector2D> floatPath;
		auto intPath = pathCont.emplace_back(fContour.toPath(floatPath));
		//map int values to float values
		for (size_t i = 0; i < floatPath.size(); ++i)
		{
			fIntPtMap[intPath[i]] = floatPath[i];
		}
	}
	//clipping;

	Clipper clpr;
	for (size_t idx = 0; idx < _contours.size(); idx++) { // divide into parallel threads
		clpr.AddPath(pathCont[idx], ptSubject, true);
	}
	ClipperLib::PolyTree polytree;
	clpr.Execute(ctUnion, polytree, pftNonZero, pftNonZero);

	//triangulate
	Hix::Polyclipping::PolytreeCDT cdt(&fIntPtMap, &polytree);
	auto triangulationResult =  cdt.triangulate();

	//add triangles
	for (auto& pair : triangulationResult)
	{
		auto& cap = pair.second;
		for (auto& trig : cap)
		{
			std::array<QVector3D, 3> vertices
			{
				QVector3D(trig[0], _cuttingPlane),
				QVector3D(trig[1], _cuttingPlane),
				QVector3D(trig[2], _cuttingPlane)

			};
			_bottomMesh->addFace(vertices[0], vertices[1], vertices[2]);
			_topMesh->addFace(vertices[2], vertices[1], vertices[0]);
		}
	}
}

void Hix::Features::Cut::ZAxisCutTask::fillOverlap(const Hix::Slicer::ContourSegment& seg)
{
	auto face = seg.face;
	if (!face.initialized())
	{
		return;
	}
	auto mvs = face.meshVertices();
	std::vector<VertexConstItr> highs;
	std::vector<VertexConstItr> lows;
	VertexConstItr med;
	for (size_t i = 0; i < 3; ++i)
	{
		auto& curr = mvs[i];
		if (curr.worldPosition().z() < _cuttingPlane)
		{
			lows.emplace_back(curr);
		}
		else if (curr.worldPosition().z() > _cuttingPlane)
		{
			highs.emplace_back(curr);
		}
		else
		{
			med = curr;
		}
	}
	//two cases, one where two edges cut, or only one edge is cut
	if (med.initialized())
	{
		//one edge and vtx case
		//f->t->h
		//t->f->l
		QVector3D from(seg.from, _cuttingPlane);
		QVector3D to(seg.to, _cuttingPlane);

		_topMesh->addFace(seg.from, seg.to, highs[0].localPosition());
		_bottomMesh->addFace(seg.to, seg.from, lows[0].localPosition());
	}
	else
	{
		//two edges case
		
		Mesh* twoFacesAddedMesh, *oneFaceAddedMesh;
		QVector3D from;
		QVector3D to;
		bool isPyramid = lows.size() > highs.size();
		std::vector<VertexConstItr> majority(std::move(lows));
		std::vector<VertexConstItr> minority(std::move(highs));
		if (isPyramid)
		{
			// /_\ //
			from = QVector3D(seg.from, _cuttingPlane);
			to = QVector3D(seg.to, _cuttingPlane);
			twoFacesAddedMesh = _bottomMesh;
			oneFaceAddedMesh = _topMesh;
		}
		else
		{
			// \_/ //
			from = QVector3D(seg.to, _cuttingPlane);
			to = QVector3D(seg.from, _cuttingPlane);
			majority.swap(minority);
			twoFacesAddedMesh = _topMesh;
			oneFaceAddedMesh = _bottomMesh;
		}
		//need to sort majority to winding order
		HalfEdgeConstItr hintEdge;
		face.getEdgeWithVertices(hintEdge,majority[0], majority[1]);
		majority[0] = hintEdge.from();
		majority[1] = hintEdge.to();

		//face goes in this order, f-> t -> minority
		oneFaceAddedMesh->addFace(from, to, minority[0].localPosition());
		// maj[0]->t->f 
		// maj[0]->maj[1]->t 
		twoFacesAddedMesh->addFace(majority[0].localPosition(), to, from);
		twoFacesAddedMesh->addFace(majority[0].localPosition(), majority[1].localPosition(), to);
	}



}


removePlane();




void GLModel::generatePlane(int type) {

	//generate drawing plane
	_cuttingPlane.reset(new Hix::Features::Cut::DrawingPlane(this));
	_cuttingPlane->enablePlane(true);
	//if flat cut
	if (type == 1)
	{

	}
	else if (type == 2)
	{
		_cuttingPlane->enableDrawing(true);
		//want cutting plane to be over model mesh
		float zOverModel = _mesh->z_max() + 0.1f;
		_cuttingPlane->transform().setTranslation(QVector3D(0, 0, zOverModel));
		qmlManager->getRayCaster().setHoverEnabled(true);
	}
}

void GLModel::removePlane() {
	//freecut disable hovering
	if (cutMode == 2)
	{
		qmlManager->getRayCaster().setHoverEnabled(false);
	}
	_cuttingPlane.reset();
}


void GLModel::cutModeSelected(int type) {

	qDebug() << "cut mode selected1" << type;
	cutMode = type;
	generatePlane(cutMode);
	return;
}

void GLModel::cutFillModeSelected(int type) {
	cutFillMode = type;
	return;
}

void GLModel::getSliderSignal(double value) {
	if (cutActive || shellOffsetActive) {
		if (value == 0.0 || value == 1.8) {
			isFlatcutEdge = true;
		}
		else {
			isFlatcutEdge = false;
		}
		float zlength = _mesh->z_max() - _mesh->z_min();

		_cuttingPlane->transform().setTranslation(QVector3D(0, 0, _mesh->z_min() + value * zlength / 1.8));

	}
	else if (hollowShellActive) {
		// change radius of hollowShellSphere
		hollowShellRadius = value;
		qmlManager->hollowShellSphereMesh->setRadius(hollowShellRadius);

		qDebug() << "getting slider signal: current radius is " << value;
	}
}


void GLModel::modelCut() {

	qmlManager->openProgressPopUp();


	if (cutMode == 1) { // flat cut
		auto lmesh = new Mesh();
		auto rmesh = new Mesh();
		if (this->shellOffsetActive && isFlatcutEdge == true) {
			getSliderSignal(0.0);
		}
		bool fillCuttingSurface = cutFillMode == 2;
		Hix::Features::Cut::ZAxisCutTask task(_mesh, lmesh, rmesh, _cuttingPlane->transform().translation().z(), fillCuttingSurface);
		emit bisectDone(lmesh, rmesh);

	}
	else if (cutMode == 2) { // free cut
		auto cuttingContour = _cuttingPlane->contour();
		if (cuttingContour.size() >= 2) {
			auto lmesh = new Mesh();
			auto rmesh = new Mesh();
			cutAway(lmesh, rmesh, _mesh, cuttingContour, cutFillMode);

			if (lmesh->getFaces().size() == 0 || rmesh->getFaces().size() == 0) {

				qDebug() << "cutting contour selected not cutting";
				qmlManager->setProgress(1);
				cutModeSelected(2); // reset
				return;
			}
			emit bisectDone(lmesh, rmesh);
		}
	}
}