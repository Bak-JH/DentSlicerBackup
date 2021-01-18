#pragma once

#include "SupportModel.h"
#include "ModelAttachedSupport.h"
#include "BaseSupport.h"
#include "ContourModel.h"

using namespace Qt3DRender;
using namespace Qt3DExtras;


namespace Hix
{
	using namespace OverhangDetect;
	namespace Support
	{
		class VerticalSupportModel : public SupportModel, public BaseSupport, public ModelAttachedSupport, public ContourModel
		{
		public:
			VerticalSupportModel(SupportRaftManager* manager, const Overhang& overhang);
			virtual ~VerticalSupportModel();
			//BaseSupport
			bool hasBasePt()const override;
			const QVector3D& getBasePt()const override;
			//ModelAttachedSupport
			const Overhang& getOverhang()const override;
			std::optional<std::array<QVector3D, 2>> verticalSegment()override;


		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::FaceConstItr faceItr)override;
		private:
			QVector3D _basePt;
			Overhang _overhang;
			bool _hasBasePt = false;
			std::vector<QVector3D> _jointDir;
			void generateMesh();
			void generateSupportPath(float bottom, std::vector<float>& scales);

			std::optional <std::array<QVector3D, 2>> _vertSeg;
		};
	}
}
