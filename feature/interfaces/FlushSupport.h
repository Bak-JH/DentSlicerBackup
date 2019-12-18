#pragma once
#include "Feature.h"

namespace Hix
{
	namespace Features
	{
		class FlushSupport
		{
		public:
			FlushSupport();
			//FlushSupport(GLModel* model);

			virtual ~FlushSupport();
		};
		class FeatureContainerFlushSupport : public FeatureContainer, public FlushSupport
		{};
	}
}

