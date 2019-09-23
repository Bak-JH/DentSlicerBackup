#ifndef LABELMANAGER_H
#define LABELMANAGER_H

#include "labelModel.h"

namespace Hix 
{
	namespace Labelling
	{
		class LabelModel;
		class LabelManager
		{
		public:
			LabelManager(GLModel* parent);
			~LabelManager();
			void addLabel();
			void removeLabel();
			void applyLabel();
			void cancel();
			GLModel* getModel();
			const std::vector<const Mesh*> getLabelMeshes() const;

		private:
			GLModel* _owner;
			std::unordered_map<LabelModel*, std::unique_ptr<LabelModel>> _labels;
		};
	}
}

#endif // LABELMANAGER_H