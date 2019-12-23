#pragma once
#include "../interfaces/Feature.h"
#include "../../DentEngine/src/Bounds3D.h"
#include "CrossSectionPlane.h"
namespace Hix
{
	namespace Features
	{
		class LayerView: public Hix::Features::Feature
		{
		public:
			LayerView(const std::unordered_set<GLModel*>& selectedModels, Hix::Engine3D::Bounds3D bound);
			virtual ~LayerView();
			void crossSectionSliderSignal(int value);
			void undoImpl() override {} //do nothing
			void redoImpl() override {} //do nothing
		private:
			//cutting
			Hix::Features::CrossSectionPlane _crossSectionPlane;
			std::unordered_set<GLModel*> _models;
			Hix::Engine3D::Bounds3D _modelsBound;
			int _maxLayer;
		};
	}
}