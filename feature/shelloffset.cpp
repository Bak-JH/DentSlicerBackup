#include "shelloffset.h"
#include "qmlmanager.h"

// offset shell with mm
Mesh* ShellOffset::shellOffset(Mesh* mesh, float factor){
    int cnt=0;
    Mesh* offsetMesh = new Mesh(*mesh);

    std::vector<MeshFace> unconnectedOffsetMeshFaces;

    offsetMesh->connectFaces();
    for (const auto& mf: offsetMesh->getFaces()){
        auto edgeCirc = mf.edgeCirculator();
        for (size_t i = 0; i < 3; ++i, ++edgeCirc) {
            if (edgeCirc->nonOwningFaces().size() == 0)
            {
                unconnectedOffsetMeshFaces.push_back(mf);
                break;
            }
        }
    }

    qDebug() << "done copy and finding";
    // 승환 20%
    qmlManager->setProgress(0.17);

    // offset vertices into normal direction and reverse faces
    offsetMesh->vertexOffset(-factor);
    offsetMesh->reverseFaces();

    qDebug() << "done offsetting, reversing";
    // 승환 40%
    qmlManager->setProgress(0.42);

    (*offsetMesh) += (*mesh);

    qDebug() << "done copy2";
    // 승환 60%
    qmlManager->setProgress(0.54);

    // identify holes to connect
    for (int i=0; i<unconnectedOffsetMeshFaces.size(); i++){
        if (cnt%100 ==0)
            QCoreApplication::processEvents();
        cnt++;

        MeshFace uomf = unconnectedOffsetMeshFaces[i];

		auto uomfMVertices = uomf.meshVertices();
        auto edgeCirc = uomf.edgeCirculator();
        auto neighbors = edgeCirc->nonOwningFaces();
        if (neighbors.size() == 0){ // edge 0 is unconnected
            QVector3D qv0 = uomfMVertices[0]->position + uomfMVertices[0]->vn * factor;
            QVector3D qv1 = uomfMVertices[1]->position + uomfMVertices[1]->vn * factor;
            QVector3D qv0_in = uomfMVertices[0]->position;
            QVector3D qv1_in = uomfMVertices[1]->position;
            offsetMesh->addFaceAndConnect(qv1, qv0, qv0_in);
            offsetMesh->addFaceAndConnect(qv1, qv0_in, qv1_in);
        }
		++edgeCirc;
        neighbors = edgeCirc->nonOwningFaces();
        if (neighbors.size() == 0){ // edge 1 is unconnected
            QVector3D qv1 = uomfMVertices[1]->position + uomfMVertices[1]->vn * factor;
            QVector3D qv2 = uomfMVertices[2]->position + uomfMVertices[2]->vn * factor;
            QVector3D qv1_in = uomfMVertices[1]->position;
            QVector3D qv2_in = uomfMVertices[2]->position;
            offsetMesh->addFaceAndConnect(qv2, qv1, qv1_in);
            offsetMesh->addFaceAndConnect(qv2, qv1_in, qv2_in);
        }
		++edgeCirc;
        neighbors = edgeCirc->nonOwningFaces();
        if (neighbors.size() == 0){ // edge 2 is unconnected
            QVector3D qv0 = uomfMVertices[0]->position + uomfMVertices[0]->vn * factor;
            QVector3D qv2 = uomfMVertices[2]->position + uomfMVertices[2]->vn * factor;
            QVector3D qv0_in = uomfMVertices[0]->position;
            QVector3D qv2_in = uomfMVertices[2]->position;
            offsetMesh->addFaceAndConnect(qv0, qv2_in, qv0_in);
            offsetMesh->addFaceAndConnect(qv0, qv2, qv2_in);
        }
    }

    qDebug() << "done creating hole fills";

    // connect built Mesh
    //offsetMesh->connectFaces();
    // 승환 80%
    qmlManager->setProgress(0.83);

    // 승환 100%
    qmlManager->setProgress(1);

    qDebug() << "shell offsetting done";
	return offsetMesh;
}

void ShellOffset::connectHoles(Mesh* target_mesh, Paths3D from_holes, Paths3D to_holes){
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
