#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include <iostream>
#include <stack>
#include <stdlib.h>
#include "src/polyclipping/clipper.hpp"
#include "src/mesh.h"
#include "src/slicer.h"
#include <QDebug>

using namespace std;
using namespace ClipperLib;
class Slices;

class Point    {
public:
    int x, y;
};

void convexHull(Slices& slices);
//stack<Point> convexH(Mesh* mesh, int N);

#endif // CONVEXHULL_H
