#include "stlexporter.h"
#include "qmlmanager.h"

STLexporter::STLexporter()
{

}

Mesh* STLexporter::mergeSelectedModels() {
    if (qmlManager->selectedModels.size() == 1 && qmlManager->selectedModels[0] == nullptr) return nullptr;
    else if (qmlManager->selectedModels.size() == 1)
        return qmlManager->selectedModels[0]->mesh;

    size_t faceNum = 0;
    size_t verNum = 0;
    size_t totalFaceNum = 0;
    foreach (GLModel* model, qmlManager->selectedModels) {
        faceNum += model->mesh->faces.size();
        verNum += model->mesh->vertices.size();
    }

    Mesh* mergedMesh = new Mesh();

    mergedMesh->faces.reserve(faceNum);
    mergedMesh->vertices.reserve(verNum);

    totalFaceNum = faceNum;
    faceNum = 0;
    verNum = 0;

    foreach (GLModel* model, qmlManager->selectedModels) {
        QVector3D trans = model->m_transform->translation();

        MeshFace newFace;
        for (vector<MeshFace>::iterator it = model->mesh->faces.begin(); it != model->mesh->faces.end(); ++it) {
            newFace = (*it);
            QVector3D v1 = model->mesh->idx2MV(newFace.mesh_vertex[0]).position+trans;
            QVector3D v2 = model->mesh->idx2MV(newFace.mesh_vertex[1]).position+trans;
            QVector3D v3 = model->mesh->idx2MV(newFace.mesh_vertex[2]).position+trans;
            mergedMesh->addFace(v1,v2,v3);
        }



/*
        model->mesh->vertexMove(trans);

        MeshVertex newVer;
        foreach(MeshVertex mv, model->mesh->vertices) {
            newVer = mv;
            newVer.idx += verNum;
            mergedMesh->vertices.push_back(newVer);
        }

        model->mesh->vertexMove(-trans);

        MeshFace newFace;
        for (vector<MeshFace>::iterator it = model->mesh->faces.begin(); it != model->mesh->faces.end(); ++it) {
            newFace = (*it);
            newFace.idx += faceNum;
            if (newFace.parent_idx != -1) newFace.parent_idx += faceNum;
            newFace.mesh_vertex[0] += verNum;
            newFace.mesh_vertex[1] += verNum;
            newFace.mesh_vertex[2] += verNum;
            mergedMesh->faces.push_back(newFace);
        }

        for (QHash<uint32_t, MeshVertex>::iterator it = model->mesh->vertices_hash.begin(); it!=model->mesh->vertices_hash.end(); ++it){
            mergedMesh->vertices_hash.insert(it.key(), it.value());
        }

        faceNum += model->mesh->faces.size();
        verNum += model->mesh->vertices.size();

        if (faceNum % 100 == 0) qmlManager->setProgress((float)(0.1 + 0.3*(faceNum/totalFaceNum)));*/
    }

    float xmid = (mergedMesh->x_max + mergedMesh->x_min)/2;
    float ymid = (mergedMesh->y_max + mergedMesh->y_min)/2;
    float zmid = (mergedMesh->z_max + mergedMesh->z_min)/2;

    mergedMesh->vertexMove(QVector3D(
                           (-1)*xmid,
                           (-1)*ymid,
                           (-1)*zmid));

    mergedMesh->connectFaces();

    return mergedMesh;
}

void STLexporter::exportSTL(QString outfilename){
    Mesh* mesh;

    qmlManager->setProgress(0);
    qmlManager->setProgressText("saving");

    size_t num = qmlManager->selectedModels.size();
    if (num > 1) mesh = mergeSelectedModels();

    qDebug() << "export STL";

    ofstream outfile(outfilename.toStdString().c_str(), ios::out);

    writeHeader(outfile);
    qmlManager->setProgress(0.5);
    //qmlManager->setProgress(0.1);
    QCoreApplication::processEvents();

    size_t total_cnt = mesh->faces.size();
    size_t cnt = 0;
    for (MeshFace mf : mesh->faces){
        writeFace(outfile, mesh, mf);

        if (cnt %100 == 0){
            QCoreApplication::processEvents();
            qmlManager->setProgress((float)(0.5 + 0.4*(cnt/total_cnt)));
        }
        cnt++;
    }

    writeFooter(outfile);

    outfile.close();
    qmlManager->setProgress(1);
    QCoreApplication::processEvents();
    qmlManager->openResultPopUp("","File Saved","");

    if (num > 1) delete mesh;

    return;
}

void STLexporter::writeFace(ofstream& outfile, Mesh* mesh, MeshFace mf){

    outfile << "facet normal "<< mf.fn.x() <<" "<< mf.fn.y()<<" "<< mf.fn.z() << "\n";
    outfile << "    outer loop\n";
    MeshVertex mv1 = mesh->idx2MV(mf.mesh_vertex[0]);
    MeshVertex mv2 = mesh->idx2MV(mf.mesh_vertex[1]);
    MeshVertex mv3 = mesh->idx2MV(mf.mesh_vertex[2]);
    outfile << "        vertex "<< mv1.position.x()<<" "<< mv1.position.y()<<" "<< mv1.position.z()<<"\n";
    outfile << "        vertex "<< mv2.position.x()<<" "<< mv2.position.y()<<" "<< mv2.position.z()<<"\n";
    outfile << "        vertex "<< mv3.position.x()<<" "<< mv3.position.y()<<" "<< mv3.position.z()<<"\n";
    outfile << "    endloop\n";
    outfile << "endfacet\n";
}

void STLexporter::writeHeader(ofstream& outfile){
    outfile << "solid diridiri\n";
}

void STLexporter::writeFooter(ofstream& outfile){
    outfile << "endsolid diridiri";
}
