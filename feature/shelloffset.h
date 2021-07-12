#pragma once

#include "interfaces/Feature.h"
#include "glmodel.h"
#include "interfaces/DialogedMode.h"

#include "Mesh/BVH.h"
#include "Mesh/MTRayCaster.h"

namespace Hix
{
	namespace QML
	{
		namespace Controls
		{
			class InputSpinBox;
		}
	}

	namespace Features
	{
		class ShellOffsetMode : public DialogedMode
		{
		public:
			ShellOffsetMode();
			virtual ~ShellOffsetMode();
			void applyButtonClicked()override;
		private:
			GLModel* _subject = nullptr;
			Hix::Engine3D::Bounds3D _modelBound;
			Hix::QML::Controls::InputSpinBox* _offsetValue;
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
			void uniformSampling(float offset);

		private:
			float _offset;
			GLModel* _target;
			std::unique_ptr<Hix::Engine3D::Mesh> _prevMesh;

			std::unique_ptr<Hix::Engine3D::RayCaster> _rayCaster;

			std::unique_ptr<Hix::Engine3D::RayAccelerator> _rayAccel;
		};

		class ShellOffset : public Hix::Features::FeatureContainer
		{
		public:
			ShellOffset(GLModel* target, float offset);
			virtual ~ShellOffset();
		protected:
			void runImpl()override;

		private:
			float _offset;
			GLModel* _target;
		};
	}
};

