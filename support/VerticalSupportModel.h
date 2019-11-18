#pragma once

#include "SupportModel.h"
#include "ModelAttachedSupport.h"
#include "BaseSupport.h"
using namespace Qt3DRender;
using namespace Qt3DExtras;


namespace Hix
{
	using namespace Engine3D;
	using namespace OverhangDetect;
	namespace Support
	{
		//heaxgon centered at 0,0,0, rotated around positive z-axis CW
		std::vector<QVector3D> generateHexagon(float radius);
		class VerticalSupportModel : public SupportModel, public BaseSupport, public ModelAttachedSupport
		{
		public:
			VerticalSupportModel(SupportRaftManager* manager, const Overhang& overhang);
			virtual ~VerticalSupportModel();
			//BaseSupport
			bool hasBasePt()const override;
			const QVector3D& getBasePt()const override;
			//ModelAttachedSupport
			const Overhang& getOverhang()const override;


		protected:
			QVector4D getPrimitiveColorCode(const Hix::Engine3D::Mesh* mesh, FaceConstItr faceItr)override;
		private:
			QVector3D _basePt;
			Overhang _overhang;
			bool _hasBasePt = false;
			void generateMesh();
			std::vector<QVector3D>  generateSupportPath(float bottom, std::vector<float>& scales);
		};
	}
}
