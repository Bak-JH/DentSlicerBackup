#include "hollowshell.h"

void HollowShell::hollowShell(Mesh* mesh,const MeshFace* mf, QVector3D center, float radius){
    std::vector<const MeshFace*> hollow_shell_faces;
    QVector3D normal = mf->fn;
    detectHollowShellFaces(mesh, normal, mf, mf, &hollow_shell_faces);
    offsetHollowShellFaces(mesh, &hollow_shell_faces, center, radius);

}

void HollowShell::detectHollowShellFaces(Mesh* mesh, QVector3D normal,const MeshFace* original_mf, const MeshFace* mf, std::vector<const MeshFace*>* result){
    result->push_back(mf);
	const auto& faces = mesh->getFaces();
	auto edgeCirc = mf->edgeCirculator();
	for (size_t i = 0; i < 3; ++i, ++edgeCirc)
	{
		auto neighbors = edgeCirc->nonOwningFaces();
		for (auto& neighbor : neighbors)
		{
			// check if neighbor already checked
			bool cont = false;
			for (const MeshFace* elem : (*result)) {
                if (elem == neighbor.toPtr())
					cont = true;
			}
			if (cont)
				continue;
			// check if neighbor close to normal
			if ((neighbor->fn - normal).length() > 0.5 || \
				neighbor->meshVertices()[0]->position.distanceToPoint(original_mf->meshVertices()[0]->position) > 30)
				continue;
			//qDebug() << mesh->idx2MV(neighbor->mesh_vertex[0]).position.distanceToPoint(mesh->idx2MV(original_mf->mesh_vertex[0]).position);
			//qDebug() << "looking for " << neighbor->idx;
            detectHollowShellFaces(mesh, normal, original_mf, neighbor.toPtr(), result);
		}
	}
}

void HollowShell::offsetHollowShellFaces(Mesh* mesh, std::vector<const MeshFace*>* hollow_shell_faces, QVector3D center, float radius){
    // offset faces from center with radius distance

    for (const MeshFace* mf : *hollow_shell_faces){
		auto meshVertices = mf->meshVertices();
        QVector3D v1 = meshVertices[0]->position;
        QVector3D v2 = meshVertices[1]->position;
        QVector3D v3 = meshVertices[2]->position;

        float distance2center = center.distanceToPlane(v1,v2,v3);
        if (distance2center < radius){ // offset shell
            for (int i=0; i<3; i++){
				auto moddableVtx = mesh->toNormItr(meshVertices[i]);
				moddableVtx->position = meshVertices[i]->position + mf->fn * (radius - distance2center);
            }
        }
    }

}
