//https://github.com/kartikkukreja/blog-codes/blob/master/src/Graham%20Scan%20Convex%20Hull.cpp
#include "convex_hull.h"

// IntPoint having the least y coordinate, used for sorting other points
// according to polar angle about this point
IntPoint pivot;

// returns -1 if a -> b -> c forms a counter-clockwise turn,
// +1 for a clockwise turn, 0 if they are collinear
int ccw(IntPoint a, IntPoint b, IntPoint c) {
    int area = (b.X - a.X) * (c.Y - a.Y) - (b.Y - a.Y) * (c.X - a.X);
    if (area > 0)
        return -1;
    else if (area < 0)
        return 1;
    return 0;
}

// returns square of Euclidean distance between two points
int sqrDist(IntPoint a, IntPoint b)  {
    int dx = a.X - b.X, dy = a.Y - b.Y;
    return dx * dx + dy * dy;
}

// used for sorting points according to polar order w.r.t the pivot
bool POLAR_ORDER(IntPoint a, IntPoint b)  {
    int order = ccw(pivot, a, b);
    if (order == 0)
        return sqrDist(pivot, a) < sqrDist(pivot, b);
    return (order == -1);
}

bool compareIntPoint(IntPoint a, IntPoint b){
    if(a.Y != b.Y) return a.Y < b.Y;
    return a.X < b.X;
}

Path grahamScan(Path path)    {

    IntPoint points[path.size()];
    for(int point_idx=0; point_idx<path.size(); point_idx++){//
        points[point_idx] = path[point_idx];
    }
    /**/qDebug() << "path to arr" << path.size() << sizeof(points)/sizeof(points[0]);
    int N = path.size();
    stack<IntPoint> hull;
    Path hull_path;

    if (N < 3) return path;

    // find the point having the least y coordinate (pivot),
    // ties are broken in favor of lower x coordinate
    int leastY = 0;
    for (int i = 1; i < N; i++)
        if (compareIntPoint(points[i], points[leastY]))
            leastY = i;
    /**/qDebug() << "got lesatY" << leastY;

    // swap the pivot with the first point
    IntPoint temp = points[0];
    points[0] = points[leastY];
    points[leastY] = temp;
    /**/qDebug() << "swap lesatY with [0]";

    // sort the remaining point according to polar order about the pivot
    pivot = points[0];
    sort(points + 1, points + N, POLAR_ORDER);
    /**/qDebug() << "POLAR_ORDER sort";

    hull.push(points[0]);
    hull.push(points[1]);

    for (int i = 2; i < N; i++) {
        /**/qDebug() << i;
        IntPoint top = hull.top();
        /**/qDebug() << "pivot";
        hull.pop();
        while (hull.size()>0 && ccw(hull.top(), top, points[i]) != -1)   {
            top = hull.top();
            hull.pop();
            /**/qDebug() << hull.size();
        }
        hull.push(top);
        hull.push(points[i]);
    }
    /**/qDebug() << "hull size" << hull.size();
    while (!hull.empty()){
        hull_path.push_back(hull.top());
        hull.pop();
    }
    /**/qDebug() << "got hull_path";
    return hull_path;
}

Path getConvexHull(Path* path){
    /**/qDebug() << "getConvexHull";
    /**/debugPath(*path);
    /**/qDebug() << "orient" << Orientation(*path);
    Path convex_hull = grahamScan(*path);
    /**/qDebug() << "got convex_hull";
    if(!Orientation(convex_hull)) ReversePath(convex_hull);
    /**/qDebug() << "convexHull orient" << Orientation(convex_hull);
    return convex_hull;
}
