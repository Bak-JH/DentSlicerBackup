#include "fileloader.h"
#include <QDebug>

FileLoader::FileLoader()
{

}

/* Custom fgets function to support Mac line-ends in Ascii STL files. OpenSCAD produces this when used on Mac */
void* fgets_(char* ptr, size_t len, FILE* f)
{
    while(len && fread(ptr, 1, 1, f) > 0)
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

//c++11 no longer supplies a strcasecmp, so define our own version.
static inline int stringcasecompare(const char* a, const char* b)
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

bool loadMeshSTL_ascii(Mesh* mesh, const char* filename)
{
    FILE* f = fopen(filename, "rt");
    char buffer[1024];
    QVector3D vertex;
    int n = 0;
    QVector3D v0, v1, v2;
    float f0, f1, f2;

    fseek(f, 0L, SEEK_END);
    long long file_size = ftell(f); //The file size is the position of the cursor after seeking to the end.
    rewind(f); //Seek back to start.
    size_t face_count = (file_size - 14) / 87; //Subtract the size of the header and conclusion. Every face uses more than 87 bytes.

    mesh->faces.reserve(face_count);
    mesh->vertices.reserve(face_count);
    while(fgets_(buffer, sizeof(buffer), f))
    {
        if (sscanf(buffer, " vertex %f %f %f", &f0, &f1, &f2) == 3)
        {
            vertex.setX(f0);
            vertex.setY(f1);
            vertex.setZ(f2);

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
    fclose(f);
    mesh->connectFaces();

    return true;
}

bool loadMeshSTL_binary(Mesh* mesh, const char* filename){
    FILE* f = fopen(filename, "rb");

    fseek(f, 0L, SEEK_END);
    long long file_size = ftell(f); //The file size is the position of the cursor after seeking to the end.
    rewind(f); //Seek back to start.
    size_t face_count = (file_size - 80 - sizeof(uint32_t)) / 50; //Subtract the size of the header. Every face uses exactly 50 bytes.

    char buffer[80];
    //Skip the header
    if (fread(buffer, 80, 1, f) != 1)
    {
        fclose(f);
        return false;
    }

    uint32_t reported_face_count;
    //Read the face count. We'll use it as a sort of redundancy code to check for file corruption.
    if (fread(&reported_face_count, sizeof(uint32_t), 1, f) != 1)
    {
        fclose(f);
        return false;
    }
    if (reported_face_count != face_count)
    {
        //qDebug() << "Face count reported by file (%s) is not equal to actual face count (%s). File could be corrupt!\n", std::to_string(reported_face_count).c_str(), std::to_string(face_count).c_str();
        //logWarning("Face count reported by file (%s) is not equal to actual face count (%s). File could be corrupt!\n", std::to_string(reported_face_count).c_str(), std::to_string(face_count).c_str());
    }

    //For each face read:
    //float(x,y,z) = normal, float(X,Y,Z)*3 = vertexes, uint16_t = flags
    // Every Face is 50 Bytes: Normal(3*float), Vertices(9*float), 2 Bytes Spacer
    mesh->faces.reserve(face_count);
    mesh->vertices.reserve(face_count);
    for (unsigned int i = 0; i < face_count; i++)
    {
        if (fread(buffer, 50, 1, f) != 1)
        {
            fclose(f);
            return false;
        }
        float *v= ((float*)buffer)+3;

        QVector3D v0 = QVector3D(v[0], v[1], v[2]);
        QVector3D v1 = QVector3D(v[3], v[4], v[5]);
        QVector3D v2 = QVector3D(v[6], v[7], v[8]);

        mesh->addFace(v0, v1, v2);
    }
    fclose(f);
    mesh->connectFaces();

    return true;
}

bool loadMeshSTL(Mesh* mesh, const char* filename)
{
    FILE* f = fopen(filename, "r");
    if (f == nullptr)
    {
        return false;
    }

    //Skip any whitespace at the beginning of the file.
    unsigned long long num_whitespace = 0; //Number of whitespace characters.
    unsigned char whitespace;
    if (fread(&whitespace, 1, 1, f) != 1)
    {
        fclose(f);
        return false;
    }
    while(isspace(whitespace))
    {
        num_whitespace++;
        if (fread(&whitespace, 1, 1, f) != 1)
        {
            fclose(f);
            return false;
        }
    }
    fseek(f, num_whitespace, SEEK_SET); //Seek to the place after all whitespace (we may have just read too far).

    char buffer[6];
    if (fread(buffer, 5, 1, f) != 1)
    {
        fclose(f);
        return false;
    }
    fclose(f);

    buffer[5] = '\0';
    if (stringcasecompare(buffer, "solid") == 0)
    {
        bool load_success = loadMeshSTL_ascii(mesh, filename);
        if (!load_success)
            return false;

        // This logic is used to handle the case where the file starts with
        // "solid" but is a binary file.
        if (mesh->faces.size() < 1)
        {
            //mesh->clear();
            return loadMeshSTL_binary(mesh, filename);
        }
        return true;
    }
    return loadMeshSTL_binary(mesh, filename);
}

