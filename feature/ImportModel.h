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
		class ImportModel :public Hix::Features::Feature
		{
		public:
			ImportModel(QUrl fileUrl);
			virtual ~ImportModel();
		protected:
			void undoImpl()override;
			void redoImpl()override;
			void runImpl()override;
		private:
			QUrl _fileUrl;
			std::unique_ptr< Hix::Features::ListModel> _listModelFeature;
		};
		class ImportModelMode : public Hix::Features::Mode
		{
		public:
			ImportModelMode();
			virtual ~ImportModelMode();
		};
	}
}
