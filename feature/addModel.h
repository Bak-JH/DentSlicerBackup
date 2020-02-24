
#pragma once
#include <Qt3DCore>
#include "feature/interfaces/Feature.h"
class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
	namespace Features
	{

		//two seperate features since we know at caller where the model is added to(root or other model)
		class AddModel : public Feature
		{
		public:
			AddModel(Qt3DCore::QEntity* parent, Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform);
			virtual ~AddModel();
			GLModel* getAddedModel();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
			struct UndoInfo
			{
				std::unique_ptr<GLModel> undoModel;
				Qt3DCore::QNode* parent;
			};
			std::variant<GLModel*, UndoInfo> _model;
			Qt3DCore::QEntity* _parent;
			Hix::Engine3D::Mesh* _mesh;
			QString _fname;
			const Qt3DCore::QTransform* _transform;
		};

		class ListModel : public AddModel
		{
		public:
			ListModel(Hix::Engine3D::Mesh* mesh, QString fname, const Qt3DCore::QTransform* transform);
			virtual ~ListModel();


		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		};
	}
}