#ifndef STLEXPORTER_H
#define STLEXPORTER_H
#include <sstream>
#include <QFile>
#include "DentEngine/src/mesh.h"

class STLexporter
{
public:
    STLexporter();
    //GLModel* mergeModels(std::vector<GLModel*> models);
    Hix::Engine3D::Mesh* mergeSelectedModels();
    void exportSTL(QString outfilename);
    void writeFace(const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::MeshFace mf, std::stringstream& content);
    void writeHeader(std::stringstream& content);
    void writeFooter(std::stringstream& content);

};

#endif // STLEXPORTER_H
