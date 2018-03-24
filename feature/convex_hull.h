#ifndef CONVEX_HULL_H
#define CONVEX_HULL_H
#include <cstdio>
#include <stack>
#include <algorithm>
#include "./polyclipping/clipper.hpp"
#include "autoarrange.h"

using namespace std;
using namespace ClipperLib;

int ccw(IntPoint a, IntPoint b, IntPoint c);
int sqrDist(IntPoint a, IntPoint b);
bool POLAR_ORDER(IntPoint a, IntPoint b);
bool compareIntPoint(IntPoint a, IntPoint b);
Path grahamScan(Path path);
Path getConvexHull(Path* path);

#endif // CONVEX_HULL_H
