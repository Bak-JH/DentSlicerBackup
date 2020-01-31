#include "stlexport.h"
#include "qmlmanager.h"
using namespace Hix::Engine3D;
using namespace Hix::Features;


Hix::Features::STLExportMode::STLExportMode()
{
    auto fileUrl = QFileDialog::getSaveFileUrl(nullptr, "Save to STL file", QUrl(), "3D Model file (*.stl)");
    auto path = fileUrl.path();
    auto fileName = fileUrl.fileName();
    if (fileName.isEmpty())
    {
        return;
    }
    auto se = new STLExport(qmlManager->getSelectedModels(), path);
    qmlManager->taskManager().enqueTask(se);
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
    outfile.open(QFile::WriteOnly);

    writeHeader(contentStream);
    qmlManager->setProgress(0.5);

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
    outfile.write(QByteArray::fromStdString(contentStream.str()));
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
    content << "solid legokangpalla\n";
}

void STLExport::writeFooter(std::stringstream& content){
    content << "endsolid legokangpalla";
}
