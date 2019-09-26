#include "modelcut.h"
#include <algorithm>
#include <queue>
#include <map>
#include <string.h>
#include <stdlib.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesh_vertex_base_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <vector>
#include <CGAL/Delaunay_mesher_2.h>
#include <math.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel  Kernel;
typedef Kernel::Point_2            Point_2;
typedef CGAL::Delaunay_mesh_vertex_base_2<Kernel>     Vb;
typedef CGAL::Delaunay_mesh_face_base_2<Kernel>      Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>   Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<Kernel, Tds>  CDT;
typedef CDT::Vertex_handle          Vertex_handle;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT>    Criteria;

using namespace Hix::Engine3D;

class BigInteger {

#define BigIntegermaxSize 70
#define BigIntegermaxLen 30

public :
    bool IsZero(char *A);
    bool BiggerThan(char *A, char *B);
    void SwapCharArr(char **A, char **B);
    void fill(int *a, char *A);
    int * add(char *A, char *B, int *buf);
    int * subtract(char *A, char *B, int *buf);
    int * multiply(char *A, char *B, int *buf);
    int * divide(char *A, char *B, int *buf);
    void doOper(char *a, char *b, char oper, char *result);
    long long int char2int(char *a);
    void int2char(long long int a, char *b);
};
bool BigInteger::IsZero(char *A) {
    for(int i=0; A[i]; i++) {
        if(A[i] != '0') return false;
    }
    return true;
}
bool BigInteger::BiggerThan(char *A, char *B) {
    int len1 = strlen(A), len2 = strlen(B);
    if(len1 > len2) return true;
    if(len1 < len2) return false;
    for(int i=0; A[i]; i++) {
        if(A[i] > B[i]) return true;
        if(A[i] < B[i]) return false;
    }
    return false;
}
void BigInteger::SwapCharArr(char **A, char **B) {
    char *temp = *A;
    *A = *B;
    *B = temp;
}
void BigInteger::fill(int *a, char *A) {
    int len = strlen(A);
    for(int i=BigIntegermaxSize-len+1, j=0; i<=BigIntegermaxSize; i++, j++)
        a[i] = A[j] - '0';
}
int* BigInteger::add(char *A, char *B, int *buf) {
    int len1 = strlen(A), len2 = strlen(B), i;
    int a[BigIntegermaxSize+1]={}, b[BigIntegermaxSize+1]={};
    for (i=0;i<=BigIntegermaxSize;i++) {a[i]=0; b[i]=0;}
    fill(a, A); fill(b, B);
    for(i=BigIntegermaxSize; i>=1; i--) {
        buf[i] += a[i] + b[i];
        if(buf[i] > 9) {
            buf[i] -= 10;
            buf[i-1] ++;
        }
    }
    for(i=0; i<BigIntegermaxSize && buf[i] == 0; i++);
    return buf + i;
}
int* BigInteger::subtract(char *A, char *B, int *buf) {
    if(BiggerThan(B, A))
        SwapCharArr(&A, &B);
    int len1 = strlen(A), len2 = strlen(B), i;
    int a[BigIntegermaxSize+1]={}, b[BigIntegermaxSize+1]={};
    for (i=0;i<=BigIntegermaxSize;i++) {a[i]=0; b[i]=0;}
    fill(a, A); fill(b, B);
    for(i=BigIntegermaxSize; i>=1; i--) {
        buf[i] += a[i] - b[i];
        if(buf[i] < 0) {
            buf[i] += 10;
            buf[i-1]--;
        }
    }

    for(i=0; i<BigIntegermaxSize && buf[i] == 0; i++);
    return buf + i;
}
int* BigInteger::multiply(char *A, char *B, int *buf) {
    int len1 = strlen(A), len2 = strlen(B), i, j;
    int a[BigIntegermaxSize+1]={}, b[BigIntegermaxSize+1]={};
    for (i=0;i<=BigIntegermaxSize;i++) {a[i]=0; b[i]=0;}
    fill(a, A); fill(b, B);
    int blank = 0;
    for(j=BigIntegermaxSize; j>=BigIntegermaxSize-BigIntegermaxLen; j--) {
        for(i=BigIntegermaxSize; i>=BigIntegermaxSize-BigIntegermaxLen; i--) {
            buf[i-blank] += a[i] * b[j];
            if(buf[i-blank] >= 10) {
                buf[i-blank-1] += buf[i-blank] / 10;
                buf[i-blank] %= 10;
            }
        }
        blank++;
    }

    for(i=0; i<BigIntegermaxSize && buf[i] == 0; i++);
    return buf + i;
}
int* BigInteger::divide(char *A, char *B, int *buf) {
    int len1 = strlen(A), len2 = strlen(B), i, j;
    int a[BigIntegermaxSize+1]={}, b[BigIntegermaxSize+1]={};
    for (i=0;i<=BigIntegermaxSize;i++) {a[i]=0; b[i]=0;}
    fill(a, A); fill(b, B);
    for(j=BigIntegermaxLen; j>=0; j--) {
        int k;
        for(k=0; k<=9; k++) {
            int flag = 0;
            for(i=0; i<=BigIntegermaxSize-j; i++) {
                if(a[i] < b[i+j]) {
                    flag=1; break;
                }
                if(a[i] > b[i+j]) break;
            }
            if(flag == 1) break;

            buf[BigIntegermaxSize-j]++;
            for(i=BigIntegermaxSize-j; i>=1; i--) {
                a[i] -= b[i+j];
                if(a[i] < 0) {
                    a[i] += 10;
                    a[i-1] --;
                }
            }
        }
    }

    for(i=0; i<BigIntegermaxSize && buf[i] == 0; i++);
    return buf + i;
}
void BigInteger::doOper(char *a, char *b, char oper, char *result) {
    bool isMinus = false;
    int *buf = NULL, *res = NULL;
    buf = (int *)malloc(sizeof(int) * (BigIntegermaxSize+2));
    for (int i=0; i<=BigIntegermaxSize; i++)
        buf[i]=0;
    buf[BigIntegermaxSize+1] = -1;

    if(oper == '+') { // 0 일때 부호 안붙게
        if(a[0] == '-' && b[0] == '-') {
            if(!IsZero(a+1) || !IsZero(b+1)) isMinus=true;
            res = add(a+1, b+1, buf);
        } else if(a[0] == '-') {
            if(BiggerThan(a+1, b)) isMinus=true;
            res = subtract(b, a+1, buf);
        } else if(b[0] == '-') {
            if(BiggerThan(b+1, a)) isMinus=true;
            res = subtract(a, b+1, buf);
        } else {
            res = add(a, b, buf);
        }
    }
    else if(oper == '-') { // 0 일때 부호 안붙게
        if(a[0] == '-' && b[0] == '-') {
            if(BiggerThan(a+1, b+1)) isMinus=true;
            res = subtract(a+1, b+1, buf);
        } else if(a[0] == '-') {
            if(!IsZero(a+1) || !IsZero(b)) isMinus=true;
            res = add(a+1, b, buf);
        } else if(b[0] == '-') {
            res = add(a, b+1, buf);
        } else {
            if(BiggerThan(b, a)) isMinus=true;
            res = subtract(a, b, buf);
        }
    }
    else if(oper == '*') { // 0 일때 부호 안붙게
        if(a[0] == '-' && b[0] == '-') {
            res = multiply(a+1, b+1, buf);
        } else if(a[0] == '-') {
            if(!IsZero(a+1) && !IsZero(b)) isMinus=true;
            res = multiply(a+1, b, buf);
        } else if(b[0] == '-') {
            if(!IsZero(a) && !IsZero(b+1)) isMinus=true;
            res = multiply(a, b+1, buf);
        } else {
            res = multiply(a, b, buf);
        }
    }
    else { // 0일때 부호 안붙게
        if(a[0] == '-' && b[0] == '-') {
            res = divide(a+1, b+1, buf);
        } else if(a[0] == '-') {
            if(!BiggerThan(b, a+1)) isMinus=true;
            res = divide(a+1, b, buf);
        } else if(b[0] == '-') {
            if(!BiggerThan(b+1, a)) isMinus=true;
            res = divide(a, b+1, buf);
        } else {
            res = divide(a, b, buf);
        }
    }

    int resultCnt=0;
    if (isMinus) result[resultCnt++] = '-';
    for(int i=0;res[i] >= 0; i++, resultCnt++) {
        result[resultCnt] = res[i] + '0';
    }
    result[resultCnt] = '\0';
    free(buf);
}
long long int BigInteger::char2int(char *a){
    int st = 0;
    long long int result = 0;
    bool isMinus = (a[0]=='-');
    if (isMinus) st++;
    for(int i=st; a[i] != '\0'; i++) {
        result *= 10;
        result += a[i]-'0';
    }
    if (isMinus) result = -result;
    return result;
}
void BigInteger::int2char(long long int a, char *b){
    int i = 0;
    long long int ii=1;
    bool isMinus = (a<0);
    if (isMinus) {a = -a; b[i++] = '-';}
    while(a/ii>0) ii*=10;
    while(ii!=1) {
        ii/=10;
        b[i++] = (a/ii) + '0';
        a %= ii;
    }
    b[i] = '\0';
}

modelcut::modelcut(){
    numPoints = 0;
}




// interpolate between two random closed same oriented contours
void modelcut::interpolate(Mesh* mesh, Path3D contour1, Path3D contour2){
    Path3D bigger = contour2;
    Path3D smaller = contour1;
    if (contour1.size() > contour2.size()){
        bigger = contour1;
        smaller = contour2;
    }

    int prev_min_distance_mv_idx = -1;
    int min_distance_mv_idx;
    float min_distance = 99999;

    for (int ct1_idx =0; ct1_idx<bigger.size(); ct1_idx++){
        MeshVertex& bigger_mv = bigger[ct1_idx];
        min_distance_mv_idx = 0;
        min_distance = bigger_mv.position.distanceToPoint(smaller[min_distance_mv_idx].position);
        for (int ct2_idx=0; ct2_idx<smaller.size();ct2_idx++){
            float cur_distance = bigger_mv.position.distanceToPoint(smaller[ct2_idx].position);
            if (cur_distance < min_distance){
                min_distance_mv_idx = ct2_idx;
                min_distance = cur_distance;
            }
        }

        // add face to mesh
        if (prev_min_distance_mv_idx == min_distance_mv_idx){
            // add Face from bigger to smaller (smaller 1 bigger 2)
            mesh->addFace(bigger[ct1_idx].position, bigger[(ct1_idx+1)%bigger.size()].position, smaller[min_distance_mv_idx].position);
        } else {
            // add face from smaller to bigger (bigger 1 smaller 2)
            mesh->addFace(smaller[prev_min_distance_mv_idx].position,  bigger[ct1_idx].position, smaller[min_distance_mv_idx].position);
        }
        prev_min_distance_mv_idx = min_distance_mv_idx;
    }
}

typedef struct Custom3DPoint{
     long long int x, y, z;
     Custom3DPoint() {}
     Custom3DPoint(long long int x, long long int y, long long int z) : x(x), y(y), z(z) {}
     Custom3DPoint operator+(const Custom3DPoint& a) const
     {
         return Custom3DPoint(a.x+x, a.y+y, a.z+z);
     }
     Custom3DPoint operator-(const Custom3DPoint& a) const
     {
         return Custom3DPoint(a.x-x, a.y-y, a.z-z);
     }

     bool operator==(const Custom3DPoint& a) const
     {
         return (x == a.x && y == a.y && z == a.z);
     }
     bool operator!=(const Custom3DPoint& a) const
     {
         return (x != a.x || y != a.y || z != a.z);
     }
     bool operator<(const Custom3DPoint& a) const
     {
         if (x == a.x) {
             if (y == a.y)
                 return z < a.z;
             return y < a.y;
         }
         return x < a.x;
     }
     Custom3DPoint operator / (int c)     const { return Custom3DPoint(x/c, y/c, z/c); }
}Custom3DPoint;

Custom3DPoint custom3DCross(Custom3DPoint v1, Custom3DPoint v2)
{
    return Custom3DPoint(v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x);
    }

bool isNegativeCustom3DDot(Custom3DPoint v1, Custom3DPoint v2)
{
    BigInteger bi;

    char x1[BigIntegermaxLen], y1[BigIntegermaxLen], z1[BigIntegermaxLen], x2[BigIntegermaxLen], y2[BigIntegermaxLen], z2[BigIntegermaxLen];
    char tmp1[BigIntegermaxLen], tmp2[BigIntegermaxLen], tmp3[BigIntegermaxLen], tmp4[BigIntegermaxLen], dot[BigIntegermaxLen];

    bi.int2char(v1.x, x1);
    bi.int2char(v1.y, y1);
    bi.int2char(v1.z, z1);

    bi.int2char(v2.x, x2);
    bi.int2char(v2.y, y2);
    bi.int2char(v2.z, z2);

    bi.doOper(x1, x2, '*', tmp1);
    bi.doOper(y1, y2, '*', tmp2);
    bi.doOper(z1, z2, '*', tmp3);
    bi.doOper(tmp1, tmp2, '+', tmp4);
    bi.doOper(tmp4, tmp3, '+', dot);

    return dot[0] == '-';
    }

/*
 * 0 inner
 * 1 outter
 * 2 boundary
 *
 * control padding value
 */
int relationPointWithCuttingPolygon(Custom3DPoint p, std::vector<Custom3DPoint> polygon, long long int padding_v = 0) {
    unsigned int i, j, numPoints = polygon.size();

    int c = 1;
    long long int x = p.x, y = p.y;
    for (i = 0, j = numPoints-1; i < numPoints; j = i++) {
        if (abs(polygon[i].x - x) <= padding_v && abs(polygon[j].x - x) <= padding_v) {
            if (min(polygon[i].y, polygon[j].y) - padding_v <= y && y <= max(polygon[i].y, polygon[j].y) + padding_v)
                return 2;
        }

        if ((polygon[i].x > x) != (polygon[j].x > x)) {
            long long int padded_ymin = min( (polygon[j].x - polygon[i].x) * (y + padding_v), (polygon[j].x - polygon[i].x) * (y - padding_v) );
            long long int padded_ymax = min( (polygon[j].x - polygon[i].x) * (y + padding_v), (polygon[j].x - polygon[i].x) * (y - padding_v) );
            if (padded_ymin <= (polygon[j].y - polygon[i].y) * x + (polygon[j].x * polygon[i].y - polygon[i].x * polygon[j].y)  && (polygon[j].y - polygon[i].y) * x + (polygon[j].x * polygon[i].y - polygon[i].x * polygon[j].y) <= padded_ymax)
                return 2;

            if ((polygon[j].x - polygon[i].x) > 0) {
                if ((polygon[j].y - polygon[i].y) * x + (polygon[j].x * polygon[i].y - polygon[i].x * polygon[j].y) < (polygon[j].x - polygon[i].x) * y)
                    c = !c;
            } else {
                if ((polygon[j].y - polygon[i].y) * x + (polygon[j].x * polygon[i].y - polygon[i].x * polygon[j].y) > (polygon[j].x - polygon[i].x) * y)
                    c = !c;
            }
        }
    }
    return c;
}
int relationPoint2DPolygon(float x, float y, std::vector<Point_2> polygon) {
    unsigned int i, j, numPoints = polygon.size();
    float maxx, minx, maxy, miny, intersect_y;
    float padding_v = 0.05;
    bool c = false;

    if (numPoints < 3) return 1;

    for(i = 0, j = numPoints - 1; i < numPoints; j = i++) {
        maxx = max(polygon[i].x(), polygon[j].x()); minx = min(polygon[i].x(), polygon[j].x());
        maxy = max(polygon[i].y(), polygon[j].y()); miny = min(polygon[i].y(), polygon[j].y());

        if (!(minx < x && x <= maxx)){
            if (minx == maxx && miny <= y && y <= maxy)
                return 2;
            continue;
            }

        intersect_y = (polygon[j].y() - polygon[i].y()) * x;
        intersect_y += (polygon[j].x() * polygon[i].y() - polygon[i].x() * polygon[j].y());
        intersect_y /= (polygon[j].x() - polygon[i].x());

        if (abs(y-intersect_y) <= padding_v)
            return 2;

        if (y <= intersect_y)
            c = !c;
            }

    if (c)
        return 0;
    return 1;
        }

Custom3DPoint getInter(Custom3DPoint pp1, Custom3DPoint pp2, Custom3DPoint cp1, Custom3DPoint cp2) {
    Custom3DPoint p1, p2;
    if (pp1.x == pp2.x){
        if (pp1.y < pp2.y) {
            p1 = pp1; p2 = pp2;
        } else {
            p1 = pp2; p2 = pp1;
    }
    } else if (pp1.x < pp2.x) {
        p1 = pp1; p2 = pp2;
    } else {
        p1 = pp2; p2 = pp1;
    }

    Custom3DPoint inter;
    BigInteger bi;

    char a1[BigIntegermaxLen], b1[BigIntegermaxLen], c1[BigIntegermaxLen], a2[BigIntegermaxLen], b2[BigIntegermaxLen], c2[BigIntegermaxLen], determinant[BigIntegermaxLen];
    char tmp1[BigIntegermaxLen], tmp2[BigIntegermaxLen], tmp3[BigIntegermaxLen], interX[BigIntegermaxLen], interY[BigIntegermaxLen];

    bi.int2char(p2.y-p1.y, a1);
    bi.int2char(p1.x-p2.x, b1);
    bi.int2char((p2.y-p1.y)*p1.x + (p1.x-p2.x)*p1.y, c1);

    bi.int2char(cp2.y-cp1.y, a2);
    bi.int2char(cp1.x-cp2.x, b2);
    bi.int2char((cp2.y-cp1.y)*cp1.x + (cp1.x-cp2.x)*cp1.y, c2);

    bi.int2char((p2.y-p1.y)*(cp1.x-cp2.x) - (cp2.y-cp1.y)*(p1.x-p2.x), determinant);

    bi.doOper(b2, c1, '*', tmp1);
    bi.doOper(b1, c2, '*', tmp2);
    bi.doOper(tmp1, tmp2, '-', tmp3);
    bi.doOper(tmp3, determinant, '/', interX);
    inter.x = bi.char2int(interX);

    bi.doOper(a1, c2, '*', tmp1);
    bi.doOper(a2, c1, '*', tmp2);
    bi.doOper(tmp1, tmp2, '-', tmp3);
    bi.doOper(tmp3, determinant, '/', interY);
    inter.y = bi.char2int(interY);

    double inter_d = sqrt((inter.y-p1.y)*(inter.y-p1.y) + (inter.x-p1.x)*(inter.x-p1.x));
    double d = sqrt(((p2.y-p1.y)*(p2.y-p1.y) + (p1.x-p2.x)*(p1.x-p2.x)));

    inter.z = p1.z + (p2.z - p1.z) * inter_d / d;
    return inter;
}


std::vector<QVector3D> sortByCorrectOrder(QVector3D p1, QVector3D p2, QVector3D p3, QVector3D mv1, QVector3D mv2, QVector3D mv3) {
    std::vector<QVector3D> result;

    float dotProductV = QVector3D::dotProduct(QVector3D::normal(p1, p2, p3), QVector3D::normal(mv1, mv2, mv3));

    if (dotProductV >= 0.0){
        result.push_back(p1);
        result.push_back(p2);
        result.push_back(p3);
            } else {
        result.push_back(p3);
        result.push_back(p2);
        result.push_back(p1);
            }
    return result;
        }

std::vector<Custom3DPoint> sortByCorrectOrderCustom3D(Custom3DPoint p1, Custom3DPoint p2, Custom3DPoint p3, Custom3DPoint mv1, Custom3DPoint mv2, Custom3DPoint mv3) {
    std::vector<Custom3DPoint> result;

    if (!isNegativeCustom3DDot(custom3DCross(p2 - p1, p3- p1), custom3DCross(mv2 - mv1, mv3- mv1))){
        result.push_back(p1);
        result.push_back(p2);
        result.push_back(p3);
    } else {
        result.push_back(p3);
        result.push_back(p2);
        result.push_back(p1);
    }
    return result;
}

std::vector<Custom3DPoint> convert2vector(Custom3DPoint p1, Custom3DPoint p2, Custom3DPoint p3) {
    std::vector<Custom3DPoint> result;
    result.push_back(p1);
    result.push_back(p2);
    result.push_back(p3);
    return result;
}

Custom3DPoint getTargetZPoint(long long int targetZ, Custom3DPoint mv1, Custom3DPoint mv2) {
    if (mv1.z > mv2.z) {
        Custom3DPoint temp = mv1; mv1 = mv2; mv2 = temp;
    }
    // should be mv1.z() < targetZ < mv2.z()
    long long int targetX, targetY;
    if (targetZ - mv1.z >= mv2.z - targetZ) {
        targetX = mv1.x + (mv2.x - mv1.x) * (targetZ - mv1.z) / (mv2.z - mv1.z);
        targetY = mv1.y + (mv2.y - mv1.y) * (targetZ - mv1.z) / (mv2.z - mv1.z);
    } else {
        targetX = mv2.x + (mv1.x - mv2.x) * (mv2.z - targetZ) / (mv2.z - mv1.z);
        targetY = mv2.y + (mv1.y - mv2.y) * (mv2.z - targetZ) / (mv2.z - mv1.z);
    }
    return Custom3DPoint(targetX, targetY, targetZ);
}

Point_2 findHolePoint(std::vector<Point_2> polygon, std::vector<Point_2> parentPolygon) {
    // I assume that any two polygons do not overlap.
    int polygonN = polygon.size();
    int polygonNstep = max(1, polygonN/100);
    int polygonPN = parentPolygon.size();
    int polygonPNstep = max(1, polygonPN/100);
    int candiX, candiY;

    for (int i = 0; i < polygonN; i+=polygonNstep){
        for (int j = 0; j < polygonPN; j+=polygonPNstep) {
            candiX = (polygon[i].x() + parentPolygon[j].x())/2; candiY = (polygon[i].y() + parentPolygon[j].y())/2;

            if (relationPoint2DPolygon(candiX, candiY, parentPolygon) == 0 && relationPoint2DPolygon(candiX, candiY, polygon) == 1)
                return Point_2(candiX, candiY);
                    }
                }
    return Point_2(polygon[0].x()+0.00001, polygon[0].y());
}

int CCW(Custom3DPoint p1, Custom3DPoint p2, Custom3DPoint p3) {
    long long int ccw = p1.x*p2.y+p2.x*p3.y+p3.x*p1.y - p1.y*p2.x-p2.y*p3.x-p3.y*p1.x;
    if (ccw > 0)
        return 1;
    if (ccw < 0)
        return -1;
    return 0;
                    }

int point2CCW(Point_2 p1, Point_2 p2, Point_2 p3) {
    long long int ccw = p1.x()*p2.y()+p2.x()*p3.y()+p3.x()*p1.y() - p1.y()*p2.x()-p2.y()*p3.x()-p3.y()*p1.x();
    if (ccw > 0)
        return 1;
    if (ccw < 0)
        return -1;
    return 0;
                }

int point2isIntersectedLines(Point_2 A, Point_2 B, Point_2 C, Point_2 D) {
    return point2CCW(A,C,D) != point2CCW(B,C,D) && point2CCW(A,B,C) != point2CCW(A,B,D) && point2CCW(A,C,D) * point2CCW(B,C,D) * point2CCW(A,B,C) * point2CCW(A,B,D) != 0;
}

bool isIntersectedLines(Custom3DPoint A, Custom3DPoint B, Custom3DPoint C, Custom3DPoint D) {
    return CCW(A,C,D) != CCW(B,C,D) && CCW(A,B,C) != CCW(A,B,D) && CCW(A,C,D) * CCW(B,C,D) * CCW(A,B,C) * CCW(A,B,D) != 0;
                    }

struct findMaximumContour {
    std::vector<Point_2> contour;
    float area;
} maxContour;

void get_largest_contour(Custom3DPoint start, std::multimap<Custom3DPoint, Custom3DPoint> *boundaryEdges,
	std::set<Custom3DPoint> *checker, std::set<Custom3DPoint> *allChecker, std::vector<Custom3DPoint> *contourCandi) 
{
   //DFS
	std::deque<Custom3DPoint> q;
	q.push_back(start);
	allChecker->insert(start);
	checker->insert(start);
	contourCandi->push_back(start);
	std::map<Custom3DPoint, Custom3DPoint> traverseMap;


	while (!q.empty())
	{
		auto curr = q.back();
		q.pop_back();

		size_t count = 0;
		for (auto targetIter = boundaryEdges->lower_bound(curr); targetIter != boundaryEdges->upper_bound(curr); targetIter++)
		{
			Custom3DPoint nextPoint = targetIter->second;
			if (checker->find(nextPoint) == checker->end())
			{
				q.push_back(nextPoint);
				allChecker->insert(curr);
				checker->insert(curr);
				traverseMap[nextPoint] = curr;

				++count;
			}
		}
		if (count == 0)
		{
			//checking self intersecting
			bool isIntersect = false;
			//build contourCandi
			contourCandi->clear();
			contourCandi->push_back(curr);
			auto parent = traverseMap.find(curr);
			if (!traverseMap.empty())
			{
				//backtrace DFS to get the stitched contour -now closed.
				do
				{
					contourCandi->push_back(parent->second);
					parent = traverseMap.find(parent->second);


				} while (parent != traverseMap.end());
			}

			unsigned int t = contourCandi->size();
			for (unsigned int i = 1; i < t; i++) {
				if (isIntersectedLines((*contourCandi)[i - 1], (*contourCandi)[i], (*contourCandi)[0], (*contourCandi)[t - 1])) {
					isIntersect = true;
					break;
				}
			}

			if (!isIntersect) {
				float candiArea = 0.0;

				CGAL::Delaunay_triangulation_2<Kernel> dt;
				for (unsigned int i = 0; i < t; i++)
					dt.insert(Point_2((*contourCandi)[i].x / 1000000.0, (*contourCandi)[i].y / 1000000.0));

				for (CGAL::Delaunay_triangulation_2<Kernel>::Finite_faces_iterator fit = dt.finite_faces_begin(); fit != dt.finite_faces_end(); fit++) {
					float x1, y1, x2, y2, x3, y3;
					x1 = float(dt.triangle(fit)[0].x()); y1 = float(dt.triangle(fit)[0].y());
					x2 = float(dt.triangle(fit)[1].x()); y2 = float(dt.triangle(fit)[1].y());
					x3 = float(dt.triangle(fit)[2].x()); y3 = float(dt.triangle(fit)[2].y());
					candiArea += abs(x1 * y2 + x2 * y3 + x3 * y1 - x2 * y1 - x3 * y2 - x1 * y3) / 2.0;
				}
				if (candiArea > maxContour.area) {
					maxContour.contour.clear();
					for (unsigned int i = 0; i < t; i++) {
						maxContour.contour.push_back(Point_2((*contourCandi)[i].x, (*contourCandi)[i].y));
					}
					maxContour.area = candiArea;
				}
			}
		}
	}
}



void printCustomPoints(Custom3DPoint p1, Custom3DPoint p2, Custom3DPoint p3) {
    qDebug() << "(" << p1.x << ", " << p1.y << ", " << p1.z << ") " << "(" << p2.x << ", " << p2.y << ", " << p2.z << ") " << "(" << p3.x << ", " << p3.y << ", " << p3.z << ")";
}

void bisectModelByPlane(Mesh* leftMesh, Mesh* rightMesh, Mesh* mesh, float planeZ, int cutFillMode){
    std::vector<std::pair<Custom3DPoint, Custom3DPoint> > boundaryEdgeCandis;
    std::multimap<Custom3DPoint, Custom3DPoint> boundaryEdges;

    std::queue<std::vector<Custom3DPoint> > meshFaceQueue;
    Custom3DPoint mv1, mv2, mv3;

    double padding = 1000000.0;
    long long int targetZ = int(planeZ * padding);

    qDebug() << "mesh faces" << mesh->getFaces().size();

    qDebug() << "make meshFace Queue";
    std::vector<Custom3DPoint> meshVertices, parentMesh;
    for (const auto& mf: mesh->getFaces()){
        meshVertices.clear();
        for (int i = 0; i < 3; i++) {
			const auto& mvs = mf.meshVertices();
            QVector3D vertex = mvs[i]->position;
            Custom3DPoint customVertex;
            customVertex.x = int(vertex.x() * padding); customVertex.y = int(vertex.y() * padding); customVertex.z = int(vertex.z() * padding);
            meshVertices.push_back(customVertex);
        }
        meshFaceQueue.push(meshVertices);
    }

    while (!meshFaceQueue.empty()){
        meshVertices = meshFaceQueue.front(); meshFaceQueue.pop();

        mv1 = meshVertices[0]; mv2 = meshVertices[1]; mv3 = meshVertices[2];

        if (((mv1.z > targetZ) != (mv2.z > targetZ)) && targetZ != mv1.z && targetZ != mv2.z) {
            Custom3DPoint inter = getTargetZPoint(targetZ, mv1, mv2);

            if (mv1 != inter && mv2 != inter && mv3 != inter) {
                meshFaceQueue.push(convert2vector(mv1, inter, mv3));
                meshFaceQueue.push(convert2vector(inter, mv2, mv3));
                continue;
                }
                }
        if (((mv2.z > targetZ) != (mv3.z > targetZ)) && targetZ != mv2.z && targetZ != mv3.z) {
            Custom3DPoint inter = getTargetZPoint(targetZ, mv2, mv3);

            if (mv1 != inter && mv2 != inter && mv3 != inter) {
                meshFaceQueue.push(convert2vector(mv2, inter, mv1));
                meshFaceQueue.push(convert2vector(inter, mv3, mv1));
                continue;
            }
        }
        if (((mv3.z > targetZ) != (mv1.z > targetZ)) && targetZ != mv3.z && targetZ != mv1.z) {
            Custom3DPoint inter = getTargetZPoint(targetZ, mv3, mv1);

            if (mv1 != inter && mv2 != inter && mv3 != inter) {
                meshFaceQueue.push(convert2vector(mv3, inter, mv2));
                meshFaceQueue.push(convert2vector(inter, mv1, mv2));
                continue;
    }
        }

        // didn't cutted mesh
        int innerOutter = 0;

        if (mv1.z + mv2.z > targetZ * 2) innerOutter--;
        else innerOutter++;

        if (mv2.z + mv3.z > targetZ * 2) innerOutter--;
        else innerOutter++;

        if (mv3.z + mv1.z > targetZ * 2) innerOutter--;
        else innerOutter++;

        QVector3D addingMesh1 = QVector3D(mv1.x/padding, mv1.y/padding, mv1.z/padding);
        QVector3D addingMesh2 = QVector3D(mv2.x/padding, mv2.y/padding, mv2.z/padding);
        QVector3D addingMesh3 = QVector3D(mv3.x/padding, mv3.y/padding, mv3.z/padding);
        if (innerOutter > 0)
            rightMesh->addFace(addingMesh1,addingMesh2,addingMesh3);
        else {
            leftMesh->addFace(addingMesh1,addingMesh2,addingMesh3);

            /*
             * 1. z == targetZ
             *
             * if Mesh edge's vertice are both included on 1
             * => that edge is boundary edge
             */
            if (mv1.z == targetZ && mv2.z == targetZ) {
                boundaryEdgeCandis.push_back(std::make_pair(mv1, mv2));
    }
            if (mv2.z == targetZ && mv3.z == targetZ) {
                boundaryEdgeCandis.push_back(std::make_pair(mv2, mv3));
            }
            if (mv3.z == targetZ && mv1.z == targetZ) {
                boundaryEdgeCandis.push_back(std::make_pair(mv3, mv1));
            }
        }
    }

    qDebug() << "boundary edge candi: " << boundaryEdgeCandis.size();
    //check boundary edge candidate has self intersection
    while(true){
        int i, j, becCnt = boundaryEdgeCandis.size();
        for (i = 0; i < becCnt; i++){
            for (j = i+1; j < becCnt; j++){
                Custom3DPoint A = boundaryEdgeCandis[i].first, B = boundaryEdgeCandis[i].second, C = boundaryEdgeCandis[j].first, D = boundaryEdgeCandis[j].second;
                if (A==C || A==D || B==C || B==D) continue;
                if (CCW(A,C,D) != CCW(B,C,D) && CCW(A,B,C) != CCW(A,B,D)) {
                    Custom3DPoint inter = getInter(A, B, C, D);
                    inter.z = targetZ;
                    boundaryEdgeCandis[i] = boundaryEdgeCandis[becCnt-1];
                    boundaryEdgeCandis[j] = boundaryEdgeCandis[becCnt-2];
                    boundaryEdgeCandis.pop_back(); boundaryEdgeCandis.pop_back();
                    if (CCW(C,A,D) != 0)
                        boundaryEdgeCandis.push_back(std::make_pair(inter, A));
                    if (CCW(C,B,D) != 0)
                        boundaryEdgeCandis.push_back(std::make_pair(inter, B));
                    if (CCW(A,C,B) != 0)
                        boundaryEdgeCandis.push_back(std::make_pair(inter, C));
                    if (CCW(A,D,B) != 0)
                        boundaryEdgeCandis.push_back(std::make_pair(inter, D));
                    break;
                }
            }
            if (j < becCnt)
                break;
        }
        if (i>=becCnt) {
            for (i = 0; i < becCnt; i++){
                boundaryEdges.insert(std::make_pair(boundaryEdgeCandis[i].first, boundaryEdgeCandis[i].second));
                boundaryEdges.insert(std::make_pair(boundaryEdgeCandis[i].second, boundaryEdgeCandis[i].first));
            }
            boundaryEdgeCandis.clear();
            break;
        }
    }

    if (cutFillMode == 2) {
        // get contour
        qDebug() << "get contours";
        std::multimap<Custom3DPoint, Custom3DPoint>::iterator iter;
        std::set<Custom3DPoint> checker, allChecker;
        std::vector<std::vector<Point_2>> contours;
        std::vector<Custom3DPoint> contourCandi;
        Custom3DPoint prevPoint, currPoint;
        for (iter = boundaryEdges.begin(); iter != boundaryEdges.end(); iter++){
            if (allChecker.find(iter->first) == allChecker.end()){
                qDebug() << "find contour";
                maxContour.area = 0.0;
                get_largest_contour(iter->first, &boundaryEdges, &checker, &allChecker, &contourCandi);
                if (maxContour.area > 0.5) {
                    qDebug() << "contour size: " << maxContour.contour.size();
                    contours.push_back(maxContour.contour);
                }
            }
        }
        qDebug() << contours.size();

        int contourN = contours.size();

        // add polygon and constraints on CDT
        CDT cdt;
        for (unsigned int i = 0; i < contourN; i++) {
            std::vector<Vertex_handle> vhs;
            size_t numPoints = contours[i].size();

            for (unsigned int j = 0; j < numPoints; j++)
                vhs.push_back(cdt.insert(contours[i][j]));

            for (unsigned int j = 0; j < numPoints; j++)
                cdt.insert_constraint(vhs[j], vhs[(j+1)%numPoints]);
        }

        // I assumed that the maximum number of contours is 100.
        int directedInclusionRelationship[100][100];
        for (int contourI=0; contourI < contourN; contourI++) {
            for (int contourJ=0; contourJ < contourN; contourJ++) {
                directedInclusionRelationship[contourI][contourJ] = 0;
            }
        }
        // make directed adjacency matrix
        for (int contourI=0; contourI < contourN; contourI++) {
            for (int contourJ=0; contourJ < contourN; contourJ++) {
                if (contourI == contourJ) continue;

                int relation;
                for (int kkk=0; kkk<=contours[contourJ].size(); kkk++){
                    relation = relationPoint2DPolygon(contours[contourJ][kkk].x(), contours[contourJ][kkk].y(), contours[contourI]);
                    if (relation != 2){
                        break;
                    }
                }
                if (relation == 0) {
                    directedInclusionRelationship[contourI][contourJ] = 1;
                }
            }
        }

        // using Flody-Warshall get topology
        for (int contourI=0; contourI < contourN; contourI++) {
            for (int contourJ=0; contourJ < contourN; contourJ++) {
                if (contourI == contourJ) continue;
                for (int contourK=0; contourK < contourN; contourK++) {
                    if (contourI == contourK || contourJ == contourK) continue;
                    if (directedInclusionRelationship[contourI][contourK] > 0 && directedInclusionRelationship[contourK][contourJ] > 0){
                        directedInclusionRelationship[contourI][contourJ] = directedInclusionRelationship[contourI][contourK] + directedInclusionRelationship[contourK][contourJ];

        }
                }
            }
        }

        // find root nodes
        std::queue<std::pair<std::pair<int, bool>, int> > contourQ;
        for (int contourI=0; contourI < contourN; contourI++) {
            int contourJ;
            for (contourJ=0; contourJ < contourN; contourJ++) {
                if (directedInclusionRelationship[contourJ][contourI] != 0)
                    break;
            }
            if (contourJ >= contourN) {
                contourQ.push(std::make_pair(std::make_pair(contourI, true), -1));
        }
        }

        // bfs for get hole point
        std::list<Point_2> list_of_seeds;
        std::pair<std::pair<int, bool>, int> qFront;
        std::pair<int, bool> contourT;
        int contourParent;
        while(!contourQ.empty()) {
            bool itHasChild = false;
            qFront = contourQ.front(); contourQ.pop();
            contourT = qFront.first; contourParent = qFront.second;
            qDebug() << contours[contourT.first].size() << " " << contourT.second;
            if (contourT.second && contourParent != -1) {
                // find hole point
                Point_2 holePoint = findHolePoint(contours[contourT.first], contours[contourParent]);
                list_of_seeds.push_back(holePoint);
        }
            for (int contourI=0; contourI < contourN; contourI++) {
                if (directedInclusionRelationship[contourT.first][contourI] == 1) {
                    itHasChild = true;
                    contourQ.push(std::make_pair(std::make_pair(contourI, !contourT.second), contourT.first));
    }
            }
            if ((!itHasChild) && (!contourT.second)) {
                // find hole point
                std::vector<Point_2> onePoint;
                onePoint.push_back(contours[contourT.first][0]);
                Point_2 holePoint = findHolePoint(onePoint, contours[contourT.first]);
                list_of_seeds.push_back(holePoint);
            }
        }

        // triangular
        qDebug() << "Triangular";
        if (list_of_seeds.size() > 0)
            CGAL::refine_Delaunay_mesh_2(cdt, list_of_seeds.begin(), list_of_seeds.end(), Criteria());
        else
            CGAL::refine_Delaunay_mesh_2(cdt, Criteria());

        std::vector<QVector3D> orderedQV;
        QVector3D mmv1, mmv2, mmv3;
        // add face
        qDebug() << "Add face";
        for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); fit++) {
            if (fit->is_in_domain()) {
                mmv1 = QVector3D(float(cdt.triangle(fit)[0].x())/padding, float(cdt.triangle(fit)[0].y())/padding, targetZ/padding);
                mmv2 = QVector3D(float(cdt.triangle(fit)[1].x())/padding, float(cdt.triangle(fit)[1].y())/padding, targetZ/padding);
                mmv3 = QVector3D(float(cdt.triangle(fit)[2].x())/padding, float(cdt.triangle(fit)[2].y())/padding, targetZ/padding);

                orderedQV = sortByCorrectOrder(mmv1, mmv2, mmv3, QVector3D(0.0, 0.0, 0.0), QVector3D(1.0, 0.0, 0.0), QVector3D(1.0, 1.0, 0.0));
                rightMesh->addFace(orderedQV[0], orderedQV[1], orderedQV[2]);
                leftMesh->addFace(orderedQV[2], orderedQV[1], orderedQV[0]);
                    }
                }
            }
        }

Custom3DPoint findConnectedPointOrRegister(std::multimap<long long int, std::vector<Custom3DPoint> > *connectVertex, Custom3DPoint p) {
    long long int padding_v = 10;
    long long int vertexSum = p.x + p.y + p.z;
    std::multimap<long long int, std::vector<Custom3DPoint> >::iterator targetIter;
    for (long long int candidateSum = vertexSum-padding_v; candidateSum <= vertexSum+padding_v; candidateSum++) {
        for (targetIter = connectVertex->lower_bound(candidateSum); targetIter != connectVertex->upper_bound(candidateSum); targetIter++){
            std::vector<Custom3DPoint> candidates = targetIter->second;
            unsigned int n = candidates.size();
            for (unsigned int i = 0; i < n; i++) {
                if (abs(candidates[i].x - p.x) + abs(candidates[i].y - p.y) + abs(candidates[i].z - p.z) <= padding_v) {
                    return candidates[i];
    }
    }
        }
    }
    std::vector<Custom3DPoint> newVector;
    newVector.push_back(p);
    connectVertex->insert(make_pair(vertexSum, newVector));
    return p;
}

void cutAway(Mesh* leftMesh, Mesh* rightMesh, Mesh* mesh, std::vector<QVector3D> cuttingPoints, int cutFillMode){
    std::multimap<long long int, std::vector<Custom3DPoint> > connectVertex;
    std::queue<std::pair<std::vector<Custom3DPoint>, std::vector<Custom3DPoint> > > meshFaceQueue;
    std::vector<Custom3DPoint> customCuttingPts;
    std::multimap<std::pair<Custom3DPoint, int>, std::pair<Custom3DPoint, int> > boundaryEdges;
    Custom3DPoint mv1, mv2, mv3;
    // calculated intersected Point with cutting line
    std::set<std::pair<Custom3DPoint, int> > meshCuttedChecker;
    double padding = 1000000.0;
    size_t cuttingPointCnt = cuttingPoints.size();
    bool isCutted;
    int i, j, k;

    qDebug() << "cuttingPoint size";
    qDebug() << cuttingPointCnt;

    for (i = 0; i < cuttingPointCnt; i++){
        Custom3DPoint customVertex;
        customVertex.x = int(cuttingPoints[i].x() * padding); customVertex.y = int(cuttingPoints[i].y() * padding); customVertex.z = int(cuttingPoints[i].z() * padding);
        customCuttingPts.push_back(customVertex);
    }

    qDebug() << "make meshFace Queue";
    std::vector<Custom3DPoint> meshVertices, parentMesh;
    for (const auto& mf: mesh->getFaces()){
		auto mvs = mf.meshVertices();
        meshVertices.clear();
        for (i = 0; i < 3; i++) {
            QVector3D vertex = mvs[i]->position;
            Custom3DPoint customVertex;
            customVertex.x = int(vertex.x() * padding); customVertex.y = int(vertex.y() * padding); customVertex.z = int(vertex.z() * padding);
            meshVertices.push_back(customVertex);
        }
        meshFaceQueue.push(make_pair(meshVertices, meshVertices));
    }

    while (!meshFaceQueue.empty()){
        std::pair<std::vector<Custom3DPoint>, std::vector<Custom3DPoint> > qFront;
        qFront = meshFaceQueue.front(); meshFaceQueue.pop();
        meshVertices = qFront.first; parentMesh = qFront.second;
        isCutted = false;

        mv1 = meshVertices[0]; mv2 = meshVertices[1]; mv3 = meshVertices[2];

        // check if cutting point in mesh
        for (i = 0; i < cuttingPointCnt; i++){
            if (relationPointWithCuttingPolygon(customCuttingPts[i], meshVertices) == 0){
                // cutting Point In Mesh
                customCuttingPts[i].z = (mv1.z + mv2.z + mv3.z)/3;
                meshFaceQueue.push(make_pair(convert2vector(mv1, mv2, customCuttingPts[i]), parentMesh));
                meshFaceQueue.push(make_pair(convert2vector(mv2, mv3, customCuttingPts[i]), parentMesh));
                meshFaceQueue.push(make_pair(convert2vector(mv3, mv1, customCuttingPts[i]), parentMesh));
                break;
            }
        }
        // cutting point in mesh
        if (i < cuttingPointCnt)
                continue;

        // check if cutting point boundary on mesh
        for (i = 0; i < cuttingPointCnt; i++){
            Custom3DPoint double1, one1, one2;
            bool cuttingPointOnMesh = false;
            if ((customCuttingPts[i].x == mv1.x && customCuttingPts[i].y == mv1.y) || (customCuttingPts[i].x == mv2.x && customCuttingPts[i].y == mv2.y) || (customCuttingPts[i].x == mv3.x && customCuttingPts[i].y == mv3.y))
                continue;

            if (min(mv1.x, mv2.x) <= customCuttingPts[i].x && customCuttingPts[i].x <= max(mv1.x, mv2.x) && min(mv1.y, mv2.y) <= customCuttingPts[i].y && customCuttingPts[i].y <= max(mv1.y, mv2.y) && CCW(mv1, mv2, customCuttingPts[i]) == 0){
                double1 = mv3; one1 = mv1; one2 = mv2; cuttingPointOnMesh = true;
            } else if (min(mv2.x, mv3.x) <= customCuttingPts[i].x && customCuttingPts[i].x <= max(mv2.x, mv3.x) && min(mv2.y, mv3.y) <= customCuttingPts[i].y && customCuttingPts[i].y <= max(mv2.y, mv3.y) && CCW(mv2, mv3, customCuttingPts[i]) == 0){
                double1 = mv1; one1 = mv2; one2 = mv3; cuttingPointOnMesh = true;
            } else if (min(mv3.x, mv1.x) <= customCuttingPts[i].x && customCuttingPts[i].x <= max(mv3.x, mv1.x) && min(mv3.y, mv1.y) <= customCuttingPts[i].y && customCuttingPts[i].y <= max(mv3.y, mv1.y) && CCW(mv3, mv1, customCuttingPts[i]) == 0){
                double1 = mv2; one1 = mv3; one2 = mv1; cuttingPointOnMesh = true;
            }
            if (cuttingPointOnMesh) {
                customCuttingPts[i].z = (one1.z + one2.z)/2;
                meshFaceQueue.push(make_pair(convert2vector(double1, one1, customCuttingPts[i]), parentMesh));
                meshFaceQueue.push(make_pair(convert2vector(double1, one2, customCuttingPts[i]), parentMesh));
                break;
            }
        }
        // cutting point on mesh
        if (i < cuttingPointCnt)
            continue;

        for (i = 0, j = cuttingPointCnt - 1; i < cuttingPointCnt; j = i++){
            if (isIntersectedLines(mv1, mv2, customCuttingPts[i], customCuttingPts[j]) && meshCuttedChecker.find(std::make_pair(mv1, i)) == meshCuttedChecker.end() && meshCuttedChecker.find(std::make_pair(mv2, i)) == meshCuttedChecker.end()) {
                Custom3DPoint inter = getInter(mv1, mv2, customCuttingPts[i], customCuttingPts[j]);

                meshCuttedChecker.insert(std::make_pair(inter, i));

                meshFaceQueue.push(make_pair(convert2vector(mv1, inter, mv3), parentMesh));
                meshFaceQueue.push(make_pair(convert2vector(mv2, inter, mv3), parentMesh));
                break;
            }
            if (isIntersectedLines(mv2, mv3, customCuttingPts[i], customCuttingPts[j]) && meshCuttedChecker.find(std::make_pair(mv2, i)) == meshCuttedChecker.end() && meshCuttedChecker.find(std::make_pair(mv3, i)) == meshCuttedChecker.end()) {
                Custom3DPoint inter = getInter(mv2, mv3, customCuttingPts[i], customCuttingPts[j]);

                meshCuttedChecker.insert(std::make_pair(inter, i));

                meshFaceQueue.push(make_pair(convert2vector(mv2, inter, mv1), parentMesh));
                meshFaceQueue.push(make_pair(convert2vector(mv3, inter, mv1), parentMesh));
                break;
            }
            if (isIntersectedLines(mv3, mv1, customCuttingPts[i], customCuttingPts[j]) && meshCuttedChecker.find(std::make_pair(mv3, i)) == meshCuttedChecker.end() && meshCuttedChecker.find(std::make_pair(mv1, i)) == meshCuttedChecker.end()) {
                Custom3DPoint inter = getInter(mv3, mv1, customCuttingPts[i], customCuttingPts[j]);

                meshCuttedChecker.insert(std::make_pair(inter, i));

                meshFaceQueue.push(make_pair(convert2vector(mv3, inter, mv2), parentMesh));
                meshFaceQueue.push(make_pair(convert2vector(mv1, inter, mv2), parentMesh));
                break;
            }
        }

        // didn't cutted mesh
        if (i >= cuttingPointCnt) {
            int innerOutter = 0;
            int isOnLine;

            isOnLine = relationPointWithCuttingPolygon((mv1+mv2)/2, customCuttingPts, 5);
            if (isOnLine == 1) innerOutter++;
            else if (isOnLine == 0) innerOutter--;

            isOnLine = relationPointWithCuttingPolygon((mv2+mv3)/2, customCuttingPts, 5);
            if (isOnLine == 1) innerOutter++;
            else if (isOnLine == 0) innerOutter--;

            isOnLine = relationPointWithCuttingPolygon((mv3+mv1)/2, customCuttingPts, 5);
            if (isOnLine == 1) innerOutter++;
            else if (isOnLine == 0) innerOutter--;

            std::vector<Custom3DPoint> correctOrder = sortByCorrectOrderCustom3D(mv1, mv2, mv3, parentMesh[0], parentMesh[1], parentMesh[2]);
            correctOrder[0] = findConnectedPointOrRegister(&connectVertex, correctOrder[0]);
            correctOrder[1] = findConnectedPointOrRegister(&connectVertex, correctOrder[1]);
            correctOrder[2] = findConnectedPointOrRegister(&connectVertex, correctOrder[2]);

            QVector3D addingMesh1 = QVector3D(correctOrder[0].x/padding, correctOrder[0].y/padding, correctOrder[0].z/padding);
            QVector3D addingMesh2 = QVector3D(correctOrder[1].x/padding, correctOrder[1].y/padding, correctOrder[1].z/padding);
            QVector3D addingMesh3 = QVector3D(correctOrder[2].x/padding, correctOrder[2].y/padding, correctOrder[2].z/padding);
            if (innerOutter > 0)
                rightMesh->addFace(addingMesh1,addingMesh2,addingMesh3);
            else {
                leftMesh->addFace(addingMesh1,addingMesh2,addingMesh3);

                /*
                 * 1. slide by cutting line -> intersection point (in Mesh Cutted Checker)
                 * 2. ccw == 0 with cutting line also, point should be contained in cp1(x,y) cp2(x,y) box
                 *
                 * if Mesh edge's vertice are both included on 1 or 2 (cutting point is included in 2)
                 * => that edge is boundary edge
                 */
                for (i = 0, j = cuttingPointCnt - 1; i < cuttingPointCnt; j = i++){
                    std::vector<Custom3DPoint> boundaryVertice;
                    for (k = 0; k < 3; k++){
                        if (meshCuttedChecker.find(std::make_pair(correctOrder[k], i)) != meshCuttedChecker.end()){
                            boundaryVertice.push_back(correctOrder[k]);
                        } else if (CCW(customCuttingPts[i], customCuttingPts[j], correctOrder[k]) == 0 && min(customCuttingPts[i].x, customCuttingPts[j].x) <= correctOrder[k].x && correctOrder[k].x <= max(customCuttingPts[i].x, customCuttingPts[j].x) && min(customCuttingPts[i].y, customCuttingPts[j].y) <= correctOrder[k].y && correctOrder[k].y <= max(customCuttingPts[i].y, customCuttingPts[j].y)) {
                            boundaryVertice.push_back(correctOrder[k]);
                        }
                    }
                    if (boundaryVertice.size() == 2) {
                        boundaryEdges.insert(make_pair(std::make_pair(boundaryVertice[0], i), std::make_pair(boundaryVertice[1], i)));
                        boundaryEdges.insert(make_pair(std::make_pair(boundaryVertice[1], i), std::make_pair(boundaryVertice[0], i)));
                    }
                }
            }
        }
    }

    if (cutFillMode == 2) {
        // hole filling
        qDebug() << "boundary Edge " << boundaryEdges.size();

        for (i = 0, j = cuttingPointCnt-1; i < cuttingPointCnt; j = i++) {
            qDebug() << "cutting edge" << i;
            bool isCandidate;
            std::set<Custom3DPoint> checker;
            std::multimap<std::pair<Custom3DPoint, int>, std::pair<Custom3DPoint, int> >::iterator iter, targetIter;
            std::vector<std::vector<Custom3DPoint>> contours;
            std::pair<Custom3DPoint, int> prevPoint, currPoint;

            // find start with cutting point
            for (iter = boundaryEdges.begin(); iter != boundaryEdges.end(); iter++){
                isCandidate = (iter->first.second == i) && ((customCuttingPts[i].x == iter->first.first.x) && (customCuttingPts[i].y == iter->first.first.y) || ((customCuttingPts[j].x == iter->first.first.x) && (customCuttingPts[j].y == iter->first.first.y)));
                if (isCandidate && checker.find(iter->first.first) == checker.end()) {
                    std::vector<Custom3DPoint> contour;
                    // new contour
                    prevPoint = iter->first;
                    currPoint = iter->second;
                    contour.push_back(prevPoint.first);
                    checker.insert(prevPoint.first);
                    while(true){
                        prevPoint = currPoint;
                        contour.push_back(prevPoint.first);
                        checker.insert(prevPoint.first);

                        bool findFlag = false;

                        for (targetIter = boundaryEdges.lower_bound(prevPoint); targetIter != boundaryEdges.upper_bound(prevPoint); targetIter++){
                            currPoint = targetIter->second;
                            if (checker.find(currPoint.first) == checker.end()){
                                findFlag = true;
                                break;
            }
        }

                        if (!findFlag) {
                            break;
        }
        }
                    qDebug() << "cutting edge start >> " << contour.size();
                    contours.push_back(contour);
    }
            }

            // find non start with cutting point
            for (iter = boundaryEdges.begin(); iter != boundaryEdges.end(); iter++){
                isCandidate = iter->first.second == i;
                if (isCandidate && checker.find(iter->first.first) == checker.end()) {
                    std::vector<Custom3DPoint> contour;
                    // new contour
                    prevPoint = iter->first;
                    currPoint = iter->second;
                    contour.push_back(prevPoint.first);
                    checker.insert(prevPoint.first);
                    while(true){
                        prevPoint = currPoint;
                        contour.push_back(prevPoint.first);
                        checker.insert(prevPoint.first);

                        bool findFlag = false;

                        for (targetIter = boundaryEdges.lower_bound(prevPoint); targetIter != boundaryEdges.upper_bound(prevPoint); targetIter++){
                            currPoint = targetIter->second;
                            if (checker.find(currPoint.first) == checker.end()){
                                findFlag = true;
                                break;
                    }
                }

                        if (!findFlag) {
                            break;
            }

        }
                    qDebug() << "plane start >> " << contour.size();
                    qDebug() << contour[0].x << contour[0].y << contour[0].z << "||" << contour.back().x << contour.back().y << contour.back().z;
                    contours.push_back(contour);
    }
    }
            qDebug() << "total contours : " << contours.size();

            qDebug() << "get seperated contour";
            std::vector<int> seperatedContourIdxs;
            for (k=0; k<contours.size(); k++){
                if ((customCuttingPts[i].x == contours[k][0].x) && (customCuttingPts[i].y == contours[k][0].y) && (customCuttingPts[j].x == contours[k].back().x) && (customCuttingPts[j].y == contours[k].back().y)) {
                    seperatedContourIdxs.push_back(k);
                } else if ((customCuttingPts[j].x == contours[k][0].x) && (customCuttingPts[j].y == contours[k][0].y) && (customCuttingPts[i].x == contours[k].back().x) && (customCuttingPts[i].y == contours[k].back().y)) {
                    std::reverse(contours[k].begin(), contours[k].end());
                    seperatedContourIdxs.push_back(k);
                    }
                }
            qDebug() << seperatedContourIdxs.size();

            qDebug() << "start to connect seperated contour";
            int minIdx, maxIdx, originMinIdx, originMaxIdx;
            long long int minz, maxz;
            while (seperatedContourIdxs.size() > 2) {
                qDebug() << seperatedContourIdxs.size();
                minz = 10000000000; maxz = -10000000000;
                for (k = 0; k < seperatedContourIdxs.size(); k++) {
                    if (contours[seperatedContourIdxs[k]][0].z < minz) {
                        minz = contours[seperatedContourIdxs[k]][0].z;
                        minIdx = k;
            }
                    if (contours[seperatedContourIdxs[k]][0].z > maxz) {
                        maxz = contours[seperatedContourIdxs[k]][0].z;
                        maxIdx = k;
        }
    }
                qDebug() << minIdx << " " << minz;
                qDebug() << maxIdx << " " << maxz;
                originMinIdx = seperatedContourIdxs[minIdx]; seperatedContourIdxs[minIdx] = seperatedContourIdxs.back(); seperatedContourIdxs.pop_back();
                originMaxIdx = seperatedContourIdxs[maxIdx]; seperatedContourIdxs[maxIdx] = seperatedContourIdxs.back(); seperatedContourIdxs.pop_back();

                std::reverse(contours[originMinIdx].begin(), contours[originMinIdx].end());
                contours[originMaxIdx].insert(contours[originMaxIdx].end(), contours[originMinIdx].begin(), contours[originMinIdx].end());
                contours[originMinIdx] = contours.back(); contours.pop_back();
    }
            qDebug() << "complete connect seperated contour => " << contours.size();



            // if size of contour less than 3 -> remove
            int contourN = contours.size();
            for (int k = 0; k < contourN; k++) {
                if (contours[k].size() < 3) {
                    contours[k] = contours.back();
                    contours.pop_back();
                    contourN--;
                    k--;
            }
        }

            qDebug() << "make projected contours";
            std::vector<std::vector<Point_2> > projContours;
            for (unsigned int k = 0; k < contourN; k++) {
                std::vector<Point_2> projContour;
                int nn = contours[k].size();
                for (unsigned int h = 0; h < nn; h++){
                    if (abs(customCuttingPts[i].x - customCuttingPts[j].x) >= abs(customCuttingPts[i].y - customCuttingPts[j].y))
                        projContour.push_back(Point_2(contours[k][h].x/padding, contours[k][h].z/padding));
                    else
                        projContour.push_back(Point_2(contours[k][h].y/padding, contours[k][h].z/padding));
                }
                contours[k].clear();
                projContours.push_back(projContour);
            }
            contours.clear();

            bool projContoursInter = false;
            //check projected contours have intersection
            for (unsigned int prj1 = 0; prj1 < projContours.size(); prj1++){
                unsigned int prj1N = projContours[prj1].size();
                for (unsigned int prj2 = prj1+1; prj2 < projContours.size(); prj2++){
                    unsigned int prj2N = projContours[prj2].size();
                    for (unsigned int i1 = 0, i2 = prj1N-1; i1 < prj1N; i2 = i1++){
                        for (unsigned int j1 = 0, j2 = prj2N-1; j1 < prj2N; j2 = j1++){
                            if (point2isIntersectedLines(projContours[prj1][i1], projContours[prj1][i2], projContours[prj2][j1], projContours[prj2][j2])) {
                                projContoursInter = true;
                                break;
                            }
                        }
                        if (projContoursInter) break;
                    }
                    if (projContoursInter) break;
                }
                if (projContoursInter) break;
            }

            qDebug() << "make cdt";
            if (projContoursInter) {
                for (unsigned int prj1 = 0; prj1 < projContours.size(); prj1++){
                    unsigned int prj1N = projContours[prj1].size();
                    CGAL::Delaunay_triangulation_2<Kernel> dt;
                    for (unsigned int i = 0; i < prj1N; i++)
                        dt.insert(projContours[prj1][i]);

                    Custom3DPoint point1, point2, point3;
                    for (CGAL::Delaunay_triangulation_2<Kernel>::Finite_faces_iterator fit = dt.finite_faces_begin(); fit != dt.finite_faces_end(); fit++) {
                        long long int x1, y1, x2, y2, x3, y3;
                        x1 = int(dt.triangle(fit)[0].x() * padding); y1 = int(dt.triangle(fit)[0].y() * padding);
                        x2 = int(dt.triangle(fit)[1].x() * padding); y2 = int(dt.triangle(fit)[1].y() * padding);
                        x3 = int(dt.triangle(fit)[2].x() * padding); y3 = int(dt.triangle(fit)[2].y() * padding);

                        if (abs(cuttingPoints[i].x() - cuttingPoints[j].x()) >= abs(cuttingPoints[i].y() - cuttingPoints[j].y())) {
                            point1 = Custom3DPoint(x1, (x1 - customCuttingPts[j].x) * (customCuttingPts[i].y - customCuttingPts[j].y) / (customCuttingPts[i].x - customCuttingPts[j].x) + customCuttingPts[j].y, y1);
                            point2 = Custom3DPoint(x2, (x2 - customCuttingPts[j].x) * (customCuttingPts[i].y - customCuttingPts[j].y) / (customCuttingPts[i].x - customCuttingPts[j].x) + customCuttingPts[j].y, y2);
                            point3 = Custom3DPoint(x3, (x3 - customCuttingPts[j].x) * (customCuttingPts[i].y - customCuttingPts[j].y) / (customCuttingPts[i].x - customCuttingPts[j].x) + customCuttingPts[j].y, y3);
                        } else {
                            point1 = Custom3DPoint((x1 - customCuttingPts[j].y) * (customCuttingPts[i].x - customCuttingPts[j].x) / (customCuttingPts[i].y - customCuttingPts[j].y) + customCuttingPts[j].x, x1, y1);
                            point2 = Custom3DPoint((x2 - customCuttingPts[j].y) * (customCuttingPts[i].x - customCuttingPts[j].x) / (customCuttingPts[i].y - customCuttingPts[j].y) + customCuttingPts[j].x, x2, y2);
                            point3 = Custom3DPoint((x3 - customCuttingPts[j].y) * (customCuttingPts[i].x - customCuttingPts[j].x) / (customCuttingPts[i].y - customCuttingPts[j].y) + customCuttingPts[j].x, x3, y3);
                        }
                        point1 = findConnectedPointOrRegister(&connectVertex, point1);
                        point2 = findConnectedPointOrRegister(&connectVertex, point2);
                        point3 = findConnectedPointOrRegister(&connectVertex, point3);

                        rightMesh->addFace(QVector3D(point1.x/padding, point1.y/padding, point1.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point3.x/padding, point3.y/padding, point3.z/padding));
                        rightMesh->addFace(QVector3D(point3.x/padding, point3.y/padding, point3.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point1.x/padding, point1.y/padding, point1.z/padding));
                        leftMesh->addFace(QVector3D(point1.x/padding, point1.y/padding, point1.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point3.x/padding, point3.y/padding, point3.z/padding));
                        leftMesh->addFace(QVector3D(point3.x/padding, point3.y/padding, point3.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point1.x/padding, point1.y/padding, point1.z/padding));
                    }
                }
            }
            else {
                // add polygon and constraints on CDT
                CDT cdt;
                for (unsigned int k = 0; k < contourN; k++) {
                    std::vector<Vertex_handle> vhs;
                    size_t numPoints = projContours[k].size();

                    for (unsigned int h = 0; h < numPoints; h++){
                        vhs.push_back(cdt.insert(projContours[k][h]));
                    }

                    for (unsigned int h = 0; h < numPoints; h++)
                        cdt.insert_constraint(vhs[h], vhs[(h+1)%numPoints]);
                }

                qDebug() << "contour relation initialize";
                // I assumed that the maximum number of contours is 100.
                int directedInclusionRelationship[100][100];
                for (int contourI=0; contourI < contourN; contourI++) {
                    for (int contourJ=0; contourJ < contourN; contourJ++) {
                        directedInclusionRelationship[contourI][contourJ] = 0;
                    }
                }
                qDebug() << "contour relation make";
                // make directed adjacency matrix
                for (int contourI=0; contourI < contourN; contourI++) {
                    for (int contourJ=0; contourJ < contourN; contourJ++) {
                        if (contourI == contourJ) continue;
                        int relation;
                        for (int kkk=0; kkk<=projContours[contourJ].size(); kkk++){
                            relation = relationPoint2DPolygon(projContours[contourJ][kkk].x(), projContours[contourJ][kkk].y(), projContours[contourI]);
                            if (relation != 2){
                                break;
                            }
                        }
                        if (relation == 0) {
                            directedInclusionRelationship[contourI][contourJ] = 1;
                        }
                    }
                }
                // using Flody-Warshall get topology
                qDebug() << "contour topology";
                for (int contourI=0; contourI < contourN; contourI++) {
                    for (int contourJ=0; contourJ < contourN; contourJ++) {
                        if (contourI == contourJ) continue;
                        for (int contourK=0; contourK < contourN; contourK++) {
                            if (contourI == contourK || contourJ == contourK) continue;
                            if (directedInclusionRelationship[contourI][contourK] > 0 && directedInclusionRelationship[contourK][contourJ] > 0){
                                directedInclusionRelationship[contourI][contourJ] = directedInclusionRelationship[contourI][contourK] + directedInclusionRelationship[contourK][contourJ];
                            }
                        }
                    }
                }

                // find root nodes
                std::queue<std::pair<std::pair<int, bool>, int> > contourQ;
                for (int contourI=0; contourI < contourN; contourI++) {
                    int contourJ;
                    for (contourJ=0; contourJ < contourN; contourJ++) {
                        if (directedInclusionRelationship[contourJ][contourI] != 0)
                            break;
                    }
                    if (contourJ >= contourN) {
                        contourQ.push(std::make_pair(std::make_pair(contourI, true), -1));
                    }
                }

                qDebug() << "contour tree bfs -> hole point";
                // bfs for get hole point
                std::list<Point_2> list_of_seeds;
                std::pair<std::pair<int, bool>, int> qFront;
                std::pair<int, bool> contourT;
                int contourParent;
                while(!contourQ.empty()) {
                    qFront = contourQ.front(); contourQ.pop();
                    contourT = qFront.first; contourParent = qFront.second;
                    qDebug() << projContours[contourT.first].size() << " " << contourT.second;
                    if (contourT.second && contourParent != -1) {
                        // find hole point
                        Point_2 holePoint = findHolePoint(projContours[contourT.first], projContours[contourParent]);
                        list_of_seeds.push_back(holePoint);
                    }
                    for (int contourI=0; contourI < contourN; contourI++) {
                        if (directedInclusionRelationship[contourT.first][contourI] == 1) {
                            contourQ.push(std::make_pair(std::make_pair(contourI, !contourT.second), contourT.first));
                        }
                    }
                }

                // triangular
                qDebug() << "Triangular";
                if (list_of_seeds.size() > 0)
                    CGAL::refine_Delaunay_mesh_2(cdt, list_of_seeds.begin(), list_of_seeds.end(), Criteria(0.125, 1.5), false);
                else
                    CGAL::refine_Delaunay_mesh_2(cdt, Criteria(0.125, 1.5));

                // add face
                Custom3DPoint point1, point2, point3;
                qDebug() << "Add face";
                for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); fit++) {
                    if (fit->is_in_domain()) {
                        long long int x1, y1, x2, y2, x3, y3;
                        x1 = int(cdt.triangle(fit)[0].x() * padding); y1 = int(cdt.triangle(fit)[0].y() * padding);
                        x2 = int(cdt.triangle(fit)[1].x() * padding); y2 = int(cdt.triangle(fit)[1].y() * padding);
                        x3 = int(cdt.triangle(fit)[2].x() * padding); y3 = int(cdt.triangle(fit)[2].y() * padding);

                        if (abs(cuttingPoints[i].x() - cuttingPoints[j].x()) >= abs(cuttingPoints[i].y() - cuttingPoints[j].y())) {
                            point1 = Custom3DPoint(x1, (x1 - customCuttingPts[j].x) * (customCuttingPts[i].y - customCuttingPts[j].y) / (customCuttingPts[i].x - customCuttingPts[j].x) + customCuttingPts[j].y, y1);
                            point2 = Custom3DPoint(x2, (x2 - customCuttingPts[j].x) * (customCuttingPts[i].y - customCuttingPts[j].y) / (customCuttingPts[i].x - customCuttingPts[j].x) + customCuttingPts[j].y, y2);
                            point3 = Custom3DPoint(x3, (x3 - customCuttingPts[j].x) * (customCuttingPts[i].y - customCuttingPts[j].y) / (customCuttingPts[i].x - customCuttingPts[j].x) + customCuttingPts[j].y, y3);
                        } else {
                            point1 = Custom3DPoint((x1 - customCuttingPts[j].y) * (customCuttingPts[i].x - customCuttingPts[j].x) / (customCuttingPts[i].y - customCuttingPts[j].y) + customCuttingPts[j].x, x1, y1);
                            point2 = Custom3DPoint((x2 - customCuttingPts[j].y) * (customCuttingPts[i].x - customCuttingPts[j].x) / (customCuttingPts[i].y - customCuttingPts[j].y) + customCuttingPts[j].x, x2, y2);
                            point3 = Custom3DPoint((x3 - customCuttingPts[j].y) * (customCuttingPts[i].x - customCuttingPts[j].x) / (customCuttingPts[i].y - customCuttingPts[j].y) + customCuttingPts[j].x, x3, y3);
                        }
                        point1 = findConnectedPointOrRegister(&connectVertex, point1);
                        point2 = findConnectedPointOrRegister(&connectVertex, point2);
                        point3 = findConnectedPointOrRegister(&connectVertex, point3);

                        rightMesh->addFace(QVector3D(point1.x/padding, point1.y/padding, point1.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point3.x/padding, point3.y/padding, point3.z/padding));
                        rightMesh->addFace(QVector3D(point3.x/padding, point3.y/padding, point3.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point1.x/padding, point1.y/padding, point1.z/padding));
                        leftMesh->addFace(QVector3D(point1.x/padding, point1.y/padding, point1.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point3.x/padding, point3.y/padding, point3.z/padding));
                        leftMesh->addFace(QVector3D(point3.x/padding, point3.y/padding, point3.z/padding), QVector3D(point2.x/padding, point2.y/padding, point2.z/padding), QVector3D(point1.x/padding, point1.y/padding, point1.z/padding));
                    }
                }
                qDebug() << "add face complete";
            }
        }
    }

    qDebug() << "complete!!!";
//    leftMesh->connectFaces();
//    rightMesh->connectFaces();
//    qDebug() << "complete2!!!";
}
