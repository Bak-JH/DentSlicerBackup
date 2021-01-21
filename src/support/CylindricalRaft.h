#pragma once

#include "RaftModel.h"


namespace Hix
{
	using namespace Engine3D;
	using namespace OverhangDetect;
	namespace Support
	{
		class CylindricalRaft : public Hix::Support::RaftModel
		{
		public:
			CylindricalRaft(SupportRaftManager* manager, std::vector<QVector3D>);
			virtual ~CylindricalRaft();

		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			void generateMesh(const std::vector<QVector3D>& overhangs);
			void generateMeshForContour(Mesh* mesh, const std::vector<QVector3D>& contour);



		};
	}
}
