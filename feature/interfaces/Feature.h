#pragma once
#include <unordered_set>
#include <memory>
#include <deque>
#include "../../Tasking/Task.h"
#include "Progress.h"
class GLModel;
class QObject;
namespace Hix
{
	namespace Features
	{

		class FeatureAttorney;
		class Feature: public Hix::Tasking::Task
		{
		public:
			Feature();
			virtual ~Feature();
			void undo()noexcept;
			void redo()noexcept;
			void run() override;

		protected:

			void tryRunFeature(Feature& other);
			void tryUndoFeature(Feature& other);
			void tryRedoFeature(Feature& other);
			virtual void runImpl() = 0;
			virtual void undoImpl() = 0;
			virtual void redoImpl() = 0;
			//friend class FeatureAttorney;
		};

		//class FeatureAttorney
		//{
		//private:
		//	static inline void runImpl(Feature& feature)
		//	{
		//		feature.runImpl();
		//	}
		//	static inline void undoImpl(Feature& feature)
		//	{
		//		feature.undoImpl();
		//	}
		//	static inline void redoImpl(Feature& feature)
		//	{
		//		feature.redoImpl();
		//	}
		//	friend class FeatureContainer;
		//	friend class A;
		//};
		
		class FeatureContainer: virtual public Feature
		{
		public:
			FeatureContainer();
			void addFeature(Feature* feature);
			virtual ~FeatureContainer();
			const bool empty();
			std::deque<std::unique_ptr<Feature>>& getContainer();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		protected:
			std::deque<std::unique_ptr<Feature>> _container;
		};


	}
}