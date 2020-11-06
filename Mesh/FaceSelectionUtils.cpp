#include "FaceSelectionUtils.h"
using namespace Hix;
using namespace Hix::Engine3D;
using namespace Hix::Render;



std::deque<std::deque<HalfEdgeConstItr>> Hix::Engine3D::boundaryPath(const std::unordered_set<FaceConstItr>& faces)
{
	std::unordered_map<VertexConstItr, HalfEdgeConstItr> edgeMap;
	std::deque<std::deque<HalfEdgeConstItr>> paths;
	for (auto& f : faces)
	{
		auto e = f.edge();
		for (size_t i = 0; i < 3; ++i, e.moveNext())
		{
			auto nfs = e.nonOwningFaces();
			bool isBoundary = true;
			for (auto& nf : nfs)
			{
				if (faces.find(nf) != faces.cend())
				{
					isBoundary = false;
					break;
				}
			}
			if (isBoundary)
			{
				edgeMap.insert(std::make_pair(e.from(), e));
			}
		}
	}
	if (edgeMap.empty())
		return paths;


	while (!edgeMap.empty())
	{
		std::deque<HalfEdgeConstItr> path;
		path.emplace_back(edgeMap.begin()->second);
		edgeMap.erase(edgeMap.begin());
		bool pathIncomplete = true;
		while (pathIncomplete)
		{

			auto to = path.back().to();
			auto found = edgeMap.find(to);
			if (found != edgeMap.end())
			{
				path.emplace_back(found->second);
			}
			else
			{
				paths.emplace_back(std::move(path));
				pathIncomplete = false;
			}
			edgeMap.erase(to);
		}
	}
	return paths;
}



std::vector<QVector3D>  Hix::Engine3D::extendAlongOutline(
	QVector3D normal, double distance, const std::deque<HalfEdgeConstItr>& path)
{
	std::vector<QVector3D> vec;
	vec.reserve(path.size() * 2);
	for (auto& e : path)
	{
		auto v0 = e.from().localPosition();
		auto v1 = e.to().localPosition();
		auto e0 = v0 + distance * normal;
		auto e1 = v1 + distance * normal;
		vec.emplace_back(e1);
		vec.emplace_back(v0);
		vec.emplace_back(v1);

		vec.emplace_back(e1);
		vec.emplace_back(e0);
		vec.emplace_back(v0);
		//mesh.addFace(e1, v0, v1);
		//mesh.addFace(e1, e0, v0);
	}
	return vec;
}


std::vector<QVector3D>  Hix::Engine3D::extendAlongOutline2(
	QVector3D normal, double distance, const std::deque<HalfEdgeConstItr>& path)
{
	std::vector<QVector3D> vec;
	vec.reserve(path.size() * 2);
	for (auto& e : path)
	{
		auto v0 = e.from().worldPosition();
		auto v1 = e.to().worldPosition();
		auto e0 = v0 + distance * normal;
		auto e1 = v1 + distance * normal;
		vec.emplace_back(e1);
		vec.emplace_back(v0);
		vec.emplace_back(v1);

		vec.emplace_back(e1);
		vec.emplace_back(e0);
		vec.emplace_back(v0);
		//mesh.addFace(e1, v0, v1);
		//mesh.addFace(e1, e0, v0);
	}
	return vec;
}