#include "stlexporter.h"
#include "qmlmanager.h"
using namespace Hix::Engine3D;
STLexporter::STLexporter()
{

}

Mesh* STLexporter::mergeSelectedModels() {
    std::vector<GLModel*> targetModels;

    auto& selectedModels = qmlManager->getSelectedModels();
    if (selectedModels.empty())
    {
        for (auto& pair : qmlManager->glmodels) {
            auto glm = &pair.second;
            targetModels.push_back(glm);
        }
        if (targetModels.size() == 0)
            return nullptr;
    }
    else
    {
        if (selectedModels.size() == 1)
        {
            //make copy of existing mesh
            auto existingMesh = (*selectedModels.cbegin())->getMesh();
            return new Mesh(*existingMesh);
        }
        else
        {
            targetModels.assign(selectedModels.cbegin(), selectedModels.cend());
        }
    }

    size_t faceNum = 0;
    size_t verNum = 0;
    size_t totalFaceNum = 0;
    for (GLModel* model: targetModels) {
        faceNum += model->getMesh()->getFaces().size();
        verNum += model->getMesh()->getVertices().size();
    }

    Mesh* mergedMesh = new Mesh();
    totalFaceNum = faceNum;
    faceNum = 0;
    verNum = 0;

    for (GLModel* model: targetModels) {
        QVector3D trans = model->m_transform.translation();

        for (const auto& each : model->getMesh()->getFaces()) {
            auto meshVertices = each.meshVertices();
            QVector3D v1 = meshVertices[0]->position+trans;
            QVector3D v2 = meshVertices[1]->position+trans;
            QVector3D v3 = meshVertices[2]->position+trans;
            mergedMesh->addFace(v1,v2,v3);
        }

        faceNum += model->getMesh()->getFaces().size();
        verNum += model->getMesh()->getVertices().size();

        if (faceNum % 100 == 0) qmlManager->setProgress((float)(0.1 + 0.3*(faceNum/totalFaceNum)));


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
        for (std::vector<MeshFace>::iterator it = model->mesh->faces.begin(); it != model->mesh->faces.end(); ++it) {
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

    float xmid = (mergedMesh->x_max() + mergedMesh->x_min())/2;
    float ymid = (mergedMesh->y_max() + mergedMesh->y_min())/2;
    float zmid = (mergedMesh->z_max() + mergedMesh->z_min())/2;

    mergedMesh->vertexMove(QVector3D(
                           (-1)*xmid,
                           (-1)*ymid,
                           (-1)*zmid));

    mergedMesh->connectFaces();

    return mergedMesh;
}

void STLexporter::exportSTL(QString outfilename){
    const Mesh* mesh;

    qmlManager->setProgress(0);
    qmlManager->setProgressText("saving");

    auto& selectedModels = qmlManager->getSelectedModels();
    if (selectedModels.empty())
        return;
    if (selectedModels.size() > 1) mesh = mergeSelectedModels();
    else mesh = (*selectedModels.cbegin())->getMesh();

    qDebug() << "export STL";

    QFile outfile(outfilename);
    std::stringstream contentStream;
    outfile.open(QFile::WriteOnly);

    writeHeader(contentStream);
    qmlManager->setProgress(0.5);
    //qmlManager->setProgress(0.1);
    QCoreApplication::processEvents();

    size_t total_cnt = mesh->getFaces().size();
    size_t cnt = 0;
    for (const auto& mf : mesh->getFaces()){
        writeFace(mesh, mf, contentStream);

        if (cnt %100 == 0){
            QCoreApplication::processEvents();
            qmlManager->setProgress((float)(0.5 + 0.4*(cnt/total_cnt)));
        }
        cnt++;
    }

    writeFooter(contentStream);
    outfile.write(QByteArray::fromStdString(contentStream.str()));

    outfile.close();
    qmlManager->setProgress(1);
    QCoreApplication::processEvents();
    qmlManager->openResultPopUp("","File Saved","");

    if (selectedModels.size() > 1) delete mesh;

    return;
}

void STLexporter::writeFace(const Mesh* mesh, MeshFace mf, std::stringstream& content){

    content << "facet normal "<< mf.fn.x() <<" "<< mf.fn.y()<<" "<< mf.fn.z() << "\n";
    content << "    outer loop\n";
    auto meshVertices = mf.meshVertices();

    auto& mv1 = meshVertices[0];
    auto& mv2 = meshVertices[1];
    auto& mv3 = meshVertices[2];
    content << "        vertex "<< mv1->position.x()<<" "<< mv1->position.y()<<" "<< mv1->position.z()<<"\n";
    content << "        vertex "<< mv2->position.x()<<" "<< mv2->position.y()<<" "<< mv2->position.z()<<"\n";
    content << "        vertex "<< mv3->position.x()<<" "<< mv3->position.y()<<" "<< mv3->position.z()<<"\n";
    content << "    endloop\n";
    content << "endfacet\n";
}

void STLexporter::writeHeader(std::stringstream& content){
    content << "solid diridiri\n";
}

void STLexporter::writeFooter(std::stringstream& content){
    content << "endsolid diridiri";
}
