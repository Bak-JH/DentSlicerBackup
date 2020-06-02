#ifndef VERTEX_H
#define VERTEX_H

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "../structs_and_classes.h"



struct Vertex
{
    Vertex() {}
    Vertex(const vec3d& vec) : x(vec.x), y(vec.y), z(vec.z) {}
    Vertex(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
    Vertex(std::vector<GLfloat> v) : x(v[0]), y(v[1]), z(v[2]) {}

    GLfloat x, y, z;
};

using Normal = Vertex;

// TODO - implement Normal by inheriting from Vertex
//struct Normal
//{
//    Normal() {}
//    Normal(GLfloat x, GLfloat y, GLfloat z) : x(x), y(y), z(z) {}
//    Normal(std::vector<GLfloat> v) : x(v[0]), y(v[1]), z(v[2]) {}
//
//    GLfloat x, y, z;
//};

void getVertices(std::vector<Vertex>* vertices, std::vector<Normal>* normals, const std::vector<triangle>& tris)
{
    for (auto itr = tris.cbegin(); itr != tris.cend(); ++itr)
    {
        Vertex vert(itr->point[0]);
        vertices->emplace_back(itr->point[0]);
        vertices->emplace_back(itr->point[1]);
        vertices->emplace_back(itr->point[2]);

        Normal norm(itr->normal);
        normals->push_back(norm);
        normals->push_back(norm);
        normals->push_back(norm);
    }




    //// read ascii stl file and load all vertices into `verts`
    //std::ifstream infile(stl);
    //std::string line;
    //while (std::getline(infile, line))
    //{
    //    std::string trimmed_string = trim_copy(line);
    //    
    //    if(starts_with(trimmed_string, "vertex"))
    //    {
    //        std::stringstream stream(trimmed_string.substr(6, trimmed_string.size()));
    //        std::vector<GLfloat> values(
    //             (std::istream_iterator<GLfloat>(stream)),
    //             (std::istream_iterator<GLfloat>()));
    //             
    //        Vertex vert(values);
    //        vertices->push_back(vert);
    //    } else if (starts_with(trimmed_string, "facet"))
    //    {
    //        std::stringstream stream(trimmed_string.substr(12, trimmed_string.size()));
    //        std::vector<GLfloat> values(
    //             (std::istream_iterator<GLfloat>(stream)),
    //             (std::istream_iterator<GLfloat>()));
    //             
    //        Normal norm(values);
    //        normals->push_back(norm);
    //        normals->push_back(norm);
    //        normals->push_back(norm);
    //    }
    //}
}

#endif
