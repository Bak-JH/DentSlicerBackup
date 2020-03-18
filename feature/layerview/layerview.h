#pragma once
#include "../interfaces/Mode.h"
#include "../interfaces/SliderMode.h"
#include "../../DentEngine/src/Bounds3D.h"
#include "CrossSectionPlane.h"
namespace Hix
{
	namespace Features
	{
		class LayerView: public SliderMode
		{
		public:
			LayerView();
			virtual ~LayerView();
			void onExit()override;
		private:
			//cutting
			Hix::Features::CrossSectionPlane _crossSectionPlane;
			std::unordered_set<GLModel*> _models;
			Hix::Engine3D::Bounds3D _modelsBound;
			int _maxLayer;
		};


		class LayerviewPrep : public Hix::Tasking::Task
		{
		public:
			LayerviewPrep(const std::unordered_set<GLModel*>& selected, Hix::Features::CrossSectionPlane& crossSec);
			virtual ~LayerviewPrep();
			void run()override;
		private:
			std::unordered_set<GLModel*> _models;
			Hix::Features::CrossSectionPlane& _crossSec;

		};
	}
}
