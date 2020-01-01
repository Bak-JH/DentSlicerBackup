#pragma once
#include "../../DentEngine/src/Mesh.h"
#include "../interfaces/Feature.h"
class GLModel;
class QString;
namespace Hix
{

	namespace Features
	{

		class TwoManifoldBuilder :public Hix::Features::FeatureContainer
		{
		public:
			TwoManifoldBuilder(Hix::Engine3D::Mesh& model);
			void autogenOrientation(float& cuttingPlane, float& bottomPlane, QQuaternion& rotation);
			void buildModel(const QString& name, float cuttingPlane, float bottomPlane);
			GLModel* result();
			virtual ~TwoManifoldBuilder();

		private:
			Hix::Engine3D::Mesh& _model;
			GLModel* _result;
		};
	}
}
