#pragma once
#ifdef _DEBUG
#include <deque>
#include "../Singleton.h"
#include "../../render/LineMeshEntity.h"
#include "../../render/PlaneMeshEntity.h"
#include "../../render/Color.h"
#include "../../feature/Plane3D.h"
#include "../../DentEngine/src/mesh.h"
//#include "glmodel.h"

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
			void addLine(const std::vector<QVector3D>& vertices, const QVector4D& color = Hix::Render::Colors::OutOfBound);
			void addPaths(const std::vector< std::vector<QVector3D>>& paths, const QVector4D& color = Hix::Render::Colors::OutOfBound);
			void outlineFace(const Hix::Engine3D::FaceConstItr& face, const QVector4D& color = Hix::Render::Colors::OutOfBound);
			void registerDebugColorFaces(GLModel* owner, const std::unordered_set<FaceConstItr>& faces);
			void colorDebugFaces();
			void showGLModelAabb(GLModel* target);
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