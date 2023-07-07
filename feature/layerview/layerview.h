#pragma once
#include "../interfaces/Mode.h"
#include "../interfaces/RangeSliderMode.h"
#include "../../render/Bounds3D.h"
#include "CrossSectionPlane.h"
namespace Hix
{
	namespace Render
	{
		class SceneEntityWithMaterial;
	}
	namespace Features
	{
		class LayerView: public RangeSliderMode
		{
		public:
			LayerView();
			virtual ~LayerView();
			void onExit()override;
		private:
			std::unordered_map<Hix::Render::SceneEntityWithMaterial*, QVector4D> _modelColorMap;
			Hix::Features::CrossSectionPlane _topCrossSectionPlane;
			Hix::Features::CrossSectionPlane _botCrossSectionPlane;
			std::unordered_set<GLModel*> _models;
		};


		class LayerviewPrep : public Hix::Tasking::Task
		{
		public:
			LayerviewPrep(const std::unordered_map<Hix::Render::SceneEntityWithMaterial*, QVector4D>& selected, Hix::Features::CrossSectionPlane& crossSec, bool isTop);
			virtual ~LayerviewPrep();
			void run()override;
		private:
			const std::unordered_map<Hix::Render::SceneEntityWithMaterial*, QVector4D>& _modelColorMap;
			Hix::Features::CrossSectionPlane& _crossSec;
			bool _isTop;
		};
	}
}
