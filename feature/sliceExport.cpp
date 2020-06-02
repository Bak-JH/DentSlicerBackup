#include "sliceExport.h"
#include <QFileDialog>

#include "../glmodel.h"
#include "addModel.h"
#include "../application/ApplicationManager.h"
#include "DentEngine/src/SlicerDebug.h"
#include "DentEngine/src/svgexporter.h"
#include "../Qml/components/Inputs.h"
#include "../Qml/components/Buttons.h"
#include "../slice/InfoWriter.h"
#include <unordered_set>
constexpr float ZMARGIN = 5;
using namespace Hix::Settings;


const QUrl POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupSlice.qml");

Hix::Features::SliceExportMode::SliceExportMode()
:DialogedMode(POPUP_URL)

{
	auto& co = controlOwner();
	co.getControl(_sliceTypeDrop, "sliceType");
	co.getControl(_layerHeightSpin, "layerHeight");
	co.getControl(_invertXSwtch, "invertX");


	auto& settings = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;
	_sliceTypeDrop->setEnums<SliceSetting::SlicingMode>(settings.slicingMode);
	_layerHeightSpin->setValue(settings.layerHeight);
	_invertXSwtch->setChecked(settings.invertX);

	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).sliceSetting;
	
	// bind inputs
	QObject::connect(_sliceTypeDrop, &Hix::QML::Controls::DropdownBox::indexChanged, [this, &modSettings]() {
		_sliceTypeDrop->getSelected(modSettings.slicingMode);
		});
	QObject::connect(_layerHeightSpin, &Hix::QML::Controls::InputSpinBox::valueChanged, [this, &modSettings]() {
		modSettings.layerHeight = _layerHeightSpin->getValue();
		});
	QObject::connect(_aaxySpin, &Hix::QML::Controls::InputSpinBox::valueChanged, [this, &modSettings]() {
		modSettings.AAXY = _aaxySpin->getValue();
		});
	QObject::connect(_aazSpin, &Hix::QML::Controls::InputSpinBox::valueChanged, [this, &modSettings]() {
		modSettings.AAZ = _aazSpin->getValue();
		});
	
	QObject::connect(_invertXSwtch, &Hix::QML::Controls::ToggleSwitch::checkedChanged, [this, &modSettings]() {
		modSettings.invertX = _invertXSwtch->isChecked();
		});
}

Hix::Features::SliceExportMode::~SliceExportMode()
{
}

void Hix::Features::SliceExportMode::applyButtonClicked()
{
	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).sliceSetting;
	modSettings.writeJSON();
	auto fileName = QFileDialog::getSaveFileName(nullptr, "Export sliced file", "");
	if (fileName.isEmpty())
	{
		return;
	}
	auto se = new SliceExport(Hix::Application::ApplicationManager::getInstance().partManager().allModels(), fileName);
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::unique_ptr<SliceExport>(se));
}

void Hix::Features::SliceExportMode::applyAndClose()
{
	applyButtonClicked();
	//delete schedule last due to the blocking file dialog
	scheduleForDelete();
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
	auto& setting = Hix::Application::ApplicationManager::getInstance().settings().sliceSetting;
	auto& printerSetting = Hix::Application::ApplicationManager::getInstance().settings().printerSetting;

	auto selectedBound = Hix::Engine3D::combineBounds(_models);
	auto layerGroups = SlicingEngine::sliceModels(_models, Hix::Application::ApplicationManager::getInstance().supportRaftManager(), setting.layerHeight);

	// Export to SVG
	Hix::Slicer::SVGexporter exp(setting.layerHeight, printerSetting.pixelPerMMX(), printerSetting.pixelPerMMY(), 
		printerSetting.sliceImageResolutionX, printerSetting.sliceImageResolutionY, QVector2D(printerSetting.bedOffsetX, printerSetting.bedOffsetY),
		setting.invertX, filename, setting.slicingMode);

	exp.exportSVG(layerGroups);


	//write info files
	Hix::Slicer::InfoWriter iw(filename, printerSetting.sliceImageResolutionX, printerSetting.sliceImageResolutionY, setting.layerHeight);
	iw.createInfoFile();
	iw.writeBasicInfo(layerGroups.size(), printerSetting.printerConstants);
	
	if (printerSetting.infoFileType == Hix::Settings::PrinterSetting::InfoFileType::ThreeDelight)
	{
		iw.writeVittroOptions(layerGroups.size(), printerSetting.bedBound);
	}


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

