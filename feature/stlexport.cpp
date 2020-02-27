#include "stlexport.h"
#include <qfiledialog.h>
#include "application/ApplicationManager.h"
#include "glmodel.h"
using namespace Hix::Engine3D;
using namespace Hix::Features;


Hix::Features::STLExportMode::STLExportMode()
{
	auto fileName = QFileDialog::getSaveFileName(nullptr, "Save to STL file", "", "3D Model file (*.stl)");
    if (fileName.isEmpty())
    {
        return;
    }
    auto se = new STLExport(Hix::Application::ApplicationManager::getInstance().partManager().allModels(), fileName);
    Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(se);
}

Hix::Features::STLExportMode::~STLExportMode()
{
}


STLExport::STLExport(const std::unordered_set<GLModel*>& selected, QString path): _models(selected), _path(path)
{

}

Hix::Features::STLExport::~STLExport()
{
}

void Hix::Features::STLExport::run()
{
    exportSTL();
}

void STLExport::exportSTL()
{
    QFile outfile(_path);
    std::stringstream contentStream;
    auto opened =  outfile.open(QFile::WriteOnly);
	auto err = outfile.errorString();
    writeHeader(contentStream);
    //Hix::Application::ApplicationManager::getInstance().setProgress(0.5);

	for (auto& sm : _models)
	{
		std::unordered_set<const GLModel*> children;
		sm->getChildrenModels(children);
		children.insert(sm);
		for (auto& c : children)
		{
			auto mesh = c->getMesh();
			if (mesh)
			{
				auto faceCend = mesh->getFaces().cend();
				for (auto mf = mesh->getFaces().cbegin(); mf != faceCend; ++mf)
				{
					writeFace(mesh, mf, contentStream);
				}
			}

		}
	}
    writeFooter(contentStream);
	auto byteArray = QByteArray::fromStdString(contentStream.str());
    outfile.write(byteArray);
    outfile.close();
    return;
}

void STLExport::writeFace(const Mesh* mesh, const Hix::Engine3D::FaceConstItr& mf, std::stringstream& content){

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

void STLExport::writeHeader(std::stringstream& content){
    content << "solid savetest\n";
}

void STLExport::writeFooter(std::stringstream& content){
    content << "endsolid savetest";
}
