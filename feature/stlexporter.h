#ifndef STLEXPORTER_H
#define STLEXPORTER_H
#include <sstream>
#include <QFile>
#include "DentEngine/src/mesh.h"

class STLexporter
{
public:
    STLexporter();
    void exportSTL(QString outfilename);
    void writeFace(const Hix::Engine3D::Mesh* mesh, const Hix::Engine3D::FaceConstItr& mf, std::stringstream& content);
    void writeHeader(std::stringstream& content);
    void writeFooter(std::stringstream& content);

};

#endif // STLEXPORTER_H
