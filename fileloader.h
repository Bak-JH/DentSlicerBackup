#ifndef FILELOADER_H
#define FILELOADER_H
#include <QVector3D>
#include  <filesystem>
#include <QFileDialog>

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
    static void* fgets_(char* ptr, size_t len, std::fstream& f);
    static bool loadMeshSTL_ascii(Hix::Engine3D::Mesh* mesh, std::filesystem::path filepath);
    static bool loadMeshSTL_binary(Hix::Engine3D::Mesh* mesh, std::filesystem::path filepath);
    static bool loadMeshSTL(Hix::Engine3D::Mesh* mesh, QUrl fileUrl);
    static bool loadMeshOBJ(Hix::Engine3D::Mesh* mesh, std::filesystem::path filepath);
};




#endif // FILELOADER_H
