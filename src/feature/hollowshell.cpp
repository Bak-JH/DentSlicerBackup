#include "hollowshell.h"
using namespace Hix::Engine3D;
void HollowShell::hollowShell(Mesh* mesh,const FaceConstItr& mf, QVector3D center, float radius){
    std::unordered_set<FaceConstItr> hollow_shell_faces;
    QVector3D normal = mf.localFn();
    detectHollowShellFaces(mesh, normal, mf, mf, hollow_shell_faces);
    offsetHollowShellFaces(mesh, hollow_shell_faces, center, radius);

}

void HollowShell::detectHollowShellFaces(Mesh* mesh, QVector3D normal, const FaceConstItr& original_mf, const FaceConstItr& mf, std::unordered_set<FaceConstItr>& result){
    result.insert(mf);
	auto edge = mf.edge();
	for (size_t i = 0; i < 3; ++i, edge.moveNext())
	{
		auto neighbors = edge.nonOwningFaces();
		for (auto& neighbor : neighbors)
		{
			// check if neighbor already checked
			if (result.find(neighbor) != result.end())
				continue;
			// check if neighbor close to normal
			if ((neighbor.localFn() - normal).length() > 0.5 || \
				neighbor.meshVertices()[0].localPosition().distanceToPoint(original_mf.meshVertices()[0].localPosition()) > 30)
				continue;
			//qDebug() << mesh->idx2MV(neighbor->mesh_vertex[0]).position.distanceToPoint(mesh->idx2MV(original_mf->mesh_vertex[0]).position);
			//qDebug() << "looking for " << neighbor->idx;
            detectHollowShellFaces(mesh, normal, original_mf, neighbor, result);
		}
	}
}

void HollowShell::offsetHollowShellFaces(Mesh* mesh, std::unordered_set<FaceConstItr>& hollow_shell_faces, QVector3D center, float radius){
    // offset faces from center with radius distance

    for (const auto& mf : hollow_shell_faces){
		auto meshVertices = mf.meshVertices();
        QVector3D v1 = meshVertices[0].localPosition();
        QVector3D v2 = meshVertices[1].localPosition();
        QVector3D v3 = meshVertices[2].localPosition();

        float distance2center = center.distanceToPlane(v1,v2,v3);
        if (distance2center < radius){ // offset shell
            for (int i=0; i<3; i++){
				auto moddableVtx = mesh->toNormItr(meshVertices[i]);
				moddableVtx.ref().position = meshVertices[i].localPosition() + mf.localFn() * (radius - distance2center);
            }
        }
    }

}
