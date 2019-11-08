#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include <Qt3DRender>
#include "interfaces/SelectFaceFeature.h"
#include "interfaces/PPShaderFeature.h"

class GLModel;
namespace Hix
{
	namespace Features
	{
		using namespace Hix::Engine3D;
		class Extend: public SelectFaceFeature, public PPShaderFeature //pee pee hee hee
		{
		public:
			Extend(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~Extend();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void extendMesh(double distance);

		private:
			Paths3D detectExtensionOutline(Mesh* mesh, const std::unordered_set<FaceConstItr>& meshfaces);
			void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPath, double distance);
			void coverCap(GLModel* model, QVector3D normal, const std::unordered_set<FaceConstItr>& extension_faces, double distance);

			GLModel* _model = nullptr;
			QVector3D _normal;
			std::unordered_set<FaceConstItr> _extensionFaces;
		};
	}
}

#endif // EXTENSION_H
