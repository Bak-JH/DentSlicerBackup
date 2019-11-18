#pragma once

#include "interfaces/Feature.h"
#include "glmodel.h"
#include "cut/DrawingPlane.h"

namespace Hix
{
	namespace Features
	{
		class ShellOffset : public Hix::Features::Feature
		{
		public:
			ShellOffset(GLModel* glmodel);
			virtual ~ShellOffset();
			void getSliderSignal(double value);
			void doOffset(float offset);
			void undo() override {} //TODO:undo
		private:
			GLModel* _subject = nullptr;
			float _zPlane = 0;
			void doOffsetImpl(GLModel* glModel, float offset, float zPlane);
			Hix::Engine3D::Bounds3D _modelBound;
			Hix::Features::Cut::DrawingPlane _cuttingPlane;

		};
	}
};

