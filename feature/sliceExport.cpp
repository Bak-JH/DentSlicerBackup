#include "sliceExport.h"
#include <QFileDialog>

#include "../glmodel.h"
#include "addModel.h"
#include "../application/ApplicationManager.h"
#include "DentEngine/src/SlicerDebug.h"
#include "DentEngine/src/svgexporter.h"

#include <unordered_set>
constexpr float ZMARGIN = 5;



Hix::Features::SliceExportMode::SliceExportMode()
{
	auto fileName = QFileDialog::getSaveFileName(nullptr, "Export sliced file", "");
	if (fileName.isEmpty())
	{
		return;
	}
	auto se = new SliceExport(Hix::Application::ApplicationManager::getInstance().partManager().allModels(), fileName);
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::unique_ptr<SliceExport>(se));
	scheduleForDelete();
}

Hix::Features::SliceExportMode::~SliceExportMode()
{
}



Hix::Features::SliceExport::SliceExport(const std::unordered_set<GLModel*>& selected, QString path): _models(selected), _path(path)
{
}

void Hix::Features::SliceExport::run()
{
	// need to generate support, raft
	 auto filename = _path + "_export";
	QDir dir(filename);
	if (!dir.exists()) {
		dir.mkpath(".");
	}
	else {
		dir.removeRecursively();
		dir.mkpath(".");
	}
	//debug log
	if (Hix::Slicer::Debug::SlicerDebug::getInstance().enableDebug)
	{
		Hix::Slicer::Debug::SlicerDebug::getInstance().debugFilePath = filename + QString("/debug/");
		QDir debugDir(Hix::Slicer::Debug::SlicerDebug::getInstance().debugFilePath);
		if (!debugDir.exists()) {
			debugDir.mkpath(".");
		}
	}

	auto selectedBound = Hix::Engine3D::combineBounds(_models);
	auto shellSlices = SlicingEngine::sliceModels(_models, Hix::Application::ApplicationManager::getInstance().supportRaftManager());

	// Export to SVG
	SVGexporter exp;
	exp.exportSVG(shellSlices, filename, false);


	//int layer = shellSlices.size();
	//int time = layer * 15 / 60;
	//auto bounds = selectedBound.lengths();
	//int64_t area = 0;
	//float delta = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting.layerHeight;

	//float volume = ((float)(area / pow(Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelPerMMX(), 2)) / 1000000) * delta;
	////Hix::Application::ApplicationManager::getInstance().setProgress(1);
	//QStringList name_word = filename.split("/");

	//QString size;
	//size.sprintf("%.1f X %.1f X %.1f mm", bounds.x(), bounds.y(), bounds.z());
	////Hix::Application::ApplicationManager::getInstance().openResultPopUp("",
	////                            QString(name_word[name_word.size()-1]+" slicing done.").toStdString(),
	////                            "");
	//return { time , layer, size, volume };
}

