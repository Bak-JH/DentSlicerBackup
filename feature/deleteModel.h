#pragma once
#include "feature/interfaces/Feature.h"
#include <QObject>

namespace Hix
{
	namespace Features
	{
		class DeleteModel : public Feature
		{
		public:
			DeleteModel(GLModel* target);
			virtual ~DeleteModel();
			void undo()override;
			void redo()override;
			GLModel* getDeletedModel();

		private:
			std::pair<GLModel*, QObject*> _deletedModel;
			std::pair<GLModel*, QObject*> _addedModel;
		};
	}
}