#include "ZAxialCut.h"
#include "../../glmodel.h"
#include "../../qmlmanager.h"
#include "../../common/GTEngine/Include/Mathematics/GteSplitMeshByPlane.h"
#include "feature/addModel.h"
#include "feature/deleteModel.h"
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
using namespace ClipperLib;

namespace Hix
{

	namespace Features
	{
		namespace Cut
		{
			class ZAxialCutImp
			{

			public:
				ZAxialCutImp(GLModel* subject, float cuttingPlane, Mesh*& topMesh, Mesh*& botMesh);
			private:
				void divideTriangles();
				void generateCutContour();
				void generateCaps();
				void fillOverlap(const Hix::Slicer::ContourSegment& seg);
				float _cuttingPlane;
				const Engine3D::Mesh* _origMesh;
				Engine3D::Mesh* _bottomMesh;
				Engine3D::Mesh* _topMesh;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _botFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _overlapFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _topFaces;
				std::vector<Hix::Slicer::Contour> _contours;
			};
		}
	}
}


Hix::Features::Cut::ZAxialCut::ZAxialCut(GLModel* subject, float cuttingPlane) :
	_cuttingPlane(cuttingPlane), top_no(1), bot_no(1)
{
	qDebug() << "zmax: " << subject->recursiveAabb().zMax() << "plane : " << cuttingPlane << "zmin: " << subject->recursiveAabb().zMin();
	if (cuttingPlane >= subject->recursiveAabb().zMax() || cuttingPlane <= subject->recursiveAabb().zMin())
	{
		return;
	}//do listed part first

	doChildrenRecursive(subject, cuttingPlane);
	addFeature(new DeleteModel(subject));
	//qmlManager->deleteModelFile(subject->ID);

	qDebug() << qmlManager->glmodels.size();
}

void Hix::Features::Cut::ZAxialCut::doChildrenRecursive(GLModel* subject, float cuttingPlane)
{
	Mesh* childTopMesh = nullptr;
	Mesh* childBotMesh = nullptr;
	ZAxialCutImp(subject, cuttingPlane, childTopMesh, childBotMesh);
	if(subject->getMesh()->getFaces().empty())
	{ }
	else if (childBotMesh == nullptr || childBotMesh->getFaces().empty())
	{
		auto addTopModel = dynamic_cast<AddModel*>(qmlManager->createAndListModel(subject->getMeshModd(), subject->modelName() + "_top", &subject->transform()));
		addFeature(addTopModel);
		_upperModels.insert(subject);
	}
	else if (childTopMesh == nullptr || childTopMesh->getFaces().empty())
	{
		auto addBotModel = dynamic_cast<AddModel*>(qmlManager->createAndListModel(subject->getMeshModd(), subject->modelName() + "_bot", &subject->transform()));
		addFeature(addBotModel);
		_upperModels.insert(subject);
	}
	else
	{
		auto addTopModel = dynamic_cast<AddModel*>(qmlManager->createAndListModel(childTopMesh, subject->modelName() + "_top", &subject->transform()));
		auto addBotModel = dynamic_cast<AddModel*>(qmlManager->createAndListModel(childBotMesh, subject->modelName() + "_bot", &subject->transform()));
		
		_upperModels.insert(addTopModel->getAddedModel());
		_lowerModels.insert(addBotModel->getAddedModel());
		
		addFeature(addTopModel);
		addFeature(addBotModel);
	}
	for (auto childNode : subject->childNodes())
	{
		auto model = dynamic_cast<GLModel*>(childNode);
		if (model)
		{
			doChildrenRecursive(model, cuttingPlane);
		}
	}
}

std::unordered_set<GLModel*>& Hix::Features::Cut::ZAxialCut::upperModels()
{
	return _upperModels;
}

std::unordered_set<GLModel*>& Hix::Features::Cut::ZAxialCut::lowerModels()
{
	return _lowerModels;
}

Hix::Features::Cut::ZAxialCutImp::ZAxialCutImp(GLModel* subject, float cuttingPlane, Mesh*& topMesh, Mesh*& botMesh) :
	_cuttingPlane(cuttingPlane), _origMesh(subject->getMesh())
{
	//empty model
	_bottomMesh = new Mesh();
	_topMesh = new Mesh();
	divideTriangles();
	//early exit, no cut needed
	if (_origMesh->getFaces().size() == 0)
	{
		//empty node, add empty halves to both sides
		botMesh = _bottomMesh;
		topMesh = _topMesh;
		//subject->clearMesh();
		return;
	}
	else if (_bottomMesh->getFaces().size() == _origMesh->getFaces().size())
	{
		delete _topMesh;
		delete _bottomMesh;
		topMesh = nullptr;
		botMesh = subject->getMeshModd();
		return;
	}
	else if (_topMesh->getFaces().size() == _origMesh->getFaces().size())
	{
		delete _topMesh;
		delete _bottomMesh;
		topMesh = subject->getMeshModd();
		botMesh = nullptr;
		return;
	}
	generateCutContour();
	generateCaps();
	for (auto& contour : _contours)
	{
		for (auto& seg : contour.segments)
		{
			fillOverlap(seg);
		}
	}

	botMesh = _bottomMesh;
	topMesh = _topMesh;
	//subject->clearMesh();
}


void Hix::Features::Cut::ZAxialCutImp::divideTriangles()
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

void Hix::Features::Cut::ZAxialCutImp::generateCutContour()
{
	ContourBuilder contourBuilder(_origMesh, _overlapFaces, _cuttingPlane);
	_contours = contourBuilder.buildContours();
}

void Hix::Features::Cut::ZAxialCutImp::generateCaps()
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
	auto triangulationResult = cdt.triangulate();

	//add triangles
	for (auto& pair : triangulationResult)
	{
		auto& cap = pair.second;
		for (auto& trig : cap)
		{
			std::array<QVector3D, 3> vertices
			{
				_origMesh->ptToLocal(QVector3D(trig[0], _cuttingPlane)),
				_origMesh->ptToLocal(QVector3D(trig[1], _cuttingPlane)),
				_origMesh->ptToLocal(QVector3D(trig[2], _cuttingPlane))

			};
			_bottomMesh->addFace(vertices[0], vertices[1], vertices[2]);
			_topMesh->addFace(vertices[2], vertices[1], vertices[0]);
		}
	}
}

void Hix::Features::Cut::ZAxialCutImp::fillOverlap(const Hix::Slicer::ContourSegment& seg)
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
		from = _origMesh->ptToLocal(from);
		to = _origMesh->ptToLocal(to);
		_topMesh->addFace(seg.from, seg.to, highs[0].localPosition());
		_bottomMesh->addFace(seg.to, seg.from, lows[0].localPosition());
	}
	else
	{
		//two edges case

		Mesh* twoFacesAddedMesh, * oneFaceAddedMesh;
		QVector3D from;
		QVector3D to;
		bool isPyramid = lows.size() > highs.size();
		std::vector<VertexConstItr> majority(std::move(lows));
		std::vector<VertexConstItr> minority(std::move(highs));
		if (isPyramid)
		{
			// ▽
			from = QVector3D(seg.from, _cuttingPlane);
			to = QVector3D(seg.to, _cuttingPlane);
			twoFacesAddedMesh = _bottomMesh;
			oneFaceAddedMesh = _topMesh;
		}
		else
		{
			// △
			from = QVector3D(seg.to, _cuttingPlane);
			to = QVector3D(seg.from, _cuttingPlane);
			majority.swap(minority);
			twoFacesAddedMesh = _topMesh;
			oneFaceAddedMesh = _bottomMesh;
		}
		from = _origMesh->ptToLocal(from);
		to = _origMesh->ptToLocal(to);
		//need to sort majority to winding order
		HalfEdgeConstItr hintEdge;
		face.getEdgeWithVertices(hintEdge, majority[0], majority[1]);
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




