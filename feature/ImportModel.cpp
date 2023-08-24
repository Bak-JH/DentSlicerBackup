#include "ImportModel.h"
#include <QFileDialog>
#include "../glmodel.h"
#include "addModel.h"
#include "repair/meshrepair.h"
#include "arrange/autoarrange.h"
#include "../application/ApplicationManager.h"
#include "zip/zip.h"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/filereadstream.h"
#include "stlexport.h"
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "feature/SupportFeature.h"

namespace fs = std::filesystem;
using namespace Hix::Settings;

Hix::Features::ImportModelMode::ImportModelMode()
{
	
	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).basicSetting;
	modSettings.parseJSON();
	QUrl latestUrl(QString(modSettings.importFilePath.c_str()));

	auto fileUrls = QFileDialog::getOpenFileUrls(nullptr, "Please choose 3D models or zipped export", latestUrl, "3D files(*.stl *.obj, *.zip)");

	for (auto& u : fileUrls)
	{
		if (!u.isEmpty())
		{
			Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new ImportModel(u));
			modSettings.importFilePath = u.adjusted(QUrl::RemoveFilename).toString().toStdString();
			modSettings.writeJSON();
		}
	}
}

Hix::Features::ImportModelMode::~ImportModelMode()
{
}



Hix::Features::ImportModel::ImportModel(QUrl fileUrl): _fileUrl(fileUrl)
{
	_progress.setDisplayText("Import Model");
}

Hix::Features::ImportModel::~ImportModel()
{
}

GLModel* Hix::Features::ImportModel::get()
{
	for (auto& f : _container)
	{
		auto listModel = dynamic_cast<ListModel*>(f.get());
		if (listModel)
		{
			return listModel->get();
		}
	}
}



void Hix::Features::ImportModel::runImpl()
{
	auto filename = _fileUrl.fileName();
	fs::path filePath(_fileUrl.toLocalFile().toStdU16String());

	if (boost::iequals(filePath.extension().string(), ".zip"))
	{
		std::ifstream zipStrm(filePath, std::ios_base::binary);
		miniz_cpp::zip_file zf(zipStrm);
		if (!zf.has_file(Hix::Features::STL_EXPORT_JSON))
			return;
		//get info json
		auto jsonStr = zf.read(Hix::Features::STL_EXPORT_JSON);
		rapidjson::Document document;
		document.Parse(jsonStr);

		std::unordered_map<std::string, LoadModelInfo> modelNameMap;
		Hix::Settings::SupportSetting tempSetting;
		bool raftActive = false;
		for (auto& m : document.GetObject())
		{
			//settings
			if (m.name == "settings")
			{
				auto settings = m.value.GetArray();
				tempSetting.supportRadiusMax = settings[0].GetDouble();
				tempSetting.supportRadiusMin = settings[1].GetDouble();
				tempSetting.supportDensity = settings[2].GetDouble();
				tempSetting.supportBaseHeight = settings[3].GetDouble();
				tempSetting.thickenFeet = settings[4].GetBool();

				tempSetting.raftThickness = settings[5].GetDouble();
				tempSetting.raftRadiusMult = settings[6].GetDouble();
				tempSetting.raftMinMaxRatio = settings[7].GetDouble();

				tempSetting.interconnectType = Hix::Settings::SupportSetting::InterconnectType(settings[8].GetInt());
				tempSetting.maxConnectDistance = settings[9].GetDouble();

				raftActive = settings[10].GetBool();
				continue;
			}

			//info array
			auto arr = m.value.GetArray();

			//model name
			auto modelname = arr[0].GetString();

			//position
			auto posArr = arr[1].GetArray();
			QVector3D pos(posArr[0].GetFloat(), posArr[1].GetFloat(), posArr[2].GetFloat());

			//supports
			std::vector<LoadSupportInfo> supports;
			for (auto idx = 2; idx < arr.Size(); ++idx)
			{
				Hix::Features::Extrusion::Contour contour;
				std::vector<QVector3D> jointDir;
				std::vector<float> scales;
				auto supportArr = arr[idx].GetArray();

				auto contourArr = supportArr[0].GetArray();
				auto jointDirArr = supportArr[1].GetArray();
				auto scalesArr = supportArr[2].GetArray();

				for (auto& pt : contourArr)
				{
					contour.push_back(QVector3D(pt[0].GetFloat(), pt[1].GetFloat(), pt[2].GetFloat()));
				}

				for (auto& pt : jointDirArr)
				{
					jointDir.push_back(QVector3D(pt[0].GetFloat(), pt[1].GetFloat(), pt[2].GetFloat()));
				}

				for (auto& scale : scalesArr)
				{
					scales.push_back(scale.GetFloat());
				}

				supports.push_back({ contour, jointDir, scales });
			}
			modelNameMap[m.name.GetString()] = { modelname, pos, supports };
		}

		//read each models
		for (auto& p : modelNameMap)
		{
			auto mesh = new Hix::Engine3D::Mesh();
			auto modelStr = zf.read(p.first);
			std::stringstream strStrm(modelStr);
			if (!FileLoader::loadMeshSTL(mesh, strStrm))
			{
				std::stringstream strStrmBin(modelStr, std::ios_base::in | std::ios_base::binary);
				FileLoader::loadMeshSTL_binary(mesh, strStrmBin);
			}

			Qt3DCore::QTransform* testtransform = new Qt3DCore::QTransform();
			testtransform->setTranslation(p.second.pos);
			auto model = createModel(mesh, QString::fromStdString(p.second.name), testtransform);
			model->setHitTestable(true);
			model->setHoverable(true);
			qDebug() << model->modelName();

			auto move = new Move(model, QVector3D(0, 0, tempSetting.raftThickness + tempSetting.supportBaseHeight));
			tryRunFeature(*move);
			addFeature(move);

			for (auto supportInfo : p.second.supports)
			{
				std::unique_ptr<SupportModel> supportModel;

				postUIthread([&] {
					auto& srMan = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
					supportModel = srMan.createSupportWithContour(model, supportInfo, tempSetting);
					});


				auto supportFeature = new Hix::Features::AddSupport(std::move(supportModel));
				tryRunFeature(*supportFeature);
				addFeature(supportFeature);

				if (raftActive)
				{
					auto addRaft = new AddRaft();
					tryRunFeature(*addRaft);
					addFeature(addRaft);
				}
			}
		}
	}
	else
	{
		auto stem = filePath.stem();
		QString name = name.fromStdWString(stem.wstring());
		importSingle(name, filePath);
	}
}

void Hix::Features::ImportModel::importSingle(const QString& name, const std::filesystem::path& path)
{

	auto mesh = new Hix::Engine3D::Mesh();
	std::fstream file(path);
	if (!file.is_open())
		return;

	if (boost::iequals(path.extension().string(), ".stl")) {

		if (!FileLoader::loadMeshSTL(mesh, file))
		{
			std::fstream fileBinary(path, std::ios_base::in | std::ios_base::binary);
			FileLoader::loadMeshSTL_binary(mesh, fileBinary);
		}
	}
	else if (boost::iequals(path.extension().string(), ".obj")) {
		FileLoader::loadMeshOBJ(mesh, file);
	}
	file.close();
	createModel(mesh, name);
}

GLModel* Hix::Features::ImportModel::createModel(Hix::Engine3D::Mesh* mesh, const QString& name, const Qt3DCore::QTransform* transform)
{
	mesh->centerMesh();
	auto listModel = new ListModel(mesh, name, transform);
	tryRunFeature(*listModel);
	addFeature(listModel);
	//repair mode
	if (Hix::Features::isRepairNeeded(mesh))
	{
		//Hix::Application::ApplicationManager::getInstance().setProgressText("Repairing mesh.");
		auto repair = new MeshRepair(listModel->get());
		tryRunFeature(*repair);
		addFeature(repair);
	}

	if (transform == nullptr)
	{
		auto bound = listModel->get()->recursiveAabb().centred();
		auto arrange = new AutoArrangeAppend(listModel->get());
		tryRunFeature(*arrange);
		addFeature(arrange);
	}

	return listModel->get();
}