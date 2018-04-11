#include "shelloffset.h"

// offset shell with mm
void shellOffset(GLModel* glmodel, float factor){

    Mesh* offsetMesh = new Mesh();
    offsetMesh->faces.reserve(glmodel->mesh->faces.size()*3);
    offsetMesh->vertices.reserve(glmodel->mesh->faces.size()*3);

    // copy original mesh for innershell in CCW order
    foreach (MeshFace mf, glmodel->mesh->faces){
        QVector3D qv1 = glmodel->mesh->idx2MV(mf.mesh_vertex[0]).position;
        QVector3D qv2 = glmodel->mesh->idx2MV(mf.mesh_vertex[1]).position;
        QVector3D qv3 = glmodel->mesh->idx2MV(mf.mesh_vertex[2]).position;
        offsetMesh->addFace(qv3, qv2, qv1);
    }


    // offset vertices into normal direction
    offsetMesh->vertexOffset(factor);

    // copy original mesh for outer shell
    foreach (MeshFace mf, glmodel->mesh->faces){
        QVector3D qv1 = glmodel->mesh->idx2MV(mf.mesh_vertex[0]).position;
        QVector3D qv2 = glmodel->mesh->idx2MV(mf.mesh_vertex[1]).position;
        QVector3D qv3 = glmodel->mesh->idx2MV(mf.mesh_vertex[2]).position;
        offsetMesh->addFace(qv1, qv2, qv3);
    }

    // identify holes to connect
    identifyHoles(glmodel->mesh);

    // copy hole indexes to offsetMesh holes
    offsetMesh->holes.reserve(glmodel->mesh->holes.size());
    for (Path3D p3d : glmodel->mesh->holes){
        Path3D p3d_copy;
        for (MeshVertex mv : p3d){
            p3d_copy.push_back(offsetMesh->idx2MV(mv.idx));
        }
        offsetMesh->holes.push_back(p3d_copy);
    }

    // connect offsetMesh's holes to outterMesh's holes
    // do something from glmodel->mesh->holes to offsetMesh->holes
    //connectHoles(offsetMesh, glmodel->mesh->holes, offsetMesh->holes);

    // connect built Mesh
    offsetMesh->connectFaces();

    GLModel* offsetModel = new GLModel(glmodel->parentModel->mainWindow, glmodel->parentModel, offsetMesh, "", false);
    float zlength = glmodel->mesh->z_max - glmodel->mesh->z_min;
    QVector3D tmp = offsetModel->m_transform->translation();
    offsetModel->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),zlength/2));

    qDebug() << "shell offsetting done";
}

void connectHoles(Mesh* target_mesh, Paths3D from_holes, Paths3D to_holes){
    for (int path_idx=0; path_idx<from_holes.size(); path_idx ++){
        Path3D fp = from_holes[path_idx];
        Path3D tp = to_holes[path_idx];

        for (int mv_idx=0; mv_idx<fp.size()-1; mv_idx ++){
            MeshVertex from_mv_cur = fp[mv_idx];
            MeshVertex from_mv_next = fp[mv_idx+1];
            MeshVertex to_mv_cur = tp[mv_idx];
            MeshVertex to_mv_next = tp[mv_idx+1];

            target_mesh->addFace(from_mv_cur.position, from_mv_next.position, to_mv_cur.position);
            target_mesh->addFace(to_mv_cur.position, to_mv_next.position, from_mv_next.position);
        }

        target_mesh->addFace(fp[fp.size()-1].position, fp[0].position, tp[tp.size()-1].position);
        target_mesh->addFace(tp[tp.size()-1].position, tp[0].position, fp[0].position);
    }
}
