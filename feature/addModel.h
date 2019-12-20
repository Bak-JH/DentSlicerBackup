
#pragma once
#pragma once
#include "feature/interfaces/Feature.h"
#include "DentEngine/src/mesh.h"
#include "glmodel.h"

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

		private:
			GLModel* _addedModel;
			GLModel* _deletedModel;
		};
	}
}