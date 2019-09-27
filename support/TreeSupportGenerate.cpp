#include "TreeSupportGenerate.h"
#include "SupportRaftManager.h"
#include "SupportModel.h"
#include "TreeSupportModel.h"

Hix::Support::TreeSupportGenerate::TreeSupportGenerate(SupportRaftManager* manager, Hix::OverhangDetect::Overhangs _overhangs,
	std::unordered_map<SupportModel*, std::unique_ptr<SupportModel>>* supports)
{
	_manager = manager;
	_supports = supports;
	generateVerticalTips(_overhangs);
}

float Hix::Support::TreeSupportGenerate::getZmin()
{
	return _manager->supportBottom();
}

void Hix::Support::TreeSupportGenerate::generateVerticalTips(Hix::OverhangDetect::Overhangs _overhangs)
{
	for (auto& each : _overhangs)
	{
		QVector3D overhang;
		QVector3D tipNormal;
		QVector3D bottom;

		if (each.index() == 0)
		{
			auto& vtx = std::get<0>(each);
			overhang = vtx->position;
			tipNormal = vtx->vn;
		}
		else
		{
			auto& faceOverhang = std::get<1>(each);
			overhang = faceOverhang.first;
			tipNormal = faceOverhang.second->fn;
		}

		bottom = overhang - QVector3D(0, 0, tip_len);

		tree_support_path[overhang] = overhang;
		tree_support_info[overhang] = TreeSupportInfo{ tipNormal, radius_min, Top };

		auto newModel = new TreeSupportModel(_manager, this, each, overhang, bottom);
		(*_supports)[newModel] = std::unique_ptr<TreeSupportModel>(newModel);
	}
}

QVector3D Hix::Support::TreeSupportGenerate::coneNconeIntersection(QVector3D coneApex1, QVector3D coneApex2)
{
	float z_min = getZmin();

	float x1 = coneApex1.x();
	float y1 = coneApex1.y();
	float z1 = coneApex1.z() - z_min;
	float x2 = coneApex2.x();
	float y2 = coneApex2.y();
	float z2 = coneApex2.z() - z_min;
	float tan = float(std::tan(critical_angle_radian));
	float xyDis = std::sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));

	if (xyDis == 0) {
		if (z1 <= z2) return coneApex1;
		else return coneApex2;
	}

	if (xyDis >= z1 * tan + z2 * tan) // intersection under bed
		return QVector3D(99999, 99999, 99999);

	if (xyDis > std::abs((z1 - z2) * tan)) {
		float radius = 0.5f * ((z1 + z2) * tan - xyDis);
		float m = z1 * tan - radius;
		float n = z2 * tan - radius;
		float x = (n * x1 + m * x2) / (m + n);
		float y = (n * y1 + m * y2) / (m + n);
		float z = radius / tan;
		return QVector3D(x, y, z + z_min);
	}
	else {
		if (z1 <= z2) return coneApex1;
		else return coneApex2;
	}
}
