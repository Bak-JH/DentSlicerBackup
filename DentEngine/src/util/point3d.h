#ifndef POINT3D_H
#define POINT3D_H


class Point3D
{
public:
    Point3D(float xp, float yp, float zp){x = xp; y = yp; z = zp;}
    float x;
    float y;
    float z;
    Point3D operator+(const Point3D& other);
    Point3D operator=(const Point3D& other);
};

#endif // POINT3D_H
