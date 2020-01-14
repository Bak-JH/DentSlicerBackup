#include "sliceExport.h"
#include <QFileDialog>
#include "../qmlmanager.h"
#include "../glmodel.h"
#include "addModel.h"
#include <unordered_set>
constexpr float ZMARGIN = 5;


Hix::Features::ExportMode::ExportMode()
{
	auto fileUrl = QFileDialog::getSaveFileUrl(nullptr, "Export sliced file", QUrl());
	auto path = fileUrl.path();
	auto fileName = fileUrl.fileName();
	if (fileName.isEmpty())
	{
		return;
	}
	auto se = new SliceExport(qmlManager->getSelectedModels(), path);
	qmlManager->taskManager().enqueTask(se);

}

Hix::Features::ExportMode::~ExportMode()
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
	std::vector<std::reference_wrapper<const GLModel>> constSelectedModels;
	constSelectedModels.reserve(_models.size());
	std::transform(std::begin(_models), std::end(_models), std::back_inserter(constSelectedModels),
		[](GLModel* ptr)-> std::reference_wrapper<const GLModel> {
			return std::cref(*ptr);
		});

	auto selectedBound = Hix::Engine3D::combineBounds(_models);
	_result = SlicingEngine::sliceModels(_isTemp, selectedBound.zMax(), constSelectedModels, qmlManager->supportRaftManager(), _path);

}

SlicingEngine::Result Hix::Features::SliceExport::getResult()
{
	return _result;
}
