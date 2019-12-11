#pragma once
#include "feature/interfaces/Feature.h"

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
			GLModel* _deletedModel;
			GLModel* _addedModel;
		};
	}
}