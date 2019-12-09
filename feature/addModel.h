#pragma once
#pragma once
#include "feature/interfaces/Feature.h"
#include "DentEngine/src/mesh.h"

namespace Hix
{
	namespace Features
	{
		class AddModel : public Feature
		{
		public:
			AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, int id, const Qt3DCore::QTransform* transform);
			AddModel(Qt3DCore::QEntity* parent, GLModel* model, int id);
			virtual ~AddModel();
			void undo()override;
			void redo()override {} //TODO: redo
			GLModel* getAddedModel();
			std::unique_ptr<GLModel> getAddedModelUnique();

		private:
			GLModel* _addedModel;
		};
	}
}