#include "generateraft.h"
#include "generatesupport.h"
#include "DentEngine/src/configuration.h"

GenerateRaft::GenerateRaft()
{

}

float getRaftTopZ(const Mesh* mesh)
{
	return mesh->z_min() - scfg->support_base_height; //adds another layer as a margin
}

Mesh* GenerateRaft::generateRaft(Mesh* shellmesh, std::vector<OverhangPoint> overhangPoints) {

    Mesh* raftMesh = new Mesh();

    for (OverhangPoint op : overhangPoints){
        qDebug() << "overhangposition" << op.position;
        op.position.setZ(getRaftTopZ(shellmesh));
        generateKCylinder(raftMesh, op, scfg->raft_thickness);
    }

    raftMesh->connectFaces();
    qDebug() << "generateRaft Done";
    return raftMesh;
}

void GenerateRaft::generateFaces(Mesh* mesh, OverhangPoint top, OverhangPoint bottom) {
    for (size_t i = 0; i < 6; i++) {
        mesh->addFace(top.position + top.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      top.position + top.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      bottom.position + bottom.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0));
        mesh->addFace(bottom.position + bottom.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      bottom.position + bottom.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      top.position + top.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0));
    }
}

void GenerateRaft::generateCap(Mesh* mesh, OverhangPoint top, OverhangPoint bottom){
    for (size_t i=0; i<6; i++){
        mesh->addFace(top.position + top.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      top.position + top.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      top.position);
        mesh->addFace(bottom.position + bottom.radius * QVector3D(qCos(M_PI * (i+1) / 3), qSin(M_PI * (i+1) / 3), 0),
                      bottom.position + bottom.radius * QVector3D(qCos(M_PI * i / 3), qSin(M_PI * i / 3), 0),
                      bottom.position);
    }
}

void GenerateRaft::generateCylinder(Mesh* mesh, OverhangPoint op, float height){
    op.radius = scfg->raft_base_radius;

    OverhangPoint bottom_op;
    bottom_op.position = op.position - QVector3D(0,0,height);
    bottom_op.radius = scfg->raft_base_radius;

    generateFaces(mesh, op, bottom_op);
}

void GenerateRaft::generateKCylinder(Mesh* mesh, OverhangPoint op, float height){
    op.radius = scfg->raft_base_radius;

    OverhangPoint middle_op;
    middle_op.position = op.position - QVector3D(0,0,height/2);
    middle_op.radius = scfg->raft_offset_radius;

    generateFaces(mesh, op, middle_op);

    OverhangPoint bottom_op;
    bottom_op.position = op.position - QVector3D(0,0,height);
    bottom_op.radius = scfg->raft_base_radius;

    generateFaces(mesh, middle_op, bottom_op);

    generateCap(mesh, op, bottom_op);
}


