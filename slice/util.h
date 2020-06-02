#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include "vertex.h"
#include "../structs_and_classes.h"
std::vector<GLfloat> find_min_max(std::vector<Vertex>& vertices)
{
    GLfloat xmin = vertices[0].x;
    GLfloat xmax = vertices[0].x;
    GLfloat ymin = vertices[0].y;
    GLfloat ymax = vertices[0].y;
    GLfloat zmin = vertices[0].z;
    GLfloat zmax = vertices[0].z;
    for(int i=1; i<vertices.size(); ++i)
    {
        if(vertices[i].x < xmin)
        {
            xmin = vertices[i].x;
        }
        if(vertices[i].x > xmax) 
        {
            xmax = vertices[i].x;
        }
        
        if(vertices[i].y < ymin)
        {
            ymin = vertices[i].y;
        }
        if(vertices[i].y > ymax) 
        {
            ymax = vertices[i].y;
        }
        
        if(vertices[i].z < zmin)
        {
            zmin = vertices[i].z;
        }
        if(vertices[i].z > zmax) 
        {
            zmax = vertices[i].z;
        }
    }
    
    std::vector<GLfloat> a = {xmin, xmax, ymin, ymax, zmin, zmax};
    return a;
}


std::vector<GLfloat> find_min_max(const std::vector<triangle>& tri)
{
    GLfloat xmin = tri[0].point[0].x;
    GLfloat xmax = tri[0].point[0].x;
    GLfloat ymin = tri[0].point[0].y;
    GLfloat ymax = tri[0].point[0].y;
    GLfloat zmin = tri[0].point[0].z;
    GLfloat zmax = tri[0].point[0].z;
    for (int i = 1; i < tri.size(); ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (tri[i].point[j].x < xmin)
            {
                xmin = tri[i].point[j].x;
            }
            if (tri[i].point[j].x > xmax)
            {
                xmax = tri[i].point[j].x;
            }

            if (tri[i].point[j].y < ymin)
            {
                ymin = tri[i].point[j].y;
            }
            if (tri[i].point[j].y > ymax)
            {
                ymax = tri[i].point[j].y;
            }

            if (tri[i].point[j].z < zmin)
            {
                zmin = tri[i].point[j].z;
            }
            if (tri[i].point[j].z > zmax)
            {
                zmax = tri[i].point[j].z;
            }
        }

    }

    std::vector<GLfloat> a = { xmin, xmax, ymin, ymax, zmin, zmax };
    return a;
}
#endif
