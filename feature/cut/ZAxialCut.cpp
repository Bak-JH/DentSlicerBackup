#include "ZAxialCut.h"
#include "../../glmodel.h"
#include "feature/addModel.h"
#include "feature/deleteModel.h"
#include "../cdt/HixCDT.h"

using namespace Hix;
using namespace Hix::Features::Cut;
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
				ZAxialCutImp(GLModel* subject, float cuttingPlane, Mesh*& topMesh, Mesh*& botMesh, Hix::Features::Cut::KeepType keep);
			private:
				void divideTriangles();
				void generateCutContour();
				void generateCaps();
				void fillOverlap(const Hix::Slicer::ContourSegment& seg);
				float _cuttingPlane;
				const Engine3D::Mesh* _origMesh;
				std::unique_ptr<Engine3D::Mesh> _bottomMesh;
				std::unique_ptr<Engine3D::Mesh> _topMesh;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _botFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _overlapFaces;
				std::unordered_set<Hix::Engine3D::FaceConstItr> _topFaces;
				std::vector<Hix::Slicer::Contour> _contours;
				Hix::Features::Cut::KeepType _keepType;
			};
		}
	}
}


Hix::Features::Cut::ZAxialCut::ZAxialCut(GLModel* subject, float cuttingPlane, KeepType keep) : FeatureContainerFlushSupport(subject),
	_cuttingPlane(cuttingPlane), _subject(subject), _keep(keep)
{


}

void Hix::Features::Cut::ZAxialCut::runImpl()
{
	if (_cuttingPlane >= _subject->recursiveAabb().zMax() || _cuttingPlane <= _subject->recursiveAabb().zMin())
	{
		throw std::runtime_error("Z Axial Cut cutting plane on the z limit of the model");
	}//do listed part first
	doChildrenRecursive(_subject, _cuttingPlane);
	addFeature(new DeleteModel(_subject));
	Hix::Features::FeatureContainerFlushSupport::runImpl();
}

void Hix::Features::Cut::ZAxialCut::doChildrenRecursive(GLModel* subject, float cuttingPlane)
{
	Mesh* childTopMesh = nullptr;
	Mesh* childBotMesh = nullptr;
	ZAxialCutImp(subject, cuttingPlane, childTopMesh, childBotMesh, _keep);
	if(subject->getMesh()->getFaces().empty())
	{ }
	else
	{
		if (childTopMesh != nullptr && !childTopMesh->getFaces().empty())
		{
			auto addTopModel = new ListModel(childTopMesh, subject->modelName() + "_top", &subject->transform());
			addFeature(addTopModel);

		}
		if (childBotMesh != nullptr && !childBotMesh->getFaces().empty())
		{
			auto addBotModel = new ListModel(childBotMesh, subject->modelName() + "_bot", &subject->transform());
			addFeature(addBotModel);
		}
		
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

Hix::Features::Cut::ZAxialCutImp::ZAxialCutImp(GLModel* subject, float cuttingPlane, Mesh*& topMesh, Mesh*& botMesh, Hix::Features::Cut::KeepType keepType) :
	_cuttingPlane(cuttingPlane), _origMesh(subject->getMesh()), _keepType(keepType)
{
	if(_keepType != KeepBottom)
		_topMesh.reset( new Mesh());
	if(_keepType != KeepTop)
		_bottomMesh.reset(new Mesh());
	//early exit, no cut needed
	if (_origMesh->getFaces().size() == 0)
	{
		//empty node, add empty halves to both sides
		botMesh = _bottomMesh.release();
		topMesh = _topMesh.release();
		return;
	}

	divideTriangles();
	
	if (_bottomMesh && _bottomMesh->getFaces().size() == _origMesh->getFaces().size())
	{
		botMesh = subject->getMeshModd();
		return;
	}
	else if (_topMesh && _topMesh->getFaces().size() == _origMesh->getFaces().size())
	{
		topMesh = subject->getMeshModd();
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
	if(_bottomMesh)
		botMesh = _bottomMesh.release();
	if(_topMesh)
		topMesh = _topMesh.release();
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
	if (_bottomMesh)
	{
		for (auto& each : _botFaces)
		{
			_bottomMesh->addFace(each);
		}
	}
	if (_topMesh)
	{
		for (auto& each : _topFaces)
		{
			_topMesh->addFace(each);
		}
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
	Hix::CDT::PolytreeCDT cdt(&fIntPtMap, &polytree);
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
			if(_bottomMesh)
				_bottomMesh->addFace(vertices[0], vertices[1], vertices[2]);
			if(_topMesh)
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
		if (_topMesh)
			_topMesh->addFace(seg.from, seg.to, highs[0].localPosition());
		if (_bottomMesh)
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
			twoFacesAddedMesh = _bottomMesh.get();
			oneFaceAddedMesh = _topMesh.get();
		}
		else
		{
			// △
			from = QVector3D(seg.to, _cuttingPlane);
			to = QVector3D(seg.from, _cuttingPlane);
			majority.swap(minority);
			twoFacesAddedMesh = _topMesh.get();
			oneFaceAddedMesh = _bottomMesh.get();
		}
		from = _origMesh->ptToLocal(from);
		to = _origMesh->ptToLocal(to);
		//need to sort majority to winding order
		HalfEdgeConstItr hintEdge;
		face.getEdgeWithVertices(hintEdge, majority[0], majority[1]);
		majority[0] = hintEdge.from();
		majority[1] = hintEdge.to();

		//face goes in this order, f-> t -> minority
		if(oneFaceAddedMesh)
			oneFaceAddedMesh->addFace(from, to, minority[0].localPosition());
		// maj[0]->t->f 
		// maj[0]->maj[1]->t
		if (twoFacesAddedMesh)
		{
			twoFacesAddedMesh->addFace(majority[0].localPosition(), to, from);
			twoFacesAddedMesh->addFace(majority[0].localPosition(), majority[1].localPosition(), to);
		}

	}



}




