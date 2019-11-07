#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include <Qt3DRender>
#include "Feature.h"

class GLModel;
namespace Hix
{
	namespace Features
	{
		using namespace Hix::Engine3D;
		class Extend : public Feature
		{
		public:
			Extend(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~Extend();
			void extendMesh(Mesh* mesh, const QVector3D& normal, const std::unordered_set<FaceConstItr>& mf, double distance);
			Paths3D detectExtensionOutline(Mesh* mesh, const std::unordered_set<FaceConstItr>& meshfaces);
			void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPath, double distance);
			void coverCap(Mesh* mesh, QVector3D normal, const std::unordered_set<FaceConstItr>& extension_faces, double distance);
		private:
			std::unordered_set<GLModel*> _models;
		};
	}
}

#endif // EXTENSION_H
