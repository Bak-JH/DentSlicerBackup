#pragma once
#include "feature/interfaces/Feature.h"
#include "feature/interfaces/FlushSupport.h"
#include <QObject>
#include <Qt3DCore>
#include <variant>
#include <memory>
namespace Hix
{
	namespace Features
	{
		class DeleteModel : public Feature, public FlushSupport
		{
		public:
			DeleteModel(GLModel* target);
			virtual ~DeleteModel();
			void undo()override;
			void redo()override;
			GLModel* getDeletedModel();

		private:
			struct RedoInfo
			{
				std::unique_ptr<GLModel> redoModel;
				Qt3DCore::QNode* parent;
			};
			std::variant<GLModel*, RedoInfo> _model;
		};
	}
}