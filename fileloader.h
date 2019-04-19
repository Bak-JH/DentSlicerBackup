#ifndef FILELOADER_H
#define FILELOADER_H
#include <QVector3D>
#include "DentEngine/src/mesh.h"

//TODO: Factory pattern
class FileLoader
{
public:
    static void* fgets_(char* ptr, size_t len, FILE* f);
    static bool loadMeshSTL_ascii(Mesh* mesh, const char* filename);
    static bool loadMeshSTL_binary(Mesh* mesh, const char* filename);
    static bool loadMeshSTL(Mesh* mesh, const char* filename);
    static bool loadMeshOBJ(Mesh* mesh, const char* filename);
};




#endif // FILELOADER_H
