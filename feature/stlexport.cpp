#include "stlexport.h"
#include <qfiledialog.h>
#include "application/ApplicationManager.h"
#include "support/RaftModel.h"
#include "glmodel.h"
#include "STL/stlio.hpp"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "zip/zip.h"
using namespace Hix::Engine3D;
using namespace Hix::Features;
using namespace Hix::Render;
using namespace tyti::stl;
const constexpr auto STL_TEMP_PATH = "hix_temp_stl";

#if defined(_DEBUG)
//#define SAVE_ASCII
#endif
Hix::Features::STLExportMode::STLExportMode()
{
	auto models = Hix::Application::ApplicationManager::getInstance().partManager().allModels();

	auto& modSettings = Hix::Application::SettingsChanger::settings(Hix::Application::ApplicationManager::getInstance()).basicSetting;
	modSettings.parseJSON();
	QString latestUrl(modSettings.exportFilePath.c_str());

	QString fileName;
	if (models.size() == 1)
	{
		fileName = QFileDialog::getSaveFileName(nullptr, "Save to STL file", latestUrl, "3D Model file (*.stl)");

	}
	else if (models.size() > 1)
	{
		fileName = QFileDialog::getSaveFileName(nullptr, "Save to STL Zip file", latestUrl, "3D Model Collection(*.zip)");

	}
    if (fileName.isEmpty())
    {
        return;
    }
    auto se = new STLExport(models, fileName);
    Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::unique_ptr<STLExport>(se));

	modSettings.exportFilePath = QUrl(fileName).adjusted(QUrl::RemoveFilename).toString().toStdString();
	modSettings.writeJSON();
}

Hix::Features::STLExportMode::~STLExportMode()
{
}


STLExport::STLExport(const std::unordered_set<GLModel*>& selected, QString path): _models(selected), _path(path.toStdU16String())
{
}

Hix::Features::STLExport::~STLExport()
{
}

void Hix::Features::STLExport::run()
{

	_tmpPath = std::filesystem::temp_directory_path().append(STL_TEMP_PATH);
	try {
		std::filesystem::remove(_tmpPath);
	}
	catch (...) {}
	std::filesystem::create_directory(_tmpPath);

	exportModels();
}

std::unordered_set<const SceneEntity*> getSuppRaft()
{
	auto& supp = Hix::Application::ApplicationManager::getInstance().supportRaftManager();
	std::unordered_set<const SceneEntity*> entities;

	if (supp.supportActive())
	{
		auto supps = supp.supportModels();
		for (auto& e : supps)
		{
			entities.insert(e);
		}
		if (supp.raftActive())
		{
			auto raft = supp.raftModel();
			entities.insert(dynamic_cast<const SceneEntity*>(raft));
		}
	}
	return entities;
}

void Hix::Features::STLExport::exportModels()
{
	for (auto& m : _models)
	{
		//insert to map
		_modelsMap[_currIdx] = m;
		std::unordered_set<const GLModel*> children;
		m->getChildrenModels(children);
		children.insert(m);
		std::unordered_set<const SceneEntity*> entities;
		for (auto& e : children)
		{
			entities.insert(e);
		}

#ifdef SAVE_ASCII
		//if (_models.size() == 1)
		//{
		//	auto supp = getSuppRaft();
		//	for (auto& e : supp) entities.insert(e);
		//}
		exportSTLAscii(entities, _currIdx);
#else
		//if (_models.size() == 1)
		//{
		//	auto supp = getSuppRaft();
		//	for (auto& e : supp) entities.insert(e);
		//}
		exportSTLBin(entities, _currIdx);

#endif
		++_currIdx;
	}

	//zip files if there are multiple models in the scene
	if (_modelsMap.size() > 1)
	{
		//support 
		//auto supp = getSuppRaft();
		//exportSTLBin(supp, ++_currIdx);

		//write json
		auto jsonPath = _tmpPath;
		jsonPath /= STL_EXPORT_JSON;
		rapidjson::Document doc;
		doc.SetObject();

		//compress files
		miniz_cpp::zip_file file;
		for (size_t i = 0; i != _currIdx; ++i)
		{
			rapidjson::Value k(getNthModelName(i), doc.GetAllocator());
			if (_modelsMap.find(i) != _modelsMap.cend())
			{
				rapidjson::Value v(_modelsMap[i]->modelName().toStdString(), doc.GetAllocator());
				doc.AddMember(k, v, doc.GetAllocator());
			}
			//add to zip archive
			auto modelPath = _tmpPath;
			modelPath /= getNthModelName(i);
			file.write(modelPath.string(), modelPath.filename().string());
		}
		std::ofstream of(jsonPath, std::ios_base::trunc);
		rapidjson::OStreamWrapper osw{ of };
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{ osw };
		doc.Accept(writer);
		file.write(jsonPath.string(), jsonPath.filename().string());
		file.comment = "hix scene file";
		std::ofstream zipOut(_path, std::ios_base::trunc | std::ios::binary);
		file.save(zipOut);

	}
	else
	{
		auto tmpFile = _tmpPath;
		_tmpPath /= getNthModelName(0);
		std::filesystem::copy_file(_tmpPath, _path);
	}

}

tyti::stl::vec3 toExportVec(const QVector3D& vec)
{
	return vec3(vec.x(), vec.y(), vec.z());
}

void Hix::Features::STLExport::exportSTLBin(const std::unordered_set<const SceneEntity*>& childs, size_t idx)
{
	auto path = _tmpPath;
	path /= getNthModelName(idx);
	std::ofstream ofs(path, std::ios_base::trunc|std::ios::binary);

	tyti::stl::basic_solid<float> exportSolid;
	size_t faceCnt = 0;
	for (auto& c : childs)
	{
		faceCnt += c->getMesh()->getFaces().size();
	}
	exportSolid.normals.reserve(faceCnt);
	exportSolid.vertices.reserve(faceCnt * 3);
	for (auto& c : childs)
	{
		auto mesh = c->getMesh();
		if (mesh)
		{
			auto& faces = mesh->getFaces();
			auto faceCend = faces.cend();
			for (auto mf = faces.cbegin(); mf != faceCend; ++mf)
			{
				auto normal = mf.worldFn();
				auto meshVertices = mf.meshVertices();
				auto& mv1 = meshVertices[0];
				auto& mv2 = meshVertices[1];
				auto& mv3 = meshVertices[2];
				exportSolid.normals.emplace_back(toExportVec(normal));
				exportSolid.vertices.emplace_back(toExportVec(mv1.worldPosition()));
				exportSolid.vertices.emplace_back(toExportVec(mv2.worldPosition()));
				exportSolid.vertices.emplace_back(toExportVec(mv3.worldPosition()));
			}
		}
	}
	tyti::stl::write_binary(ofs, exportSolid);
	ofs.close();
	return;
}

void STLExport::exportSTLAscii(const std::unordered_set<const SceneEntity*>& childs, size_t idx)
{
	auto path = _tmpPath;
	path /= getNthModelName(idx);
	std::ofstream ofs(path, std::ios_base::trunc);
    writeHeader(ofs);
	for (auto& c : childs)
	{
		auto mesh = c->getMesh();
		if (mesh)
		{
			auto faceCend = mesh->getFaces().cend();
			for (auto mf = mesh->getFaces().cbegin(); mf != faceCend; ++mf)
			{
				writeFace(mesh, mf, ofs);
			}
		}
	}
    writeFooter(ofs);
	ofs.close();
    return;
}

void STLExport::writeFace(const Mesh* mesh, const Hix::Engine3D::FaceConstItr& mf, std::ostream& content){

    content << "    outer loop\n";
    auto meshVertices = mf.meshVertices();

    auto& mv1 = meshVertices[0];
    auto& mv2 = meshVertices[1];
    auto& mv3 = meshVertices[2];
    content << "        vertex "<< mv1.worldPosition().x()<<" "<< mv1.worldPosition().y()<<" "<< mv1.worldPosition().z()<<"\n";
    content << "        vertex "<< mv2.worldPosition().x()<<" "<< mv2.worldPosition().y()<<" "<< mv2.worldPosition().z()<<"\n";
    content << "        vertex "<< mv3.worldPosition().x()<<" "<< mv3.worldPosition().y()<<" "<< mv3.worldPosition().z()<<"\n";
    content << "    endloop\n";
    content << "endfacet\n";
}

void STLExport::writeHeader(std::ostream& content){
    content << "solid hixsolid\n";
}

void STLExport::writeFooter(std::ostream& content){
    content << "endsolid hixsolid";
}

std::string Hix::Features::STLExport::getNthModelName(size_t idx)
{
	return std::to_string(idx) + ".stl";
}
