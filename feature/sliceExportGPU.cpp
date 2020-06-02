#include "../slice/gpuSlicer.h"

#include "sliceExportGPU.h"
#include <QFileDialog>
#include "../glmodel.h"
#include "addModel.h"
#include "../application/ApplicationManager.h"
#include "../Qml/components/Inputs.h"
#include "../Qml/components/Buttons.h"
#include "../slice/InfoWriter.h"	
#include <unordered_set>
constexpr float ZMARGIN = 5;
using namespace Hix::Settings;



Hix::Features::SliceExportGPU::SliceExportGPU(const std::unordered_set<GLModel*>& selected, QString path): _models(selected), _path(path)
{
}

void Hix::Features::SliceExportGPU::run()
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

	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;
	auto& printerSetting = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;


	// Export to SVG
	Hix::Slicer::SlicerGL slicer(setting.layerHeight, filename.toStdString(), setting.AAXY, setting.AAZ);
	slicer.setScreen(printerSetting.pixelPerMMX(), printerSetting.sliceImageResolutionX, printerSetting.sliceImageResolutionY);
	slicer.addSubject(_models, Hix::Application::ApplicationManager::getInstance().supportRaftManager(), printerSetting.bedOffsetX, printerSetting.bedOffsetY, setting.invertX);
	auto layerCnt = slicer.run();
	//write info files
	Hix::Slicer::InfoWriter iw(filename, printerSetting.sliceImageResolutionX, printerSetting.sliceImageResolutionY, setting.layerHeight);
	iw.createInfoFile();
	iw.writeBasicInfo(layerCnt, printerSetting.printerConstants);
	
	if (printerSetting.infoFileType == Hix::Settings::PrinterSetting::InfoFileType::ThreeDelight)
	{
		iw.writeVittroOptions(layerCnt, printerSetting.bedBound);
	}




}

