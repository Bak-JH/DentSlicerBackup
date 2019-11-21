#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include "autoarrange.h"
#include <Qt3DRender>
#include "interfaces/SelectFaceMode.h"
#include "interfaces/PPShaderFeature.h"
#include "interfaces/Mode.h"

class GLModel;
namespace Hix
{
	namespace Features
	{
		using namespace Engine3D;
		class Extend;
		struct ExtendArg
		{
			QVector3D normal;
			std::unordered_set<FaceConstItr> extensionFaces;
		};
		class ExtendMode : public SelectFaceMode, public PPShaderFeature, public Mode
		{
		public:
			ExtendMode(const std::unordered_set<GLModel*>& selectedModels);
			virtual ~ExtendMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			std::unique_ptr<FeatureContainer> applyExtend(double distance);

		private:
			GLModel* _model = nullptr;
			QVector3D _normal;
			std::unordered_map<GLModel*, ExtendArg> _args;
		};

		class Extend: public Feature //pee pee hee hee
		{
		public:
			Extend(GLModel* selectedModel, const QVector3D& selectedFaceNormal,
					const std::unordered_set<FaceConstItr>& targetFaces, double distance);
			virtual ~Extend();
			void undo() override;

		private:
			Paths3D detectExtensionOutline(Mesh* mesh, const std::unordered_set<FaceConstItr>& meshfaces);
			void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPath, double distance);
			void coverCap(GLModel* model, QVector3D normal, const std::unordered_set<FaceConstItr>& extension_faces, double distance);

			GLModel* _model;
			QVector3D _normal;
			std::unordered_set<FaceConstItr> _extensionFaces;
			Mesh* _prevMesh;
		};
	}
}

#endif // EXTENSION_H
