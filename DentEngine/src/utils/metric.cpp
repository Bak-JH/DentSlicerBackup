#include "metric.h"

/****************** Helper Functions For Overhang Detection Step *******************/

float pointDistance3D(IntPoint A, IntPoint B){
    return sqrt(pow(A.X-B.X, 2)+pow(A.Y-B.Y,2)+pow(A.Z-B.Z,2));
}

float pointDistance(IntPoint A, IntPoint B){
    return sqrt(pow(A.X-B.X, 2)+pow(A.Y-B.Y,2));
}

float pointDistance(QVector3D A, QVector3D B){
    return sqrt(pow(A.x()-B.x(), 2)+pow(A.y()-B.y(),2)+pow(A.z()-B.z(),2));
}
