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

namespace Hix
{
	namespace Render
	{
		class SceneEntityWithMaterial;
	}
	namespace Debug
	{
		using namespace Hix::Render;
		class DebugRenderObject: public Hix::Common::Singleton<DebugRenderObject>
		{
		public:
			void initialize(Qt3DCore::QEntity* root);
			void addLine(const std::vector<QVector3D>& vertices, const QColor& color = QColor(0,0,0, 255));
			void addPaths(const std::vector< std::vector<QVector3D>>& paths, const QColor& color = QColor(0, 0, 0, 255));
			void outlineFace(const Hix::Engine3D::FaceConstItr& face, const QColor& color = QColor(0, 0, 0, 255));
			void registerDebugColorFaces(SceneEntityWithMaterial* owner, const std::unordered_set<FaceConstItr>& faces);
			void colorDebugFaces();
			void showGLModelAabb(SceneEntityWithMaterial* target);
			void clear();
			Hix::Render::PlaneMeshEntity& displayPlane(const Hix::Plane3D::PDPlane& pdplane);
		private:
			std::unordered_map<SceneEntityWithMaterial*, std::unordered_set<FaceConstItr>> _debugFaceMap;
			std::deque<Hix::Render::LineMeshEntity> _lines;
			std::deque<Hix::Render::PlaneMeshEntity>_planes;
			Qt3DCore::QEntity* _root = nullptr;
		};
	}

}
#else
#endif