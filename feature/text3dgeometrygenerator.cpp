#include "feature/text3dgeometrygenerator.h"

void Text3D::generateTextFace(const PolyTree& tree, Hix::Engine3D::Mesh* targetMesh)
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

	//get triangle
 	for (auto& eachSolid : solids)
	{
		p2t::CDT cdt(toPointVector(_floatPathMap.find(eachSolid)->second));

		if (eachSolid->ChildCount() > 0)
		{
			for (auto& hole : eachSolid->Childs)
			{
				cdt.AddHole(toPointVector(hole->Contour));
			}
		}

		cdt.Triangulate();
		for (auto& tri : cdt.GetTriangles())
		{
			p2t::Point *pt0, * pt1, * pt2;
			pt0 = tri->GetPoint(0);
			pt1 = tri->GetPoint(1);
			pt2 = tri->GetPoint(2);

			targetMesh->addFace(
				QVector3D(pt0->x, pt0->y, targetMesh->z_max()),
				QVector3D(pt1->x, pt1->y, targetMesh->z_max()),
				QVector3D(pt2->x, pt2->y, targetMesh->z_max()));
		}
	}
}

std::vector<p2t::Point*> Text3D::toPointVector(const std::vector<QVector2D> floatPath)
{
	std::vector<p2t::Point*> result;
	for (auto floatPoint : floatPath)
	{
		result.push_back(new p2t::Point(floatPoint.x(), floatPoint.y()));
	}

	return result;
}

std::vector<p2t::Point*> Text3D::toPointVector(const ClipperLib::Path intPath)
{
	std::vector<p2t::Point*> result;
	for (auto intPoint : intPath)
	{
		result.push_back(new p2t::Point(intPoint.X, intPoint.Y));
	}

	return result;
}

void Text3D::generateText3D(QFont font, QString text, Hix::Engine3D::Mesh* targetMesh)
{
	PolyTree polytree;
	Clipper clpr;
	QPainterPath painterPath;
	painterPath.setFillRule(Qt::WindingFill);
	painterPath.addText(0, 0, font, text);
	QList<QPolygonF> polygons = painterPath.toSubpathPolygons(QTransform().scale(1.0f, -1.0f));
	
	for (auto polygon : polygons)
	{
		Path IntPath;
		for (auto point : polygon)
		{
			IntPath.push_back(toInt2DPt(QVector3D(point)));
		}
		clpr.AddPath(IntPath, ptSubject, true);
	}

	// generate polytree
	clpr.Execute(ctUnion, polytree, pftNonZero, pftNonZero);

	qDebug() << polytree.ChildCount();
	// add float path
	auto curr = polytree.GetFirst();
	while (curr)
	{
		std::vector<QVector2D> floatPath;
		floatPath.reserve(curr->Contour.size());
		for (auto point : curr->Contour)
			floatPath.emplace_back(toFloatPt(point));
		_floatPathMap.insert(std::pair<ClipperLib::PolyNode*, std::vector<QVector2D>>{curr, std::move(floatPath)});
		qDebug() << curr->IsHole();
		curr = curr->GetNext();
	}
	
	generateTextFace(polytree, targetMesh);
}
