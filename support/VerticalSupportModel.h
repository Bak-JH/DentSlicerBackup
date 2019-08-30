#pragma once

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
		//heaxgon centered at 0,0,0, rotated around positive z-axis CW
		std::vector<QVector3D> generateHexagon(float radius);
		class VerticalSupportModel : public Hix::Support::SupportModel
		{
		public:
			VerticalSupportModel(SupportRaftManager* manager, std::variant<VertexConstItr, FaceOverhang> overhang);
			virtual ~VerticalSupportModel();
			QVector3D getBasePt()override;

		protected:
			QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			void generateMesh();
			std::vector<QVector3D>  generateSupportPath(const std::variant<VertexConstItr, FaceOverhang>& overhang, float bottom);
		};
	}
}
