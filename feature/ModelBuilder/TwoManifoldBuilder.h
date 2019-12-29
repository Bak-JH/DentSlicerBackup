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
			TwoManifoldBuilder(Hix::Engine3D::Mesh* model, const QString& name, float cuttingPlane, float bottomPlane);
			TwoManifoldBuilder(Hix::Engine3D::Mesh& model);
			virtual ~TwoManifoldBuilder();

		private:
			Hix::Engine3D::Mesh& _model;
			std::unordered_map<Hix::Engine3D::FaceConstItr, QVector3D> _fnCache;
		};
	}
}
