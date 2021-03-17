#pragma once
#include "../../Mesh/mesh.h"
#include "../interfaces/Feature.h"
#include "../addModel.h"
class GLModel;
class QString;
namespace Hix
{

	namespace Features
	{
		void guessOrientation(Hix::Engine3D::Mesh& model, float& cuttingPlane, float& bottomPlane, QQuaternion& rotation);
		class TwoManifoldBuilder :public Hix::Features::Feature
		{
		public:
			TwoManifoldBuilder(Hix::Engine3D::Mesh& model, const QString& name, float cuttingPlane, float bottomPlane);
			virtual ~TwoManifoldBuilder();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		private:
			Hix::Engine3D::Mesh& _model;
			float _cuttingPlane;
			float _bottomPlane;
			QString _name;
			std::unique_ptr< Hix::Features::ListModel> _listModelFeature;
		};
	}
}
