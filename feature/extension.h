#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include "meshrepair.h"
#include <Qt3DRender>
namespace Hix
{
	namespace Features
	{
		namespace Extension
		{

			void extendMesh(Mesh* mesh, FaceConstItr mf, double distance);
			Paths3D detectExtensionOutline(Mesh* mesh, const std::unordered_set<FaceConstItr>& meshfaces);
			void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPath, double distance);
			void coverCap(Mesh* mesh, QVector3D normal, const std::unordered_set<FaceConstItr>& extension_faces, double distance);
		};
	}
}

#endif // EXTENSION_H
