#include "sliceExport.h"
#include <QFileDialog>

#include "../glmodel.h"
#include "addModel.h"
#include "../application/ApplicationManager.h"
#include <unordered_set>
constexpr float ZMARGIN = 5;


Hix::Features::SliceExportMode::SliceExportMode()
{
	auto fileUrl = QFileDialog::getSaveFileUrl(nullptr, "Export sliced file", QUrl());
	auto path = fileUrl.path();
	auto fileName = fileUrl.fileName();
	if (fileName.isEmpty())
	{
		return;
	}
	auto se = new SliceExport(Hix::Application::ApplicationManager::getInstance().partManager().allModels(), path);
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(se);

}

Hix::Features::SliceExportMode::~SliceExportMode()
{
}

Hix::Features::SliceExport::SliceExport(const std::unordered_set<GLModel*>& selected): _models(selected), _path(QDir::tempPath()), _isTemp(true)
{
}

Hix::Features::SliceExport::SliceExport(const std::unordered_set<GLModel*>& selected, QString path): _models(selected), _path(path), _isTemp(false)
{
}

void Hix::Features::SliceExport::run()
{
	// need to generate support, raft
	auto selectedBound = Hix::Engine3D::combineBounds(_models);
	_result = SlicingEngine::sliceModels(_isTemp, selectedBound.zMax(), _models, Hix::Application::ApplicationManager::getInstance().supportRaftManager(), _path);

}

SlicingEngine::Result Hix::Features::SliceExport::getResult()
{
	return _result;
}
