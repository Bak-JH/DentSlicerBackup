#include "shelloffset.h"
#include "qmlmanager.h"

// offset shell with mm
Mesh* ShellOffset::shellOffset(Mesh* mesh, float factor){
    int cnt=0;
    Mesh* offsetMesh = new Mesh(mesh);

    std::vector<MeshFace> unconnectedMeshFaces;
    std::vector<MeshFace> unconnectedOffsetMeshFaces;

    // copy original mesh for innershell in CCW order
    for (const auto& mf: mesh->getFaces()){
        if (cnt%100 ==0)
            QCoreApplication::processEvents();
        cnt++;
		auto meshVertices = mf.meshVertices();

        QVector3D qv1 = meshVertices[0]->position;
        QVector3D qv2 = meshVertices[1]->position;
        QVector3D qv3 = meshVertices[2]->position;
        offsetMesh->addFace(qv3, qv2, qv1);

        // will connect later on
		auto edgeCirc = mf.edgeCirculator();
		for (size_t i = 0; i < 3; ++i, ++edgeCirc)
		{
			if (edgeCirc->nonOwningFaces().size() == 0)
			{
				unconnectedMeshFaces.push_back(mf);
				unconnectedOffsetMeshFaces.push_back(offsetMesh->getFaces().back());
				break;
			}
		}
    }
    // 승환 20%
    qmlManager->setProgress(0.17);

    // offset vertices into normal direction
    offsetMesh->vertexOffset(factor);
    // 승환 40%
    qmlManager->setProgress(0.42);

    // copy original mesh for outer shell
    for (const auto& mf: mesh->getFaces()){
        if (cnt%100 ==0)
            QCoreApplication::processEvents();
        cnt++;
		auto meshVertices = mf.meshVertices();

        QVector3D qv1 = meshVertices[0]->position;
        QVector3D qv2 = meshVertices[1]->position;
        QVector3D qv3 = meshVertices[2]->position;
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

		auto umfMVertices = umf.meshVertices();
		auto uomfMVertices = uomf.meshVertices();
		auto edgeCirc = umf.edgeCirculator();
		auto neighbors = edgeCirc->nonOwningFaces();
        if (neighbors.size() == 0){ // edge 0 is unconnected
            QVector3D qv0 = umfMVertices[0]->position;
            QVector3D qv1 = umfMVertices[1]->position;
            QVector3D qv0_in = uomfMVertices[2]->position;
            QVector3D qv1_in = uomfMVertices[1]->position;
            offsetMesh->addFace(qv1, qv0, qv0_in);
            offsetMesh->addFace(qv1, qv0_in, qv1_in);
        }
		++edgeCirc;
		neighbors = edgeCirc->nonOwningFaces();
        if (neighbors.size() == 0){ // edge 1 is unconnected
            QVector3D qv1 = umfMVertices[1]->position;
            QVector3D qv2 = umfMVertices[2]->position;
            QVector3D qv1_in = uomfMVertices[1]->position;
            QVector3D qv2_in = uomfMVertices[0]->position;
            offsetMesh->addFace(qv2, qv1, qv1_in);
            offsetMesh->addFace(qv2, qv1_in, qv2_in);
        }
		++edgeCirc;
		neighbors = edgeCirc->nonOwningFaces();
        if (neighbors.size() == 0){ // edge 2 is unconnected
            QVector3D qv0 = umfMVertices[0]->position;
            QVector3D qv2 = umfMVertices[2]->position;
            QVector3D qv0_in = uomfMVertices[2]->position;
            QVector3D qv2_in = uomfMVertices[0]->position;
            offsetMesh->addFace(qv0_in, qv0, qv2_in);
            offsetMesh->addFace(qv0, qv2, qv2_in);
        }
    }

    // connect built Mesh
    offsetMesh->connectFaces();
    // 승환 80%
    qmlManager->setProgress(0.83);

    /*GLModel* offsetModel = new GLModel(parentModel->mainWindow, parentModel, offsetMesh, "", false);
    float zlength = mesh->z_max - mesh->z_min;
    QVector3D tmp = offsetModel->m_transform->translation();
    offsetModel->m_transform->setTranslation(QVector3D(tmp.x(),tmp.y(),zlength/2));
    */

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
