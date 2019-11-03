#include "CorkMeshFactory.h"
#include "../../DentEngine/src/mesh.h"
CorkTriMesh Hix::Features::CSG::toCorkMesh(const Hix::Engine3D::Mesh& mesh)
{
	CorkTriMesh out;
	auto& vtcs = mesh.getVertices();
	auto& faces = mesh.getFaces();

	out.n_vertices = vtcs.size();
	out.n_triangles = faces.size();

	out.triangles = new uint[(out.n_triangles) * 3];
	out.vertices = new float[(out.n_vertices) * 3];
	auto faceEnd = faces.cend();
	auto vtcsEnd = vtcs.cend();
	size_t i = 0;
	for (auto faceItr = faces.cbegin(); faceItr != faceEnd; ++faceItr, ++i) {
		auto mv = faceItr.meshVertices();
		(out.triangles)[3 * i + 0] = mv[0].index();
		(out.triangles)[3 * i + 1] = mv[1].index();
		(out.triangles)[3 * i + 2] = mv[2].index();
	}
	i = 0;
	for (auto vtxItr = vtcs.cbegin(); vtxItr != vtcsEnd; ++vtxItr, ++i) {
		auto worldPos = vtxItr.worldPosition();
		(out.vertices)[3 * i + 0] = worldPos.x();
		(out.vertices)[3 * i + 1] = worldPos.y();
		(out.vertices)[3 * i + 2] = worldPos.z();
	}
	return out;
}

Hix::Engine3D::Mesh* Hix::Features::CSG::toHixMesh(const CorkTriMesh& mesh)
{
	auto out = new Hix::Engine3D::Mesh();
	auto tris = mesh.triangles;
	auto vtcs = mesh.vertices;
	for (uint i = 0; i < mesh.n_triangles; i++) {
		
		auto vIdx0 = mesh.triangles[3 * i + 0]*3;
		auto vIdx1 = mesh.triangles[3 * i + 1]*3;
		auto vIdx2 = mesh.triangles[3 * i + 2]*3;
		QVector3D v0(vtcs[vIdx0], vtcs[vIdx0 + 1], vtcs[vIdx0 + 2]);
		QVector3D v1(vtcs[vIdx1], vtcs[vIdx1 + 1], vtcs[vIdx1 + 2]);
		QVector3D v2(vtcs[vIdx2], vtcs[vIdx2 + 1], vtcs[vIdx2 + 2]);
		out->addFace(v0, v1, v2);
	}
	return out;
}
