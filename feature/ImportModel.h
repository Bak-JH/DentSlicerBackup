#pragma once
#include "interfaces/Mode.h"`
#include "render/Bounds3D.h"
#include "../../Mesh/mesh.h"
#include <qurl.h>
#include <filesystem>
class GLModel;
class QString;
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
			void importSingle(const QString& name, const std::filesystem::path& path);
			GLModel* createModel(Hix::Engine3D::Mesh* mesh, const QString& name, const Qt3DCore::QTransform* transform = nullptr);
		};
		class ImportModelMode : public Hix::Features::InstantMode
		{
		public:
			ImportModelMode();
			virtual ~ImportModelMode();
		};
	}
}
