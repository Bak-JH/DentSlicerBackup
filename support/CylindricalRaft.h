#pragma once

#include "RaftModel.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;


namespace Hix
{
	using namespace Engine3D;
	using namespace OverhangDetect;
	namespace Support
	{
		//heaxgon centered at 0,0,0, rotated around positive z-axis CW
		std::vector<QVector3D> generateHexagon(float radius);
		class CylindricalRaft : public Hix::Support::RaftModel
		{
		public:
			CylindricalRaft(SupportRaftManager* manager, std::vector<QVector3D>);
			virtual ~CylindricalRaft();

		protected:
			QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			void generateMesh(const std::vector<QVector3D>& overhangs);
			void generateMeshForContour(Mesh* mesh, const std::vector<QVector3D>& contour);



		};
	}
}
