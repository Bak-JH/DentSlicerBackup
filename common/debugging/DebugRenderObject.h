#pragma once
#ifdef _DEBUG
#include <deque>
#include "../Singleton.h"
#include "../../render/LineMeshEntity.h"
#include "../../render/Color.h"
namespace Qt3DCore
{
	class QEntity;

}
namespace Hix
{
	namespace Debug
	{
		class DebugRenderObject: public Hix::Common::Singleton<DebugRenderObject>
		{
		public:
			void initialize(Qt3DCore::QEntity* root);
			void addLine(const std::vector<QVector3D>& vertices, const QVector4D& color = Hix::Render::Colors::OutOfBound);
			void clear();
		private:
			std::deque<Hix::Render::LineMeshEntity> _lines;
			Qt3DCore::QEntity* _root = nullptr;
		};
	}

}
#else
#endif