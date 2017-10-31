#include "modelloader.h"

// imports model from file (.stl, .obj)

ModelLoader::ModelLoader(QObject *parent) :
    QObject(parent)
{
    clearModel();
}

void ModelLoader::loadModel(QString path)
{
    QFile file(path);
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray header = file.read(80);
        file.close();

        if (header.trimmed().startsWith("solid") && header.contains("\n"))
        {
            qDebug("start to read stla");
            readSTLA(path);
        }
        else
        {
            qDebug("start to read stlb");
            readSTLB(path);
        }
    }
    else
    {
        qDebug() << "file open failed";
    }
}

void ModelLoader::readSTLA(QString path)
{
    clearModel();

    QDateTime time = QDateTime::currentDateTime();

    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QVector3D vn;
    QVector3D point1;
    QVector3D point2;
    QVector3D point3;
    QVector<QVector3D> triAngel;
    face *tempFace ;


    int count=0;

    QString line;
    QTextStream in(&file);
    line = in.readLine();                   // solid

    while(true)
    {
        line = in.readLine().trimmed();     //facet or endsolid
        if(line.trimmed().startsWith("endsolid"))
        {
            break;
        }
        vn = getCoordinateFromString(line);
        line = in.readLine();               //outer loop
        line = in.readLine().trimmed();     //vertex 1
        point1 = getCoordinateFromString(line);
        line = in.readLine().trimmed();     //vertex 2
        point2 = getCoordinateFromString(line);
        line = in.readLine().trimmed();     //vertex 3
        point3 = getCoordinateFromString(line);
        line = in.readLine();               //endloop
        line = in.readLine();               //endfacet

        triAngel.clear();
        triAngel.append(point1);
        triAngel.append(point2);
        triAngel.append(point3);

        updateMinMax(point1);
        updateMinMax(point2);
        updateMinMax(point3);

        tempFace = new face;
        tempFace->setNormalVector(vn);
        tempFace->setTriAngle(triAngel);

        faceList.append(tempFace);
        count++;
    }


    time = QDateTime::currentDateTime();
    file.close();
    emit signal_finishLoad();
}


void ModelLoader::readSTLB(QString path)
{
    clearModel();
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return;
    QVector3D vn;
    QVector3D point1;
    QVector3D point2;
    QVector3D point3;
    QVector<QVector3D> triAngel;
    face *tempFace;

    file.seek(0);
    QDataStream in(&file);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    quint32 n;
    quint16 control_bytes;
    quint32 count = 0;

    file.seek(80);
    in >> n;
    qDebug() << n << "triangles in the model.";

    float nx, ny, nz, x1, x2, x3, y1, y2, y3, z1, z2, z3;

    while (count < n)
    {
        file.seek(84+count*50+0+0);
        in >> nx;
        file.seek(84+count*50+0+4);
        in >> ny;
        file.seek(84+count*50+0+8);
        in >> nz;
        file.seek(84+count*50+12+0);
        in >> x1;
        file.seek(84+count*50+12+4);
        in >> y1;
        file.seek(84+count*50+12+8);
        in >> z1;
        file.seek(84+count*50+24+0);
        in >> x2;
        file.seek(84+count*50+24+4);
        in >> y2;
        file.seek(84+count*50+24+8);
        in >> z2;
        file.seek(84+count*50+36+0);
        in >> x3;
        file.seek(84+count*50+36+4);
        in >> y3;
        file.seek(84+count*50+36+8);
        in >> z3;
        file.seek(84+count*50+48);
        in >> control_bytes;

        if (control_bytes == 0 && in.status() == QDataStream::Ok)
        {
            vn = QVector3D(nx,ny,nz);
            point1= QVector3D(x1,y1,z1);
            point2= QVector3D(x2,y2,z2);
            point3= QVector3D(x3,y3,z3);

            triAngel.clear();
            triAngel.append(point1);
            triAngel.append(point2);
            triAngel.append(point3);

            updateMinMax(point1);
            updateMinMax(point2);
            updateMinMax(point3);

            tempFace = new face;
            tempFace->setNormalVector(vn);
            tempFace->setTriAngle(triAngel);

            faceList.append(tempFace);

            count++;
        }
        else
        {
            qDebug() << "File read error when readStlBinaryFormat"
                     << (int) in.status();

        }
    }
    file.close();
    qDebug()<<tr("finish load");
    qDebug()<<faceList.size();
    emit signal_finishLoad();
}

void ModelLoader::clearModel()
{
    faceList.clear();
    size.x_min = 0;
    size.x_max = 0;
    size.y_min = 0;
    size.y_max = 0;
    size.z_min = 0;
    size.z_max = 0;
}

/*void ModelLoader::model_draw()
{
    QVector<QVector3D> triAngle;
    QVector3D normal;
    GLfloat normalVector[3];
    for(int i=0; i<faceList.size(); i++)
    {
        normal = faceList.at(i)->getNormalVector();
        normal.normalize();
        normalVector[0] = normal.x();
        normalVector[1] = normal.y();
        normalVector[2] = normal.z();
        triAngle = faceList.at(i)->getTriAngle();
      glBegin(GL_TRIANGLES);
        glNormal3fv(normalVector);
        glVertex3f(triAngle.at(0).x(),triAngle.at(0).y(),triAngle.at(0).z() );
        glVertex3f(triAngle.at(1).x(),triAngle.at(1).y(),triAngle.at(1).z() );
        glVertex3f(triAngle.at(2).x(),triAngle.at(2).y(),triAngle.at(2).z() );
      glEnd();
    }

//        glBegin(GL_TRIANGLES);								// Drawing Using Triangles
//            glVertex3f( 0.0f, 1.0f, 0.0f);					// Top
//            glVertex3f(-1.0f,-1.0f, 0.0f);					// Bottom Left
//            glVertex3f( 1.0f,-1.0f, 0.0f);					// Bottom Right
//        glEnd();
}*/

void ModelLoader::model_test()
{
    QVector<QVector3D> test;
    if(faceList.isEmpty())
        return;
    for(int i=0;i<10;i++)
    {
        test = faceList.at(i)->getTriAngle();
    }
}

modelSize ModelLoader::getSize()
{
    return size;
}

void ModelLoader::updateMinMax(QVector3D point)
{
    float pointX = point.x();
    float pointY = point.y();
    float pointZ = point.z();

   if(pointX < size.x_min)
       size.x_min = pointX;
   else if(pointX > size.x_max)
       size.x_max = pointX;

   if(pointY < size.y_min)
       size.y_min = pointY;
   else if(pointY > size.y_max)
       size.y_max = pointY;

   if(pointZ < size.z_min)
       size.z_min = pointZ;
   else if(pointZ > size.z_max)
       size.z_max = pointZ;
}

QVector3D ModelLoader::getCenter(modelSize size)
{
    QVector3D center;
    center.setX( (size.x_max + size.x_min)/2 );
    center.setY( (size.y_max + size.y_min)/2 );
    center.setZ( (size.z_max + size.z_min)/2 );
    return center;
}

int ModelLoader::getFacesCount()
{
    return faceList.size();
}

QVector3D ModelLoader::getCoordinateFromString(QString line)
{
     QVector3D coordinate;
     QVector<float> point;

     QStringList strList = line.split(" ");

    int count = strList.size();
    for(int i=count-3;i<count;i++)
    {
        point.append(strList[i].toDouble());

    }
    coordinate.setX(point[0]);
    coordinate.setY(point[1]);
    coordinate.setZ(point[2]);
    return coordinate;
}


face::face(QObject *parent) :
    QObject(parent)
{
}
QVector<QVector3D> face::getTriAngle() const
{
    return triAngle;
}

void face::setTriAngle(const QVector<QVector3D > &value)
{
    triAngle = value;
}

QVector3D face::getNormalVector() const
{
    return normalVector;
}

void face::setNormalVector(const QVector3D &value)
{
    normalVector = value;
}
