#pragma once
#include "Feature.h"
#include <unordered_set>
class GLModel;
namespace Hix
{
	namespace Features
	{
		class FlushSupport
		{
		public:
			FlushSupport(std::unordered_set<GLModel*>& models);
			FlushSupport(GLModel* model);

			virtual ~FlushSupport();
		};
		class FeatureContainerFlushSupport : public FeatureContainer, public FlushSupport
		{
		public:
			FeatureContainerFlushSupport(std::unordered_set<GLModel*>& models);
			FeatureContainerFlushSupport(GLModel* model);
			virtual ~FeatureContainerFlushSupport();
		};
	}
}

