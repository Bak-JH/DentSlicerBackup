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
			float _resolution = 2.0f;
			GLModel* _target;

			std::unique_ptr<Hix::Engine3D::Mesh> _prevMesh;
			std::unique_ptr<Hix::Engine3D::RayCaster> _rayCaster;
			std::unique_ptr<Hix::Engine3D::RayAccelerator> _rayAccel;

			std::vector<float> _SDF;
			Hix::Engine3D::Bounds3D _samplingBound;

			Hix::Engine3D::RayHits getRayHitPoints(QVector3D rayOrigin, QVector3D rayDirection);
			float getSDFValue(QVector3D point);
			QVector3D VertexInterp(float isolevel, QVector3D p1, QVector3D p2, float valp1, float valp2);

		};
	}
};

