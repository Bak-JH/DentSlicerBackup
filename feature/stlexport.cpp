#include "stlexport.h"
#include <qfiledialog.h>
#include "application/ApplicationManager.h"
#include "glmodel.h"
#include "STL/stlio.hpp"
#include "../common/rapidjson/writer.h"
#include "../common/rapidjson/stringbuffer.h"
#include "../common/rapidjson/PrettyWriter.h"
#include "../common/rapidjson/ostreamwrapper.h"
#include "zip/zip_file.hpp"
using namespace Hix::Engine3D;
using namespace Hix::Features;
using namespace tyti::stl;
const constexpr auto STL_TEMP_PATH = "hix_temp_stl";

#if defined(_DEBUG)
//#define SAVE_ASCII
#endif
Hix::Features::STLExportMode::STLExportMode()
{
	auto models = Hix::Application::ApplicationManager::getInstance().partManager().allModels();
	QString fileName;
	if (models.size() == 1)
	{
		fileName = QFileDialog::getSaveFileName(nullptr, "Save to STL file", "", "3D Model file (*.stl)");

	}
	else if (models.size() > 1)
	{
		fileName = QFileDialog::getSaveFileName(nullptr, "Save to STL Zip file", "", "3D Model Collection(*.zip)");

	}
    if (fileName.isEmpty())
    {
        return;
    }
    auto se = new STLExport(models, fileName);
    Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::unique_ptr<STLExport>(se));
}

Hix::Features::STLExportMode::~STLExportMode()
{
}


STLExport::STLExport(const std::unordered_set<GLModel*>& selected, QString path): _models(selected), _path(path.toStdString())
{
}

Hix::Features::STLExport::~STLExport()
{
}

void Hix::Features::STLExport::run()
{

	_tmpPath = std::filesystem::temp_directory_path().append(STL_TEMP_PATH);
	std::filesystem::remove(_tmpPath);
	std::filesystem::create_directory(_tmpPath);

	exportModels();
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

#ifdef SAVE_ASCII
		exportSTLAscii(children, _currIdx);
#else
		exportSTLBin(children, _currIdx);
#endif
		++_currIdx;
	}
	//zip files if there are multiple models in the scene
	if (_modelsMap.size() > 1)
	{
		//write json
		auto jsonPath = _tmpPath;
		jsonPath /= "export.json";
		rapidjson::Document doc;
		doc.SetObject();

		//compress files
		miniz_cpp::zip_file file;
		for (size_t i = 0; i != _currIdx; ++i)
		{
			rapidjson::Value n(std::to_string(i).c_str(), doc.GetAllocator());
			doc.AddMember(n, _modelsMap[i]->modelName().toStdString(), doc.GetAllocator());

			//add to zip archive
			auto modelPath = _tmpPath;
			modelPath /= std::to_string(i);
			file.write(modelPath.string());
		}
		std::ofstream of(jsonPath, std::ios_base::trunc);
		rapidjson::OStreamWrapper osw{ of };
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer{ osw };
		doc.Accept(writer);

		file.save(_path.string());

	}
	else
	{
		auto tmpFile = _tmpPath;
		_tmpPath /= std::to_string(0);
		std::filesystem::copy_file(_tmpPath, _path);
	}

}

tyti::stl::vec3 toExportVec(const QVector3D& vec)
{
	return { vec.x(), vec.y(), vec.z() };
}

void Hix::Features::STLExport::exportSTLBin(const std::unordered_set<const GLModel*>& childs, size_t idx)
{
	auto path = _tmpPath;
	path /= std::to_string(idx);
	std::filesystem::create_directory(path);
	std::ofstream ofs(path, std::ios_base::trunc);
	for (auto& c : childs)
	{
		auto mesh = c->getMesh();
		if (mesh)
		{
			auto& faces = mesh->getFaces();
			tyti::stl::basic_solid<float> exportSolid;
			exportSolid.normals.reserve(faces.size());
			exportSolid.vertices.reserve(faces.size() * 3);

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
			tyti::stl::write_binary(ofs, exportSolid);
			ofs.close();
		}
	}
	return;
}

void STLExport::exportSTLAscii(const std::unordered_set<const GLModel*>& childs, size_t idx)
{
	auto path = _tmpPath;
	path /= std::to_string(idx);
	std::filesystem::create_directory(path);
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
