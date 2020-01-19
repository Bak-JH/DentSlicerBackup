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

		class HollowMesh : public Hix::Features::Feature
		{
		public:
			HollowMesh(GLModel* target, float offset);
			virtual ~HollowMesh();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;

		private:
			float _offset;
			GLModel* _target;
			std::unique_ptr<Hix::Engine3D::Mesh> _prevMesh;
		};

		class ShellOffset : public Hix::Features::FeatureContainer
		{
		public:
			ShellOffset(GLModel* target, float offset, float zPlane);
			virtual ~ShellOffset();
		protected:
			void runImpl()override;

		private:
			float _offset;
			float _zPlane;
			GLModel* _target;
		};
	}
};

