#ifndef STLEXPORTER_H
#define STLEXPORTER_H
#include "DentEngine/src/mesh.h"
#include <fstream>

class STLexporter
{
public:
    STLexporter();
    Mesh* mergeSelectedModels();
    void exportSTL(QString outfilename);
    void writeFace(ofstream& outfile, Mesh* mesh, MeshFace mf);
    void writeHeader(ofstream& outfile);
    void writeFooter(ofstream& outfile);

};

#endif // STLEXPORTER_H
