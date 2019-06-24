#ifndef STLEXPORTER_H
#define STLEXPORTER_H
#include <fstream>
#include "DentEngine/src/mesh.h"

class STLexporter
{
public:
    STLexporter();
    //GLModel* mergeModels(std::vector<GLModel*> models);
    Hix::Engine3D::Mesh* mergeSelectedModels();
    void exportSTL(QString outfilename);
    void writeFace(std::ofstream& outfile, const Hix::Engine3D::Mesh* mesh, Hix::Engine3D::MeshFace mf);
    void writeHeader(std::ofstream& outfile);
    void writeFooter(std::ofstream& outfile);

};

#endif // STLEXPORTER_H
