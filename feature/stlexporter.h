#ifndef STLEXPORTER_H
#define STLEXPORTER_H
#include "mesh.h"
#include <fstream>

class STLexporter
{
public:
    STLexporter();
    void exportSTL(Mesh* mesh, QString outfilename);
    void writeFace(ofstream& outfile, Mesh* mesh, MeshFace mf);
    void writeHeader(ofstream& outfile);
    void writeFooter(ofstream& outfile);

};

#endif // STLEXPORTER_H