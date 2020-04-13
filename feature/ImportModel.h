#pragma once
#include "interfaces/Mode.h"`
#include "DentEngine/src/Bounds3D.h"
#include "../../DentEngine/src/Mesh.h"
#include <qurl.h>
#include <filesystem>
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
			GLModel* get();
		protected:

			void runImpl()override;
		private:
			QUrl _fileUrl;
			void importSingle(const std::string& name, const std::filesystem::path& path);
			void createModel(Hix::Engine3D::Mesh* mesh, const std::string& name);
		};
		class ImportModelMode : public Hix::Features::InstantMode
		{
		public:
			ImportModelMode();
			virtual ~ImportModelMode();
		};
	}
}
