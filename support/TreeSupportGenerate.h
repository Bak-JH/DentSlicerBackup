#include "SupportModel.h"
#include "SupportRaftManager.h"
#include "SupportModel.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;

namespace Hix
{
	using namespace Engine3D;
	using namespace OverhangDetect;
	namespace Support
	{
		class TreeSupportGenerate
		{
		public:

			enum Place { Top, Support, Mesh, Bed };

			struct TreeSupportInfo {
				QVector3D normal;
				float radius;
				Place place;
			};

			TreeSupportGenerate(SupportRaftManager* manager, Hix::OverhangDetect::Overhangs _overhangs,
				std::unordered_map<SupportModel*, std::unique_ptr<SupportModel>>* _supports);

			float critical_angle = 45;
			float critical_angle_radian = M_PI * (critical_angle / 180.0);
			float radius_min = 0.2f;
			float tip_len = 3;

			std::unordered_map<QVector3D, TreeSupportInfo> tree_support_info; // -> TreeSupportModel.h?

		private:
			SupportRaftManager* _manager;
			std::unordered_map<SupportModel*, std::unique_ptr<SupportModel>>* _supports;

			std::unordered_map<QVector3D, QVector3D> tree_support_path;

			float getZmin();
			void generateVerticalTips(Hix::OverhangDetect::Overhangs _overhangs); // param?
			QVector3D coneNconeIntersection(QVector3D coneApex1, QVector3D coneApex2);
		};
	}
}
