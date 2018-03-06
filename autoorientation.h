#ifndef AUTOORIENTATION_H
#define AUTOORIENTATION_H
#include "mesh.h"
#include "QMatrix4x4"
#include <math.h>
#include <algorithm>
#include <map>
#include <string>
#include <QTime>
using namespace std;

class rotateResult{
public:
    float phi;
    QVector3D v;
//    float R[16];
    QMatrix4x4 R;
};
class Orient {
public:
    float val;
    QVector3D label;
};
class Liste {
public:
    QVector3D orientation;
    float bottomA;
    float overhangA;
    float lineL;
    bool isActive=false;
};

class autoorientation : public QObject
{
    Q_OBJECT
public:
    autoorientation();
    int progress;
    rotateResult* Tweak(Mesh* mesh, bool bi_algorithmic,int CA,bool *appropriately_rotated);
    float approachvertex(Mesh* mesh,float n[]);
public slots:
    void progressChanged(float value);
signals:
    void setProgress(QVariant value);
private:
    float target_function(float touching,float overhang,float line);
    float* lithograph(Mesh* mesh, float n[], float amin, int CA);
    float get_touching_line(Mesh* mesh,float a[],int i,float touching_height);
    Orient* area_cumulation(Mesh* mesh,float n[],bool bi_algorithmic);
    Orient* egde_plus_vertex(Mesh* mesh, int bsvest_n);
    float* calc_random_normal(Mesh* mesh,int i);
    Orient* remove_duplicates(Orient* o,int *orientCnt);
    rotateResult* euler(Liste bestside);
};
#endif // AUTOORIENTATION_H
