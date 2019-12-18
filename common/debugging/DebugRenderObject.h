#pragma once
#ifdef _DEBUG
#include <deque>
#include "../Singleton.h"
#include "../../render/LineMeshEntity.h"
#include "../../render/PlaneMeshEntity.h"
#include "../../render/Color.h"
#include "../../feature/Plane3D.h"
#include "../../DentEngine/src/mesh.h"

namespace Qt3DCore
{
	class QEntity;

}
class GLModel;
namespace Hix
{
	namespace Debug
	{
		class DebugRenderObject: public Hix::Common::Singleton<DebugRenderObject>
		{
		public:
			void initialize(Qt3DCore::QEntity* root);
			void addLine(const std::vector<QVector3D>& vertices, const QColor& color = QColor(0,0,0, 255));
			void addPaths(const std::vector< std::vector<QVector3D>>& paths, const QColor& color = QColor(0, 0, 0, 255));
			void outlineFace(const Hix::Engine3D::FaceConstItr& face, const QColor& color = QColor(0, 0, 0, 255));
			void registerDebugColorFaces(GLModel* owner, const std::unordered_set<FaceConstItr>& faces);
			void colorDebugFaces();
			void clear();
			Hix::Render::PlaneMeshEntity& displayPlane(const Hix::Plane3D::PDPlane& pdplane);
		private:
			std::unordered_map<GLModel*, std::unordered_set<FaceConstItr>> _debugFaceMap;
			std::deque<Hix::Render::LineMeshEntity> _lines;
			std::deque<Hix::Render::PlaneMeshEntity>_planes;
			Qt3DCore::QEntity* _root = nullptr;
		};
	}

}
#else
#endif