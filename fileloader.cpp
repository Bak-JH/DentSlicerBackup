#include "fileloader.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <iostream>
#include "DentEngine/src/mesh.h"
#include <fstream> 

using namespace Hix::Engine3D;
/* Custom fgets function to support Mac line-ends in Ascii STL files. OpenSCAD produces this when used on Mac */
void* FileLoader::fgets_(char* ptr, size_t len, std::fstream& f)
{
    while(len && f.read(ptr, 1).gcount() > 0)
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

inline char* readFace(char* ptr, size_t len, std::fstream& f, char* save)
{
    while(len && f.read(ptr, 1).gcount() > 0)
    {
        char c = *ptr;
        if (c == '\n' || c == '\r')
        {
            *ptr = '\0';
            return nullptr;
        }
        else if (c == ' ' || c == '/' || c == '0' || c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '6' || c == '7' || c == '8' || c == '9')
        {
            ptr++;
            len--;
        }
        else {
            *save = *ptr;
            *ptr = '\0';
            return ptr;
        }
    }
    return nullptr;
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

    int face_cnt = 0;

    while(fgets_(buffer, sizeof(buffer), f))
    {

        if (face_cnt%1000==0)
            QCoreApplication::processEvents();
        face_cnt += 1;

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
	std::fstream f(filepath);
	f.seekg(0L, SEEK_END);

    long long file_size = f.tellg(); //The file size is the position of the cursor after seeking to the end.
	f.seekg(0, SEEK_SET); //Seek back to start.
    size_t face_count = (file_size - 80 - sizeof(uint32_t)) / 50; //Subtract the size of the header. Every face uses exactly 50 bytes.

    char buffer[80];
    //Skip the header
    if (f.read(buffer, 1).gcount() != 1)
    {
		f.close();
        return false;
    }

    char faceBuffer[80];
    //Read the face count. We'll use it as a sort of redundancy code to check for file corruption.
    if (f.read(faceBuffer, sizeof(uint32_t)).gcount() != 1)
    {
		f.close();
        return false;
    }

    //For each face read:
    //float(x,y,z) = normal, float(X,Y,Z)*3 = vertexes, uint16_t = flags
    // Every Face is 50 Bytes: Normal(3*float), Vertices(9*float), 2 Bytes Spacer

    for (unsigned int i = 0; i < face_count; i++)
    {
        if (i%1000==0)
            QCoreApplication::processEvents();
        if (f.read(buffer, 1).gcount() != 1)
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

	f.read(whitespace, 1);
    if (f.gcount() != 1)
    {
		f.close();
		//fclose(f);
        return false;
    }

    while(isspace(whitespace[0]))
    {
        num_whitespace++;
		f.read(whitespace, 1);
		if (f.gcount() != 1)
        {
			f.close();
			//fclose(f);
            return false;
        }
    }
	f.seekg(num_whitespace, SEEK_SET);

    char buffer[6];
	//qDebug() << "asdf" << fread(buffer, 5, 1, f);
	f.read(buffer, 1);
	if (f.gcount() != 1)
    {
		f.close();
		//fclose(f);
        return false;
    }
	//fclose(f);
	f.close();
	qDebug() << buffer;

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

bool FileLoader::loadMeshOBJ(Mesh* mesh, std::filesystem::path filepath){
    char c;
    int lines = 0;
	std::fstream f(filepath);

    if(!f.is_open())
        return 0;

    while((c = f.get()) != EOF)
        if(c == '\n')
            lines++;

	f.close();

    if(c != '\n')
        lines++;

	std::fstream file(filepath);
    if( !file.is_open() )
        return false;

    std::vector<QVector3D> temp_vertices;
    temp_vertices.reserve(lines);
    qDebug() << "test 2";
    while( 1 ){
        char lineHeader[128];
        // read the first word of the line
        file >> lineHeader;
        if (lineHeader == "")
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader

        if ( strcmp( lineHeader, "v" ) == 0 ){
            float v_x, v_y, v_z;
			file >> v_x >> v_y >> v_z;
            QVector3D vertex = QVector3D(v_x,v_y,v_z);
            temp_vertices.push_back(vertex);
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
			char line[100]{ 0 };
            char save;
            char *f = readFace(line, 100, file, &save);

            char *startptr = line + 1;
            char *endptr;
            unsigned int vertexIndex[3];
            QVector3D v0, v1, v2;

            /*
             * (1) f v1/vt1 v2/vt2 v3/vt3 ...
             * (2) f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
             * (3) f v1//vn1 v2//vn2 v3//vn3 ...
             */
            if ((endptr = strchr(startptr, '/'))) {
                *endptr = '\0';
                vertexIndex[0] = atoi(startptr);
                v0 = temp_vertices[vertexIndex[0]-1];

                startptr = strchr(endptr + 1, ' ') + 1;
                endptr = strchr(startptr, '/');
                *endptr = '\0';
                vertexIndex[1] = atoi(startptr);
                v1 = temp_vertices[vertexIndex[1]-1];

                while ( (startptr = strchr(endptr + 1, ' ')) && *(startptr += 1)) {
                    endptr = strchr(startptr, '/');
                    *endptr = '\0';
                    vertexIndex[2] = atoi(startptr);
                    v2 = temp_vertices[vertexIndex[2]-1];
                    mesh->addFace(v0, v1, v2);
                    v1 = v2;
                }
            }
            /* f v1 v2 v3 ... */
            else {
                endptr = strchr(startptr, ' ');
                *endptr = '\0';
                vertexIndex[0] = atoi(startptr);
                v0 = temp_vertices[vertexIndex[0]-1];

                startptr = endptr + 1;
                endptr = strchr(startptr, ' ');
                *endptr = '\0';
                vertexIndex[1] = atoi(startptr);
                v1 = temp_vertices[vertexIndex[1]-1];

                bool flag = true;
                while ( flag ) {
                    startptr = endptr + 1;
                    endptr = strchr(startptr, ' ');
                    if ( !endptr || !(*(endptr + 1)) ) flag = false;
                    else *endptr = '\0';
                    vertexIndex[2] = atoi(startptr);
                    v2 = temp_vertices[vertexIndex[2]-1];
                    mesh->addFace(v0, v1, v2);
                    v1 = v2;
                }
            }

            if (f) { /* if no line separation (ex. f v1 v2 v3f v'1 v'2 v'3s n1...) */
                *f = save;
                file.seekg(-1, SEEK_CUR);
            }

        } else{
            // Probably a comment, eat up the rest of the line
            char stupidBuffer[1000];
            file.get(stupidBuffer, 1000);
        }

    }
	file.close();

    return true;
}
