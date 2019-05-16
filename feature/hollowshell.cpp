#include "hollowshell.h"

void HollowShell::hollowShell(Mesh* mesh,const MeshFace* mf, QVector3D center, float radius){
    std::vector<const MeshFace*> hollow_shell_faces;
    QVector3D normal = mf->fn;
    detectHollowShellFaces(mesh, normal, mf, mf, &hollow_shell_faces);
    offsetHollowShellFaces(mesh, &hollow_shell_faces, center, radius);

}

void HollowShell::detectHollowShellFaces(Mesh* mesh, QVector3D normal,const MeshFace* original_mf, const MeshFace* mf, std::vector<const MeshFace*>* result){
    result->push_back(mf);
	const auto& faces(*mesh->getFaces());
    for (std::vector<const MeshFace*> neighbors : mf->neighboring_faces){
        for (const MeshFace* neighbor : neighbors){
            // check if neighbor already checked
            bool cont = false;
            for (const MeshFace* elem : (*result)){
                if (elem->idx == neighbor->idx)
                    cont = true;
            }
            if (cont)
                continue;
            // check if neighbor close to normal
            if ((neighbor->fn - normal).length() > 0.5 ||\
                    neighbor->mesh_vertex[0]->position.distanceToPoint(original_mf->mesh_vertex[0]->position) > 30)
                continue;
            //qDebug() << mesh->idx2MV(neighbor->mesh_vertex[0]).position.distanceToPoint(mesh->idx2MV(original_mf->mesh_vertex[0]).position);
            //qDebug() << "looking for " << neighbor->idx;
            detectHollowShellFaces(mesh, normal, original_mf, neighbor, result);
        }
    }

    return;
}

void HollowShell::offsetHollowShellFaces(Mesh* mesh, std::vector<const MeshFace*>* hollow_shell_faces, QVector3D center, float radius){
    // offset faces from center with radius distance

    for (const MeshFace* mf : *hollow_shell_faces){
        QVector3D v1 = mf->mesh_vertex[0]->position;
        QVector3D v2 = mf->mesh_vertex[1]->position;
        QVector3D v3 = mf->mesh_vertex[2]->position;

        float distance2center = center.distanceToPlane(v1,v2,v3);
        if (distance2center < radius){ // offset shell
            for (int i=0; i<3; i++){
				const auto & offsettedMeshVertex = *mf->mesh_vertex[i];
				mesh->modifyVertex(mf->mesh_vertex[i], offsettedMeshVertex.position + mf->fn * (radius - distance2center));
            }
        }
    }

}
