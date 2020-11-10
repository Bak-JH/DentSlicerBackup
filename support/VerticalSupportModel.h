#pragma once

#include "SupportModel.h"
#include "ModelAttachedSupport.h"
#include "BaseSupport.h"
#include "ContourModel.h"
using namespace Qt3DRender;
using namespace Qt3DExtras;


namespace Hix
{
	using namespace Engine3D;
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
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			QVector3D _basePt;
			QVector3D _attachmentStartPt;
			QVector3D _farthestOverhang;
			std::unordered_set<FaceConstItr> _attachmentTris;
			Overhang _overhang;
			bool _hasBasePt = false;
			void calculateAttachmentInfo(const QVector3D& supportNormal);

			void generateMesh();
			void generateSupportPath(float bottom, std::vector<float>& scales);
			void generateModelAttachment(Hix::Engine3D::Mesh& mesh);

			std::optional <std::array<QVector3D, 2>> _vertSeg;
		};
	}
}
