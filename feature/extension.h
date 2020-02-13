#ifndef EXTENSION_H
#define EXTENSION_H
#include "DentEngine/src/mesh.h"
#include <Qt3DRender>
#include "interfaces/SelectFaceMode.h"
#include "interfaces/PPShaderMode.h"
#include "interfaces/Mode.h"
#include "feature/interfaces/FlushSupport.h"
#include "interfaces/DialogedMode.h"
class GLModel;
namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class InputSpinBox;
		}
	}

	namespace Features
	{
		using namespace Engine3D;

		struct ExtendArg
		{
			QVector3D normal;
			std::unordered_set<FaceConstItr> extensionFaces;
		};


		class Extend: public Feature
		{
		public:
			Extend(GLModel* targetModel, const QVector3D& targetFaceNormal,
					const std::unordered_set<FaceConstItr>& targetFaces, double distance);
			virtual ~Extend();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		private:
			void coverCap(GLModel* model, QVector3D normal, const std::unordered_set<FaceConstItr>& extension_faces, double distance);

			GLModel* _model;
			QVector3D _normal;
			std::unordered_set<FaceConstItr> _extensionFaces;
			double _distance;
			Mesh* _prevMesh;
			Mesh* _nextMesh;
		};



		class ExtendMode: public SelectFaceMode, public PPShaderMode, public DialogedMode
		{
		public:
			ExtendMode();
			virtual ~ExtendMode();
			void faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)override;
			void apply()override;

		private:
			QVector3D _normal;
			std::unordered_map<GLModel*, ExtendArg> _args;
			Hix::QML::Controls::InputSpinBox* _extendValue;
		};
	}
}

#endif // EXTENSION_H
