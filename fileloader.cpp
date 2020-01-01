#include "fileloader.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <iostream>
#include "DentEngine/src/mesh.h"
#include <fstream>
#include <string>
#include <sstream>

using namespace Hix::Engine3D;
/* Custom fgets function to support Mac line-ends in Ascii STL files. OpenSCAD produces this when used on Mac */
void* FileLoader::fgets_(char* ptr, size_t len, std::fstream& f)
{
    while(len && f.read(ptr, 1))
    {
        if (*ptr == '\n' || *ptr == '\r')
        {
            *ptr = '\0';
            return ptr;
        }
        ptr++;
        len--;
    }
    return nullptr;
}

std::deque<int> readFace(std::string line)
{
	std::deque<int> tokens;

	std::string const delims{ " /" };
	size_t beg, pos = 0;

	line.erase(0, 1);

	while ((beg = line.find_first_not_of(delims, pos)) != std::string::npos)
	{
		pos = line.find_first_of(delims, beg + 1);
		tokens.push_back(std::stoi(line.substr(beg, pos - beg)));
	}

	


	return tokens;
}


//c++11 no longer supplies a strcasecmp, so define our own version.
inline int stringcasecompare(const char* a, const char* b)
{
    while(*a && *b)
    {
        if (tolower(*a) != tolower(*b))
            return tolower(*a) - tolower(*b);
        a++;
        b++;
    }
    return *a - *b;
}

bool FileLoader::loadMeshSTL_ascii(Mesh* mesh, std::filesystem::path filepath)
{
	std::fstream f(filepath);
    char buffer[1024];
    QVector3D vertex;
    int n = 0;
    QVector3D v0, v1, v2;
    float f0, f1, f2;

    f.seekg(0L, SEEK_END);
    long long file_size = f.tellg(); //The file size is the position of the cursor after seeking to the end.
	f.seekg(0, SEEK_SET); //Seek back to start.
    size_t face_count = (file_size - 14) / 87; //Subtract the size of the header and conclusion. Every face uses more than 87 bytes.


    while(fgets_(buffer, sizeof(buffer), f))
    {
        if (sscanf(buffer, " vertex %f %f %f", &f0, &f1, &f2) == 3)
        {
			QVector3D vertex(f0, f1, f2);
            n++;
            switch(n)
            {
            case 1:
                v0 = vertex;
                break;
            case 2:
                v1 = vertex;
                break;
            case 3:
                v2 = vertex;
                mesh->addFace(v0, v1, v2);
                n = 0;
                break;
            }
        }

    }
	f.close();

    return true;
}

bool FileLoader::loadMeshSTL_binary(Mesh* mesh, std::filesystem::path filepath){
	std::fstream f(filepath, std::ios_base::in |std::ios_base::binary);
	constexpr size_t HEADER_SIZE =  80;
	constexpr size_t TRI_CNT_SIZE = 4;
	constexpr size_t TRI_SIZE = 50;
	//read header
	f.seekg(HEADER_SIZE, std::ios_base::cur);
	//read tri cnt
	uint32_t triCnt = 0;
	f.read(reinterpret_cast<char*>(&triCnt), sizeof(triCnt));
    for (size_t i = 0; i < triCnt; ++i)
    {
		char buffer[TRI_SIZE];
        if (!f.read(buffer, TRI_SIZE))
        {
			f.close();
            return false;
        }

        float *v= ((float*)buffer)+3;

        QVector3D v0(v[0], v[1], v[2]);
        QVector3D v1(v[3], v[4], v[5]);
        QVector3D v2(v[6], v[7], v[8]);
        mesh->addFace(v0, v1, v2);
    }
	f.close();

    return true;
}

bool FileLoader::loadMeshSTL(Mesh* mesh, QUrl fileUrl)
{
	std::filesystem::path filePath(fileUrl.toLocalFile().toStdWString());
	auto tmp = fileUrl.toLocalFile().toStdString();
	std::fstream f(filePath);

	//FILE* f = fopen(tmp.c_str(), "r");
    if (!f.is_open())
    {
        return false;
    }

    //Skip any whitespace at the beginning of the file.
    unsigned long long num_whitespace = 0; //Number of whitespace characters.
    char* whitespace = new char;

    if (!f.read(whitespace, 1))
    {
		f.close();
		//fclose(f);
        return false;
    }

    while(isspace(whitespace[0]))
    {
        num_whitespace++;
		if (!f.read(whitespace, 1))
        {
			f.close();
			//fclose(f);
            return false;
        }
    }
	f.seekg(num_whitespace, SEEK_SET);

    char buffer[6];
	//qDebug() << "asdf" << fread(buffer, 5, 1, f);
	if (!f.read(buffer, 5))
    {
		f.close();
		//fclose(f);
        return false;
    }
	//fclose(f);
	f.close();

    buffer[5] = '\0';
    if (stringcasecompare(buffer, "solid") == 0)
    {
        bool load_success = loadMeshSTL_ascii(mesh, filePath);
        if (!load_success)
            return false;

        // This logic is used to handle the case where the file starts with
        // "solid" but is a binary file.
        if (mesh->getFaces().size() < 1)
        {
            //mesh->clear();
            return loadMeshSTL_binary(mesh, filePath);
        }
        return true;
    }
    return loadMeshSTL_binary(mesh, filePath);
}

bool FileLoader::loadMeshOBJ(Mesh* mesh, QUrl fileUrl){
    char c;
    int lines = 0;
	std::filesystem::path filePath(fileUrl.toLocalFile().toStdWString());
	std::fstream f(filePath);
	constexpr size_t HEADER_SIZE = 145;

    if(!f.is_open())
        return 0;

    while(f.get(c))
        if(c == '\n')
            lines++;

	f.close();

    if(c != '\n')
        lines++;

	std::fstream file(filePath);
    if( !file.is_open() )
        return false;

    std::vector<QVector3D> temp_vertices;
    temp_vertices.reserve(lines);

	file.seekg(HEADER_SIZE, std::ios_base::cur);
    while(file.tellg() != -1){
		std::stringstream lineHeader;

        // read the first word of the line
		std::string readLine, type;
		getline(file, readLine);
		lineHeader << readLine;

		lineHeader >> type;

		qDebug() << type.c_str();

		if (type == "v") {
			float v_x, v_y, v_z;
			lineHeader >> v_x >> v_y >> v_z;
			QVector3D vertex = QVector3D(v_x, v_y, v_z);
			temp_vertices.push_back(vertex);
        } 
		else if (type == "f"){
			char line[100]{ 0 };
            char save;
            std::deque<int> tokens = readFace(readLine);

			int vertexCnt = std::count(readLine.begin(), readLine.end(), ' ')-1;
			int offset = tokens.size() / vertexCnt;
			for (auto each : tokens)
				qDebug() << "tok: " << each;
			qDebug() << "space count: " << vertexCnt << "offset : " << offset;
            unsigned int vertexIndex[3];
            QVector3D v0, v1, v2;

            /*
             * (1) f v1/vt1 v2/vt2 v3/vt3 ...
             * (2) f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
             * (3) f v1//vn1 v2//vn2 v3//vn3 ...
             */
			vertexIndex[0] = tokens[0];
			v0 = temp_vertices[vertexIndex[0] - 1];

			vertexIndex[1] = tokens[offset];
			v1 = temp_vertices[vertexIndex[1] - 1];

			for (int i = (offset * 2); i < tokens.size() - 1; i += offset)
			{
				vertexIndex[2] = tokens[i];
				v2 = temp_vertices[vertexIndex[2] - 1];
				mesh->addFace(v0, v1, v2);
				v1 = v2;
			}

        } else{
        }

    }
	file.close();

    return true;
}
