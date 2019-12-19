
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
			virtual ~AddModel();
			void undo()override;
			void redo()override;
			GLModel* getAddedModel();
			std::unique_ptr<GLModel> getAddedModelUnique();

		private:
			std::unique_ptr<GLModel> _addedModel;
			std::unique_ptr<GLModel> _deletedModel;
		};
	}
}