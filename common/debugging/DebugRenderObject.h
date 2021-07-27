#pragma once
#ifdef _DEBUG
#include <deque>
#include "../Singleton.h"
#include "../../render/LineMeshEntity.h"
#include "../../render/PlaneMeshEntity.h"
#include "render/PointMeshEntity.h"
#include "../../render/Color.h"
#include "../../feature/Plane3D.h"
#include "../../Mesh/mesh.h"
#include <optional>
#include <functional>
#include "../../render/Bounds3D.h"
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
		struct FaceColorSet
		{
			std::unordered_set<Hix::Engine3D::FaceConstItr> faces;
			QVector4D color;
		};
		using namespace Hix::Render;
		class DebugRenderObject: public Hix::Common::Singleton<DebugRenderObject>
		{
		public:
			void initialize(Qt3DCore::QEntity* root);
			void addVertex(const QVector3D vertex, const QColor& color = QColor(0,0,0, 255));
			void addVertices(const std::vector<QVector3D>& vertices, const QColor& color = QColor(0,0,0, 255));
			void addLinePath(const std::vector<QVector3D>& vertices, const QColor& color = QColor(0,0,0, 255));
			void addLinePaths(const std::vector< std::vector<QVector3D>>& paths, const QColor& color = QColor(0, 0, 0, 255));
			void outlineFace(const Hix::Engine3D::FaceConstItr& face, const QColor& color = QColor(0, 0, 0, 255));
			void registerDebugColorFaces(SceneEntityWithMaterial* owner, const std::unordered_set<Hix::Engine3D::FaceConstItr>& faces, const QVector4D& color);
			void colorDebugFaces();
			void showGLModelAabb(SceneEntityWithMaterial* target);
			void showAabb(Hix::Engine3D::Bounds3D& aabb);
			void clear();
			std::optional<std::reference_wrapper<const FaceColorSet>>getOverrideColors(SceneEntityWithMaterial* owner)const;
			Hix::Render::PlaneMeshEntity& displayPlane(const Hix::Plane3D::PDPlane& pdplane);
		private:
			std::unordered_map<SceneEntityWithMaterial*, FaceColorSet> _debugFaceMap;
			std::deque<Hix::Render::LineMeshEntity> _lines;
			std::deque<Hix::Render::PlaneMeshEntity>_planes;
			std::deque<Hix::Render::PointMeshEntity>_points;
			Qt3DCore::QEntity* _root = nullptr;
		};
	}

}
#else
#endif