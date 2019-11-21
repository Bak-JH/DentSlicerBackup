#pragma once
#include "../DentEngine/src/Mesh.h"
#include "interfaces/Feature.h"
class GLModel;

namespace Hix
{

	namespace Features
	{

		class TwoManifoldBuilder : public Hix::Features::Feature
		{
		public:
			TwoManifoldBuilder(Hix::Engine3D::Mesh& model);
		private:
			const Hix::Engine3D::Mesh& _model;
			std::unordered_map<Hix::Engine3D::FaceConstItr, QVector3D> _fnCache;
		};
	}
}
