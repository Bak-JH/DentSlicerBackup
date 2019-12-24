#include "stlexporter.h"
#include "qmlmanager.h"
using namespace Hix::Engine3D;
STLexporter::STLexporter()
{

}

void STLexporter::exportSTL(QString outfilename){

    qmlManager->setProgress(0);
    qmlManager->setProgressText("saving");

    auto& selectedModels = qmlManager->getSelectedModels();
    QFile outfile(outfilename);
    std::stringstream contentStream;
    outfile.open(QFile::WriteOnly);

    writeHeader(contentStream);
    qmlManager->setProgress(0.5);

	for (auto& sm : selectedModels)
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
    qmlManager->setProgress(1);
    QCoreApplication::processEvents();
    qmlManager->openResultPopUp("","File Saved","");
    return;
}

void STLexporter::writeFace(const Mesh* mesh, const Hix::Engine3D::FaceConstItr& mf, std::stringstream& content){

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

void STLexporter::writeHeader(std::stringstream& content){
    content << "solid legokangpalla\n";
}

void STLexporter::writeFooter(std::stringstream& content){
    content << "endsolid legokangpalla";
}
