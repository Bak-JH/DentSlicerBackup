#include "modelcut.h"
/*
Mesh* bisectMesh(Mesh* mesh, Plane plane, ){
    // create new Mesh structure
    Mesh* leftMesh = new Mesh();
    Mesh* rightMesh = new Mesh();

    // do bisecting mesh
    foreach (MeshFace mf, mesh->faces){
        bool faceLeftToPlane = true;
        for (int vn=0; vn<3; vn++){
            MeshVertex mv = mesh->vertices[mf.mesh_vertex[vn]];
            if (!isLeftToPlane(plane, mv.position)) // if one vertex is left to plane, append to left vertices part
                faceLeftToPlane = false;
        }
        if (faceLeftToPlane)
            leftMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
        else
            rightMesh->addFace(mesh->vertices[mf.mesh_vertex[0]].position, mesh->vertices[mf.mesh_vertex[1]].position, mesh->vertices[mf.mesh_vertex[2]].position);
    }

    leftMesh->connectFaces();
    rightMesh->connectFaces();

    return (left ? leftMesh: rightMesh);
}

bool isLeftToPlane(Plane plane, QVector3D position){
    // determine if position is left to plane or not
    if(position.distanceToPlane(plane[0],plane[1],plane[2])>0)
        return false;
    return true;
}

void generatePlane(){

    if (cuttingPoints.size()<3){
        qDebug()<<"Error: There is not enough vectors to render a plane.";
        QCoreApplication::quit();
        return;
    }

    QVector3D v1;
    QVector3D v2;
    QVector3D v3;
    v1=cuttingPoints[cuttingPoints.size()-3];
    v2=cuttingPoints[cuttingPoints.size()-2];
    v3=cuttingPoints[cuttingPoints.size()-1];
        planeMaterial = new Qt3DExtras::QPhongMaterial();
        planeMaterial->setDiffuse(QColor(QRgb(0x00aaaa)));
        //To manipulate plane color, change the QRgb(0x~~~~~~).
        QVector3D world_origin(0,0,0);
        QVector3D original_normal(0,1,0);
        QVector3D desire_normal(QVector3D::normal(v1,v2,v3)); //size=1
        float angle = qAcos(QVector3D::dotProduct(original_normal,desire_normal))*180/M_PI;
        QVector3D crossproduct_vector(QVector3D::crossProduct(original_normal,desire_normal));

        for (int i=0;i<2;i++){
            clipPlane[i]=new Qt3DExtras::QPlaneMesh();
                clipPlane[i]->setHeight(30.0);
                clipPlane[i]->setWidth(30.0);

            planeTransform[i]=new Qt3DCore::QTransform();
                planeTransform[i]->setScale(2.0f);
                planeTransform[i]->setRotation(QQuaternion::fromAxisAndAngle(crossproduct_vector, angle+180*i));
                planeTransform[i]->setTranslation(desire_normal*(-world_origin.distanceToPlane(v1,v2,v3)));

            planeEntity[i] = new Qt3DCore::QEntity(parentEntity);
                planeEntity[i]->addComponent(clipPlane[i]);
                planeEntity[i]->addComponent(planeTransform[i]);
                planeEntity[i]->addComponent(planeMaterial);
        }
}

void addCuttingPoint(QVector3D v){
    cuttingPoints.push_back(v);
    sphereMesh[numPoints] = new Qt3DExtras::QSphereMesh;
    sphereMesh[numPoints]->setRadius(1);
    sphereTransform[numPoints] = new Qt3DCore::QTransform;
    sphereTransform[numPoints]->setTranslation(v);

    sphereMaterial[numPoints] = new Qt3DExtras::QPhongMaterial();
    sphereMaterial[numPoints]->setDiffuse(QColor(QRgb(0xaa0000)));
    sphereEntity[numPoints] = new Qt3DCore::QEntity(parentEntity);
    sphereEntity[numPoints]->addComponent(sphereMesh[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereTransform[numPoints]);
    sphereEntity[numPoints]->addComponent(sphereMaterial[numPoints]);
    numPoints++;
}


void removeCuttingPoints(Mesh* mesh){

    for(int i=0;i<mesh->numPoints;i++)
    {
        mesh->sphereEntity[i]->removeComponent(mesh->sphereMesh[i]);
        mesh->sphereEntity[i]->removeComponent(mesh->sphereTransform[i]);
        mesh->sphereEntity[i]->removeComponent(mesh->sphereMaterial[i]);
    }
    delete [] mesh->sphereEntity;
    delete [] mesh->sphereMaterial;
    delete [] mesh->sphereMesh;
    delete [] mesh->sphereTransform;
    mesh->numPoints=0;
}

void GLModel::modelcut(){

    removeCuttingPoints();
    generatePlane();
}
*/

