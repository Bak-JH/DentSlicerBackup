#pragma once

#include "SupportModel.h"
#include "TreeSupportGenerate.h"

using namespace Qt3DCore;
using namespace Qt3DRender;
using namespace Qt3DExtras;


namespace Hix
{
	using namespace Engine3D;
	using namespace OverhangDetect;
	namespace Support
	{
		class TreeSupportModel : public Hix::Support::SupportModel
		{
		public:
			TreeSupportModel(SupportRaftManager* manager, TreeSupportGenerate* tree_manager, std::variant<VertexConstItr, FaceOverhang> overhang,
				QVector3D top, QVector3D bottom);
			virtual ~TreeSupportModel();
			QVector3D getBasePt()override;

		protected:
			QVector3D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
			TreeSupportGenerate* _tree_manager;
		private:
			void generateMesh(QVector3D top, QVector3D bottom);
			std::vector<QVector3D>  generateSupportPath(const QVector3D top, const QVector3D bottom);
		};
	}
}
