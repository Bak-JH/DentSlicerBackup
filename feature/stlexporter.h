#ifndef STLEXPORTER_H
#define STLEXPORTER_H
#include <fstream>
#include "DentEngine/src/mesh.h"

class STLexporter
{
public:
    STLexporter();
    //GLModel* mergeModels(std::vector<GLModel*> models);
    Mesh* mergeSelectedModels();
    void exportSTL(QString outfilename);
    void writeFace(std::ofstream& outfile, const Mesh* mesh, MeshFace mf);
    void writeHeader(std::ofstream& outfile);
    void writeFooter(std::ofstream& outfile);

};

#endif // STLEXPORTER_H
