#pragma once
#include <sstream>
#include <QFile>
#include "DentEngine/src/mesh.h"
#include "interfaces/Feature.h"
#include "interfaces/Mode.h"




namespace Hix
{

	namespace Features
	{
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
			void exportSTL();
			void writeFace(const Hix::Engine3D::Mesh* mesh, const Hix::Engine3D::FaceConstItr& mf, std::stringstream& content);
			void writeHeader(std::stringstream& content);
			void writeFooter(std::stringstream& content);
			std::unordered_set<GLModel*> _models;
			QString _path;
		};
	}
}


