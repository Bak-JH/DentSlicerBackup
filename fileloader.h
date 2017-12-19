#ifndef FILELOADER_H
#define FILELOADER_H
#include <QVector3D>
#include "mesh.h"

class FileLoader
{
public:
    FileLoader();
};

void* fgets_(char* ptr, size_t len, FILE* f);
bool loadMeshSTL_ascii(Mesh* mesh, const char* filename);
bool loadMeshSTL_binary(Mesh* mesh, const char* filename);
bool loadMeshSTL(Mesh* mesh, const char* filename);


#endif // FILELOADER_H