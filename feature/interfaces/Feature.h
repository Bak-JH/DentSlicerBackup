#pragma once
#include <unordered_set>
#include <memory>
#include <deque>
class GLModel;
namespace Hix
{
	namespace Features
	{


		class Feature
		{
		public:
			Feature();
			virtual ~Feature();
			virtual void undo() = 0;
			virtual void redo() = 0;

		};


		class FeatureContainer: virtual public Feature
		{
		public:
			FeatureContainer();
			void addFeature(Feature* feature);
			virtual ~FeatureContainer();
			void undo()override;
			void redo()override;
			const bool empty();
		protected:
			std::deque<std::unique_ptr<Feature>> _container;
		};

	}
}