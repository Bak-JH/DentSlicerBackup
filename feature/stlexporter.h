#ifndef STLEXPORTER_H
#define STLEXPORTER_H
#include <fstream>
#include "DentEngine/src/mesh.h"

class STLexporter
{
public:
    STLexporter();
    //GLModel* mergeModels(vector<GLModel*> models);
    Mesh* mergeSelectedModels();
    void exportSTL(QString outfilename);
    void writeFace(ofstream& outfile, const Mesh* mesh, MeshFace mf);
    void writeHeader(ofstream& outfile);
    void writeFooter(ofstream& outfile);

};

#endif // STLEXPORTER_H
