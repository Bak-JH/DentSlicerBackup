#pragma once
#include "interfaces/Mode.h"`
#include "DentEngine/src/Bounds3D.h"
#include "../../DentEngine/src/Mesh.h"
#include <qurl.h>
class GLModel;

namespace Hix
{

	namespace Features
	{
		class ListModel;
		class ImportModel :public FeatureContainer
		{
		public:
			ImportModel(QUrl fileUrl);
			virtual ~ImportModel();
		protected:

			void runImpl()override;
		private:
			QUrl _fileUrl;
		};
		class ImportModelMode : public Hix::Features::InstantMode
		{
		public:
			ImportModelMode();
			virtual ~ImportModelMode();
		};
	}
}
