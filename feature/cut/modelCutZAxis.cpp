#include "modelCutZAxis.h"
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Slicer;
Hix::Features::Cut::ZAxisCutTask::ZAxisCutTask(const Engine3D::Mesh* originalMesh, float z): _origMesh(originalMesh), _cuttingPlane(z)
{
	divideTriangles();
	generateCutContour();
	generateCaps();
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
		auto sortedZ = mf->sortZ();
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
	clpr.PreserveCollinear(true);
	for (size_t idx = 0; idx < _contours.size(); idx++) { // divide into parallel threads
		clpr.AddPath(pathCont[idx], ptSubject, true);
	}
	ClipperLib::PolyTree polytree;
	clpr.Execute(ctUnion, polytree, pftNonZero, pftNonZero);
	auto curr = polytree.GetFirst();
	size_t count = 0;
	size_t fuck = 0;
	while (curr != nullptr)
	{
		for (auto each : curr->Contour)
		{
			if (fIntPtMap.find(each) == fIntPtMap.end())
			{
				++fuck;
			}
			++count;
		}
		curr = curr->GetNext();
	}

	//copy bakjh here.
}
