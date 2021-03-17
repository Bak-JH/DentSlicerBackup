#pragma once

#include "Interconnect.h"
#include "ContourModel.h"
namespace Hix
{
	using namespace OverhangDetect;
	namespace Support
	{
		class SupportRaftManager;
		class DiagInterconnect : virtual public Interconnect, public ContourModel
		{
		public:
			DiagInterconnect(SupportRaftManager* manager, std::array<QVector3D, 2> pts, std::array<SupportModel*, 2> connectedModels);
			virtual ~DiagInterconnect();


		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)override;
		private:
			std::array<QVector3D, 2> _pts;
			void generateMesh();
		};
	}
}
