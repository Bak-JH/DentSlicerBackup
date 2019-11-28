#pragma once
#ifdef _DEBUG
#include <deque>
#include "../Singleton.h"
#include "../../render/LineMeshEntity.h"
#include "../../render/Color.h"
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
			void addLine(const std::vector<QVector3D>& vertices, const QVector4D& color = Hix::Render::Colors::OutOfBound);
			void outlineFace(const Hix::Engine3D::FaceConstItr& face, const QVector4D& color = Hix::Render::Colors::OutOfBound);
			void registerDebugColorFaces(GLModel* owner, const std::unordered_set<FaceConstItr>& faces);
			void colorDebugFaces();
			void clear();
		private:
			std::unordered_map<GLModel*, std::unordered_set<FaceConstItr>> _debugFaceMap;
			std::deque<Hix::Render::LineMeshEntity> _lines;
			Qt3DCore::QEntity* _root = nullptr;
		};
	}

}
#else
#endif