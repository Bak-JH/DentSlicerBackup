#ifndef FILEOPENER_H
#define FILEOPENER_H
#include <QVector3D>
#include "mesh.h"

class FileOpener
{
public:
    FileOpener();
};

void* fgets_(char* ptr, size_t len, FILE* f);
bool loadMeshSTL_ascii(Mesh* mesh, const char* filename);
bool loadMeshSTL_binary(Mesh* mesh, const char* filename);
bool loadMeshSTL(Mesh* mesh, const char* filename);


#endif // FILEOPENER_H
