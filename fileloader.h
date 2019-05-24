#ifndef FILELOADER_H
#define FILELOADER_H
#include <QVector3D>


namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
}

//TODO: Factory pattern
class FileLoader
{
public:
    static void* fgets_(char* ptr, size_t len, FILE* f);
    static bool loadMeshSTL_ascii(Hix::Engine3D::Mesh* mesh, const char* filename);
    static bool loadMeshSTL_binary(Hix::Engine3D::Mesh* mesh, const char* filename);
    static bool loadMeshSTL(Hix::Engine3D::Mesh* mesh, const char* filename);
    static bool loadMeshOBJ(Hix::Engine3D::Mesh* mesh, const char* filename);
};




#endif // FILELOADER_H
