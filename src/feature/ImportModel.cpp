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

namespace fs = std::filesystem;

Hix::Features::ImportModelMode::ImportModelMode()
{

	auto fileUrls = QFileDialog::getOpenFileUrls(nullptr, "Please choose 3D models or zipped export", QUrl(), "3D files(*.stl *.obj, *.zip)");
	for (auto& u : fileUrls)
	{
		if (!u.isEmpty())
		{
			Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(new ImportModel(u));
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
		std::unordered_map<std::string, std::string> modelNameMap;
		for (auto& m : document.GetObject())
		{
			modelNameMap[m.name.GetString()] = m.value.GetString();
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
			createModel(mesh, QString::fromStdString(p.second));
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

void Hix::Features::ImportModel::createModel(Hix::Engine3D::Mesh* mesh, const QString& name)
{
	mesh->centerMesh();
	auto listModel = new ListModel(mesh,name, nullptr);
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

	auto bound = listModel->get()->recursiveAabb().centred();
	auto arrange = new AutoArrangeAppend(listModel->get());
	tryRunFeature(*arrange);
	addFeature(arrange);
}
