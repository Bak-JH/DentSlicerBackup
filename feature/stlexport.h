#pragma once
#include <sstream>
#include <QFile>
#include <unordered_map>
#include "DentEngine/src/mesh.h"
#include "interfaces/Feature.h"
#include "interfaces/Mode.h"
#include <filesystem>
#include <iostream>
namespace Hix
{
	namespace Render
	{
		class SceneEntity;
	}

	namespace Features
	{
		constexpr auto STL_EXPORT_JSON = "export.json";
		class STLExportMode : public Hix::Features::InstantMode
		{
		public:
			STLExportMode();
			virtual ~STLExportMode();
		};

		class STLExport : public Hix::Tasking::Task
		{
		public:
			STLExport(const std::unordered_set<GLModel*>& selected, QString path);
			virtual ~STLExport();
			void run()override;
		private:
			void exportModels();
			void exportSTLBin(const std::unordered_set<const Hix::Render::SceneEntity*>& childs, size_t idx);
			void exportSTLAscii(const std::unordered_set<const Hix::Render::SceneEntity*>& childs, size_t idx);
			void writeFace(const Hix::Engine3D::Mesh* mesh, const Hix::Engine3D::FaceConstItr& mf, std::ostream& content);
			void writeHeader(std::ostream& content);
			void writeFooter(std::ostream& content);
			std::unordered_set<GLModel*> _models;
			size_t _currIdx = 0;
			std::unordered_map<size_t, GLModel*> _modelsMap;
			std::filesystem::path _tmpPath;
			std::filesystem::path _path;
			std::string getNthModelName(size_t);

		};
	}
}


