#include "feature/text3dgeometrygenerator.h"



std::unordered_map<PolyNode*, std::vector<p2t::Triangle*>> Text3D::triangluateBFS(PolyTree& tree)
{
	// find solids
	std::unordered_set<PolyNode*> solids;
	std::deque<PolyNode*> q;
	auto firstSolids = tree.Childs;
	for (auto& each : firstSolids)
	{
		solids.emplace(each);
		q.emplace_back(each);
	}
	while (!q.empty())
	{
		auto curr = q.front();
		q.pop_front();
		auto holes = curr->Childs;
		for (auto& hole : holes)
		{
			auto currSolids = hole->Childs;
			for (auto& solid : currSolids)
			{
				solids.emplace(solid);
				q.emplace_back(solid);
			}
		}
	}

	std::unordered_map<PolyNode*, std::vector<p2t::Triangle*>> result;
	for (auto& eachSolid : solids)
	{
		p2t::CDT cdt(toPointVector(floatPathMap.find(eachSolid)->second));
		result[eachSolid] = cdt.GetTriangles();
	}

	return result;
}

std::vector<p2t::Point*> Text3D::toPointVector(std::vector<QVector2D> floatPath)
{
	std::vector<p2t::Point*> result;
	for (auto floatPoint : floatPath)
	{
		result.push_back(new p2t::Point(floatPoint.x(), floatPoint.y()));
	}

	return result;
}

void Text3D::generateText3DGeometry(QVector3D** vertices, int* verticesSize,
                            unsigned int** indices, int* indicesSize,
                            QFont font, QString text, float depth,
                            Mesh* mesh,
                            const QVector3D normalVector,
                            const QMatrix4x4& transform,
                            const QMatrix4x4& normalTransform)
{
	PolyTree polytree;
	Clipper clpr;
	QPainterPath painterPath;
	painterPath.setFillRule(Qt::WindingFill);
	painterPath.addText(0, 0, font, text);
	QList<QPolygonF> polygons = painterPath.toSubpathPolygons(QTransform().scale(1.0f, -1.0f));
	
	Path IntPath;
	for (auto polygon : polygons)
		for (auto point : polygon)
			IntPath.push_back(toInt2DPt(QVector3D(point)));

	// generate polytree
	clpr.Clear();
	clpr.AddPath(IntPath, ptSubject, true);
	clpr.Execute(ctUnion, polytree, pftNonZero, pftNonZero);
	std::vector<p2t::Triangle*> triangles;

	// add float path
	for (auto curr : polytree.Childs)
	{
		std::vector<QVector2D> floatPath;
		while (curr)
		{
			for (auto point : curr->Contour)
				floatPath.push_back(toFloatPt(point));

			floatPathMap[curr] = floatPath;
			curr->GetNext();
		}
	}

	triangluateBFS(polytree);
}
