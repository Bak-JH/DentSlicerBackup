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
				ZAxialCutImp(GLModel* subject, float cuttingPlane, Mesh*& topMesh, Mesh*& botMesh, ZAxialCut::Result option);
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
				ZAxialCut::Result _option;
			};
		}
	}
}


Hix::Features::Cut::ZAxialCut::ZAxialCut(GLModel* subject, float cuttingPlane, Hix::Features::Cut::ZAxialCut::Result option) :
	_cuttingPlane(cuttingPlane), _container(new Hix::Features::FeatureContainer())
{
	//do listed part first

	Mesh* listedTopMesh = nullptr;
	Mesh* listedBotMesh = nullptr;

	ZAxialCutImp(subject, cuttingPlane, listedTopMesh, listedBotMesh, option);
	bool deleteOriginal = false;
	GLModel* botModel = nullptr;
	GLModel* topModel = nullptr;
	if (listedBotMesh == nullptr && listedTopMesh == nullptr)
	{
	}
	else if (listedBotMesh == nullptr)
	{
		topModel = subject;
		topModel->setMesh(listedTopMesh);
	}
	else if (listedTopMesh == nullptr)
	{
		botModel = subject;
		botModel->setMesh(listedBotMesh);

	}
	else
	{
		auto addBot = qmlManager->createAndListModel(listedBotMesh, subject->modelName() + "_bot", &subject->transform());
		auto addTop = qmlManager->createAndListModel(listedTopMesh, subject->modelName() + "_top", &subject->transform());
		
		_container->addFeature(addBot);
		_container->addFeature(addTop);
		
		botModel = dynamic_cast<Hix::Features::AddModel*>(addBot)->getAddedModel();
		topModel = dynamic_cast<Hix::Features::AddModel*>(addTop)->getAddedModel();
		deleteOriginal = true;
	}
	_divisionMap.insert(std::make_pair(subject, std::make_pair(topModel, botModel)));
	//do it for each unlisted children
	doChildrenRecursive(subject, cuttingPlane, option);
	//if parent exists, add to that parent, if no parent exists, promot to listed model
	std::deque<GLModel*> promoteToListed;
	for (auto child : _topChildren)
	{
		auto origParent = dynamic_cast<GLModel*>(child->parentEntity());
		auto parentTB = _divisionMap[origParent];
		if (parentTB.first != nullptr)
		{
			child->setParent(parentTB.first);
		}
		else
		{
			promoteToListed.push_back(child);
		}
	}
	for (auto child : _botChildren)
	{
		auto origParent = dynamic_cast<GLModel*>(child->parentEntity());
		auto parentTB = _divisionMap[origParent];
		if (parentTB.second != nullptr)
		{
			child->setParent(parentTB.second);
		}
		else
		{
			promoteToListed.push_back(child);
		}
	}
	std::vector<GLModel*> listed;
	listed.reserve(promoteToListed.size() + 2);
	for (auto each : promoteToListed)
	{
		listed.push_back(dynamic_cast<Hix::Features::AddModel*>(qmlManager->listModel(each))->getAddedModel());
	}
	if (botModel)
	{
		listed.push_back(botModel);
	}
	if (topModel)
	{
		listed.push_back(topModel);
	}
	//need to set listed parts to the bed
	for (auto& each : listed)
	{
		each->setZToBed();
	}
	//delete split models
	_divisionMap.erase(subject);
	for (auto& split : _divisionMap)
	{
		size_t spltCnt = 0;
		//top
		if (split.second.first != nullptr && split.second.first != split.first)
		{
			++spltCnt;
		}
		//bot
		if (split.second.second != nullptr && split.second.second != split.first)
		{
			++spltCnt;
		}
		if (spltCnt != 0)
		{
			//if split occured, the original model needs to be deleted
			//delete split.first;
		}
	}
	if (deleteOriginal)
		_container->addFeature(new DeleteModel(subject));
		//qmlManager->deleteModelFile(subject->ID);

	qDebug() << qmlManager->glmodels.size();
}

void Hix::Features::Cut::ZAxialCut::doChildrenRecursive(GLModel* subject, float cuttingPlane, Result option)
{
	for (auto childNode : subject->childNodes())
	{
		auto model = dynamic_cast<GLModel*>(childNode);
		if (model)
		{
			GLModel* botModel = nullptr;
			GLModel* topModel = nullptr;
			Mesh* childTopMesh = nullptr;
			Mesh* childBotMesh = nullptr;
			ZAxialCutImp(model, cuttingPlane, childTopMesh, childBotMesh, option);
			if (childBotMesh == nullptr)
			{
				topModel = model;
				_topChildren.insert(topModel);
			}
			else if (childTopMesh == nullptr)
			{
				botModel = model;
				_botChildren.insert(botModel);
			}
			else
			{
				//need to set node hierarchy later
				topModel = new GLModel(model->parentEntity(), childTopMesh, model->modelName() + "_top", 0, &model->transform());
				botModel = new GLModel(model->parentEntity(), childBotMesh, model->modelName() + "_bot", 0, &model->transform());
				_topChildren.insert(topModel);
				_botChildren.insert(botModel);

			}
			_divisionMap.insert(std::make_pair(model, std::make_pair(botModel, topModel)));
			doChildrenRecursive(model, cuttingPlane, option);
		}
	}
}

void Hix::Features::Cut::ZAxialCut::undo()
{
	_container->undo();
}

Hix::Features::Cut::ZAxialCutImp::ZAxialCutImp(GLModel* subject, float cuttingPlane, Mesh*& topMesh, Mesh*& botMesh, ZAxialCut::Result option) :
	_cuttingPlane(cuttingPlane), _origMesh(subject->getMesh()), _option((option))
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
	else if (_bottomMesh->getFaces().size() == _origMesh->getFaces().size() && _option != ZAxialCut::Result::KeepTop)
	{
		delete _topMesh;
		delete _bottomMesh;
		topMesh = nullptr;
		botMesh = subject->getMeshModd();
		return;
	}
	else if (_topMesh->getFaces().size() == _origMesh->getFaces().size() && _option != ZAxialCut::Result::KeepBottom)
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

	if (_option == ZAxialCut::Result::KeepTop)
	{
		delete _bottomMesh;
		_bottomMesh = nullptr;
	}
	else if (_option == ZAxialCut::Result::KeepBottom)
	{
		delete _topMesh;
		_topMesh = nullptr;
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




