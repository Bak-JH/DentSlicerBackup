#include "shelloffset.h"
#include "qmlmanager.h"

// offset shell with mm
void shellOffset(GLModel* glmodel, float factor){
    int cnt=0;
    Mesh* offsetMesh = new Mesh();
    offsetMesh->faces.reserve(glmodel->mesh->faces.size()*4);
    offsetMesh->vertices.reserve(glmodel->mesh->faces.size()*4);
    offsetMesh->prevMesh = glmodel->mesh->prevMesh;
    offsetMesh->nextMesh = glmodel->mesh->nextMesh;

    vector<MeshFace> unconnectedMeshFaces;
    vector<MeshFace> unconnectedOffsetMeshFaces;

    // copy original mesh for innershell in CCW order
    foreach (MeshFace mf, glmodel->mesh->faces){
        if (cnt%100 ==0)
            QCoreApplication::processEvents();
        cnt++;
        QVector3D qv1 = glmodel->mesh->idx2MV(mf.mesh_vertex[0]).position;
        QVector3D qv2 = glmodel->mesh->idx2MV(mf.mesh_vertex[1]).position;
        QVector3D qv3 = glmodel->mesh->idx2MV(mf.mesh_vertex[2]).position;
        offsetMesh->addFace(qv3, qv2, qv1);

        // will connect later on
        if (mf.neighboring_faces[0].size() == 0 || mf.neighboring_faces[1].size() == 0 || mf.neighboring_faces[2].size() == 0){ // edge 0 is unconnected
            unconnectedMeshFaces.push_back(mf);
            unconnectedOffsetMeshFaces.push_back(offsetMesh->faces[offsetMesh->faces.size()-1]);
        }
    }
    // 승환 20%
    qmlManager->setProgress(0.17);

    // offset vertices into normal direction
    offsetMesh->vertexOffset(factor);
    // 승환 40%
    qmlManager->setProgress(0.42);

    // copy original mesh for outer shell
    foreach (MeshFace mf, glmodel->mesh->faces){
        if (cnt%100 ==0)
            QCoreApplication::processEvents();
        cnt++;
        QVector3D qv1 = glmodel->mesh->idx2MV(mf.mesh_vertex[0]).position;
        QVector3D qv2 = glmodel->mesh->idx2MV(mf.mesh_vertex[1]).position;
        QVector3D qv3 = glmodel->mesh->idx2MV(mf.mesh_vertex[2]).position;
        offsetMesh->addFace(qv1, qv2, qv3);
    }
    // 승환 60%
    qmlManager->setProgress(0.54);

    // identify holes to connect
    for (int i=0; i<unconnectedMeshFaces.size(); i++){
        if (cnt%100 ==0)
            QCoreApplication::processEvents();
        cnt++;
        MeshFace umf = unconnectedMeshFaces[i];
        MeshFace uomf = unconnectedOffsetMeshFaces[i];
        if (umf.neighboring_faces[0].size() == 0){ // edge 0 is unconnected
            QVector3D qv0 = glmodel->mesh->idx2MV(umf.mesh_vertex[0]).position;
            QVector3D qv1 = glmodel->mesh->idx2MV(umf.mesh_vertex[1]).position;
            QVector3D qv0_in = offsetMesh->idx2MV(uomf.mesh_vertex[2]).position;
            QVector3D qv1_in = offsetMesh->idx2MV(uomf.mesh_vertex[1]).position;
            offsetMesh->addFace(qv1, qv0, qv0_in);
            offsetMesh->addFace(qv1, qv0_in, qv1_in);
        }
        if (umf.neighboring_faces[1].size() == 0){ // edge 1 is unconnected
            QVector3D qv1 = glmodel->mesh->idx2MV(umf.mesh_vertex[1]).position;
            QVector3D qv2 = glmodel->mesh->idx2MV(umf.mesh_vertex[2]).position;
            QVector3D qv1_in = offsetMesh->idx2MV(uomf.mesh_vertex[1]).position;
            QVector3D qv2_in = offsetMesh->idx2MV(uomf.mesh_vertex[0]).position;
            offsetMesh->addFace(qv2, qv1, qv1_in);
            offsetMesh->addFace(qv2, qv1_in, qv2_in);
        }
        if (umf.neighboring_faces[2].size() == 0){ // edge 2 is unconnected
            QVector3D qv0 = glmodel->mesh->idx2MV(umf.mesh_vertex[0]).position;
            QVector3D qv2 = glmodel->mesh->idx2MV(umf.mesh_vertex[2]).position;
            QVector3D qv0_in = offsetMesh->idx2MV(uomf.mesh_vertex[2]).position;
            QVector3D qv2_in = offsetMesh->idx2MV(uomf.mesh_vertex[0]).position;
            offsetMesh->addFace(qv0_in, qv0, qv2_in);
            offsetMesh->addFace(qv0, qv2, qv2_in);
        }
    }

    // connect built Mesh
    offsetMesh->connectFaces();
    // 승환 80%
    qmlManager->setProgress(0.83);

    /*GLModel* offsetModel = new GLModel(glmodel->parentModel->mainWindow, glmodel->parentModel, offsetMesh, "", false);
    float zlength = glmodel->mesh->z_max - glmodel->mesh->z_min;
    QVector3D tmp = offsetModel->m_transform->translation();
    offsetModel->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),zlength/2));
    */

    qmlManager->deleteModelFile(glmodel->ID);
    qmlManager->createModelFile(offsetMesh, glmodel->filename+"_offset");
    // 승환 100%
    qmlManager->setProgress(1);

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
