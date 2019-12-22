
#pragma once
#include <Qt3DCore>
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
			AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform);
			virtual ~AddModel();
			void undo()override;
			void redo()override;
			GLModel* getAddedModel();
		protected:
			struct UndoInfo
			{
				std::unique_ptr<GLModel> undoModel;
				Qt3DCore::QNode* parent;
			};
			std::variant<GLModel*, UndoInfo> _model;
		};

		class ListModel : public AddModel
		{
			ListModel(Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform);
			virtual ~ListModel();
			void undo()override;
			void redo()override;
		};
	}
}