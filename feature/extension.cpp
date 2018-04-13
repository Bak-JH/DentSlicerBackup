#include "extension.h"

void extendMesh(Mesh* mesh, MeshFace* mf, float distance){
    QVector3D normal = mf->fn;
    qDebug() << normal;

    vector<MeshFace*> extension_faces;
    detectExtensionFaces(mesh, normal, mf, &extension_faces);
    qDebug() << "detected extension faces" << extension_faces.size();

    Path3D extension_outlines = detectExtensionOutline(mesh, extension_faces);
    qDebug() << "detected extension outlines" << extension_outlines.size();

    // delete extension_faces

    extendAlongOutline(mesh, normal, extension_outlines, distance);
    qDebug() << "extended along outline";
}

void detectExtensionFaces(Mesh* mesh, QVector3D normal, MeshFace* mf, vector<MeshFace*>* result){
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
            if ((neighbor->fn - normal).length() > 0.1)
                continue;

            qDebug() << "looking for " << neighbor->idx;
            detectExtensionFaces(mesh, normal, neighbor, result);
        }
    }

    return;
}

Path3D detectExtensionOutline(Mesh* mesh, vector<MeshFace*> meshfaces){
    qDebug() <<"detect extension outline" << meshfaces.size();
    Mesh* temp_mesh = new Mesh();
    for (MeshFace* mf : meshfaces){
        temp_mesh->addFace(mesh->idx2MV(mf->mesh_vertex[0]).position, \
                mesh->idx2MV(mf->mesh_vertex[1]).position, \
                mesh->idx2MV(mf->mesh_vertex[2]).position);
    }

    temp_mesh->connectFaces();
    Paths3D result;

    result = spreadingCheck3D(*temp_mesh, 0);
    qDebug() << "spreading check done" << result.size();
    return result[0];
}

void extendAlongOutline(Mesh* mesh, QVector3D normal, Path3D selectedPath, float distance){
    // drill along selected faces

    while (distance>0){
        vector<vector<QVector3D>> interpolated_faces;

        // prolong next Path
        Path3D next_path;
        next_path.reserve(selectedPath.size());
        for (int i=0; i<selectedPath.size(); i++){
            next_path.push_back(selectedPath[i]);
            (*next_path.end()).position + normal;
        }

        // extend to next Path
        interpolated_faces = interpolate(selectedPath, next_path);

        // add Face to Mesh
        for (vector<QVector3D> face : interpolated_faces){
            mesh->addFace(face[0], face[1], face[2]);
        }

        // update path
        selectedPath = next_path;

        // reduce distance
        distance -= 1;
    }

    // fill holes
    vector<vector<QVector3D>> cap = fillPath(selectedPath);
    for (vector<QVector3D> face : cap){
        mesh->addFace(face[0],face[1],face[2]);
    }

    mesh->connectFaces();
}


