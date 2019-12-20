#pragma once

#include "interfaces/Feature.h"
#include "glmodel.h"
#include "cut/DrawingPlane.h"

namespace Hix
{
	namespace Features
	{
		class ShellOffsetMode : public Hix::Features::Mode
		{
		public:
			ShellOffsetMode(GLModel* glmodel);
			virtual ~ShellOffsetMode();
			void getSliderSignal(double value);
			Hix::Features::Feature* doOffset(float offset);
		private:
			GLModel* _subject = nullptr;
			float _zPlane = 0;
			Hix::Engine3D::Bounds3D _modelBound;
			Hix::Features::Cut::DrawingPlane _cuttingPlane;

		};

		class ShellOffset : public Hix::Features::Feature
		{
		public:
			ShellOffset(GLModel* target, float offset, float zPlane);
			virtual ~ShellOffset();
			void undo()override;
			void redo()override;

		private:
			Mesh* _prevMesh;
			FeatureContainer* _container;
		};
	}
};

