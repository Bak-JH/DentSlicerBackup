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
			void undo();
			void redo();
			virtual void undoImpl() = 0;
			virtual void redoImpl() = 0;

		};


		class FeatureContainer: virtual public Feature
		{
		public:
			FeatureContainer();
			void addFeature(Feature* feature);
			virtual ~FeatureContainer();
			void undoImpl()override;
			void redoImpl()override;
			const bool empty();
		protected:
			std::deque<std::unique_ptr<Feature>> _container;
		};

	}
}