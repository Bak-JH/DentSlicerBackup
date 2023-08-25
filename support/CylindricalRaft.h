#pragma once

#include "RaftModel.h"


namespace Hix
{
	namespace Settings { class SupportSetting; }
	using namespace Engine3D;
	using namespace OverhangDetect;
	namespace Support
	{
		class CylindricalRaft : public Hix::Support::RaftModel
		{
		public:
			CylindricalRaft(SupportRaftManager* manager, std::vector<QVector3D>);
			CylindricalRaft(SupportRaftManager* manager, std::vector<QVector3D>, Hix::Settings::SupportSetting& setting);
			virtual ~CylindricalRaft();

		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			void generateMesh(const std::vector<QVector3D>& overhangs);
			void generateMesh(const std::vector<QVector3D>& overhangs, Hix::Settings::SupportSetting& setting);
			void generateMeshForContour(Mesh* mesh, const std::vector<QVector3D>& contour);
			void generateMeshForContour(Mesh* mesh, const std::vector<QVector3D>& contour, Hix::Settings::SupportSetting& setting);



		};
	}
}
