#include "extension.h"

void extendMesh(Mesh* mesh, MeshFace* mf, double distance){
    QVector3D normal = mf->fn;
    qDebug() << normal;

    vector<MeshFace*> extension_faces;
    detectExtensionFaces(mesh, normal, mf, mf, &extension_faces);
    qDebug() << "detected extension faces" << extension_faces.size();
    for (MeshFace* emf : extension_faces){
        //mesh->addFace(mesh->idx2MV(emf->mesh_vertex[0]).position+normal*2,mesh->idx2MV(emf->mesh_vertex[1]).position+normal*2,mesh->idx2MV(emf->mesh_vertex[2]).position+normal*2);
        qDebug() << "distance from selected extension_faces " << mesh->idx2MV(mf->mesh_vertex[0]).position.distanceToPoint(mesh->idx2MV(emf->mesh_vertex[0]).position);
    }

    Paths3D extension_outlines = detectExtensionOutline(mesh, extension_faces);
    qDebug() << "detected extension outlines" << extension_outlines.size();

    // delete extension_faces

    qDebug() << "mesh size : "<< mesh->faces.size();
    //mesh->addFace(mesh->idx2MV(mf->mesh_vertex[0]).position+normal*2,mesh->idx2MV(mf->mesh_vertex[1]).position+normal*2,mesh->idx2MV(mf->mesh_vertex[2]).position+normal*2);
    //mesh->connectFaces();
    extendAlongOutline(mesh, normal, extension_outlines, distance);
    qDebug() << "extended along outline";
    qDebug() << "mesh size : "<< mesh->faces.size();

    coverCap(mesh, normal, extension_faces, distance);
    mesh->connectFaces();
}

void detectExtensionFaces(Mesh* mesh, QVector3D normal, MeshFace* original_mf, MeshFace* mf, vector<MeshFace*>* result){
    result->push_back(mf);

    for (vector<MeshFace*> neighbors : mf->neighboring_faces){
        for (MeshFace* neighbor : neighbors){
            // check if neighbor already checked
            bool cont = false;
            for (MeshFace* elem : (*result)){
                if (elem->idx == neighbor->idx)
                    cont = true;
            }
            if (cont)
                continue;

            // check if neighbor close to normal
            if ((neighbor->fn - normal).length() > 0.5 ||\
                    mesh->idx2MV(neighbor->mesh_vertex[0]).position.distanceToPoint(mesh->idx2MV(original_mf->mesh_vertex[0]).position) > 10)
                continue;
            qDebug() << mesh->idx2MV(neighbor->mesh_vertex[0]).position.distanceToPoint(mesh->idx2MV(original_mf->mesh_vertex[0]).position);
            qDebug() << "looking for " << neighbor->idx;
            detectExtensionFaces(mesh, normal, original_mf, neighbor, result);
        }
    }

    return;
}

Paths3D detectExtensionOutline(Mesh* mesh, vector<MeshFace*> meshfaces){
    Mesh* temp_mesh;

    temp_mesh = new Mesh();
    for (MeshFace* mf : meshfaces){
        temp_mesh->addFace(mesh->idx2MV(mf->mesh_vertex[0]).position,\
                mesh->idx2MV(mf->mesh_vertex[1]).position,\
                mesh->idx2MV(mf->mesh_vertex[2]).position);
    }
    temp_mesh->connectFaces();

    Paths3D temp_edges;

    for (MeshFace mf : temp_mesh->faces){
        if (mf.neighboring_faces[0].size() == 0){ // edge 0 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(temp_mesh->idx2MV(mf.mesh_vertex[0]));
            temp_edge.push_back(temp_mesh->idx2MV(mf.mesh_vertex[1]));
            temp_edges.push_back(temp_edge);
        }
        if (mf.neighboring_faces[1].size() == 0){ // edge 1 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(temp_mesh->idx2MV(mf.mesh_vertex[1]));
            temp_edge.push_back(temp_mesh->idx2MV(mf.mesh_vertex[2]));
            temp_edges.push_back(temp_edge);
        }
        if (mf.neighboring_faces[2].size() == 0){ // edge 2 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(temp_mesh->idx2MV(mf.mesh_vertex[2]));
            temp_edge.push_back(temp_mesh->idx2MV(mf.mesh_vertex[0]));
            temp_edges.push_back(temp_edge);
        }
    }
    qDebug() << "collected edges : " << temp_edges.size();

    return temp_edges;
}

void extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPaths, double distance){
    // drill along selected faces

    while (distance>0){
        // prolong next Path
        Paths3D next_paths;
        next_paths.reserve(selectedPaths.size());
        for (int i=0; i<selectedPaths.size(); i++){
            next_paths.push_back(selectedPaths[i]);
            next_paths[i][0].position += normal;
            next_paths[i][1].position += normal;
            QVector3D qv0 = selectedPaths[i][0].position;
            QVector3D qv1 = selectedPaths[i][1].position;
            QVector3D qv0_in = next_paths[i][0].position;//offsetMesh->idx2MV(uomf.mesh_vertex[2]).position;
            QVector3D qv1_in = next_paths[i][1].position;//offsetMesh->idx2MV(uomf.mesh_vertex[1]).position;
            mesh->addFace(qv0_in, qv0, qv1);
            mesh->addFace(qv1_in, qv0_in, qv1);
        }

        // update path
        selectedPaths = next_paths;

        // reduce distance
        distance -= 1;
    }

    Path3D selectedPath;
    for (Path3D edge : selectedPaths){
        bool exist = false;
        for (MeshVertex mv : selectedPath){
            if(mv.idx == edge[0].idx)
                exist = true;
        }
        if (!exist)
            selectedPath.push_back(edge[0]);
    }
}

void coverCap(Mesh* mesh, QVector3D normal, vector<MeshFace*> extension_faces, double distance){
    for (MeshFace* mf : extension_faces){
        mesh->addFace(mesh->idx2MV(mf->mesh_vertex[0]).position + distance*normal,
                mesh->idx2MV(mf->mesh_vertex[1]).position + distance*normal,
                mesh->idx2MV(mf->mesh_vertex[2]).position + distance*normal);
    }
}
