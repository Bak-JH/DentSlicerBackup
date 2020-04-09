#include "ImportModel.h"
#include <QFileDialog>
#include "../glmodel.h"
#include "addModel.h"
#include "repair/meshrepair.h"
#include "arrange/autoarrange.h"
#include "../application/ApplicationManager.h"
#include "zip/zip_file.hpp"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/filereadstream.h"
#include "stlexport.h"
#include <fstream>
namespace fs = std::filesystem;

Hix::Features::ImportModelMode::ImportModelMode()
{

	auto fileUrls = QFileDialog::getOpenFileUrls(nullptr, "Please choose 3D models", QUrl(), "3D files(*.stl *.obj)");
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
	fs::path filePath(_fileUrl.toLocalFile().toStdWString());

	if (fs::equivalent(filePath.extension(), ".zip"))
	{
		std::fstream f(filePath);
		miniz_cpp::zip_file zf(f);
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
			auto modelStr = zf.read(p.first);
			std::stringstream strStrm(modelStr, std::ios_base::in | std::ios_base::binary);
			auto mesh = new Mesh();
			FileLoader::loadMeshSTL_binary(mesh, strStrm);
			createModel(mesh, p.second);
		}
	}
	else
	{
		importSingle(filePath.stem().string(), filePath);
	}
}

void Hix::Features::ImportModel::importSingle(const std::string& name, const std::filesystem::path& path)
{

	auto mesh = new Mesh();
	std::fstream file(path);
	if (!file.is_open())
		return;

	if (fs::equivalent(path.extension(), ".stl")) {

		if (FileLoader::loadMeshSTL(mesh, file))
		{
			std::fstream fileBinary(path, std::ios_base::in | std::ios_base::binary);
			FileLoader::loadMeshSTL_binary(mesh, fileBinary);
		}
	}
	else if (fs::equivalent(path.extension(), ".obj")) {
		FileLoader::loadMeshOBJ(mesh, file);
	}
	file.close();
	createModel(mesh, name);
}

void Hix::Features::ImportModel::createModel(Hix::Engine3D::Mesh* mesh, const std::string& name)
{
	GLModel::filenameToModelName(name);
	mesh->centerMesh();

	auto listModel = new ListModel(mesh, QString::fromStdString(name), nullptr);
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

	auto bound = listModel->get()->recursiveAabb();
	const auto& printBound = Hix::Application::ApplicationManager::getInstance().settings().printerSetting.bedBound;
	if (printBound.contains(bound))
	{
		auto arrange = new AutoArrangeAppend(listModel->get());
		tryRunFeature(*arrange);
		addFeature(arrange);
	}
}
