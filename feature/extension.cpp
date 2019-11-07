#include "extension.h"
#include "render/ModelMaterial.h"
#include "render/Color.h"
#include "glmodel.h"

using namespace Hix::Debug;

Hix::Features::Extend::Extend(const std::unordered_set<GLModel*>& selectedModels):_models(selectedModels)
{
	for (auto each : _models)
	{
		each->setMaterialMode(Hix::Render::ShaderMode::PerPrimitiveColor);
		each->updateMesh(true);
	}
}

Hix::Features::Extend::~Extend()
{
	for (auto each : _models)
	{
		each->setMaterialMode(Hix::Render::ShaderMode::SingleColor);
		each->updateMesh(true);
		each->unselectMeshFaces();
		each->setTargetSelected(false);
	}
}

void Hix::Features::Extend::extendMesh(Mesh* mesh, const QVector3D& normal, const std::unordered_set<FaceConstItr>& extension_faces, double distance){

    Paths3D extension_outlines = detectExtensionOutline(mesh, extension_faces);
    extendAlongOutline(mesh, normal, extension_outlines, distance);
    coverCap(mesh, normal, extension_faces, distance);
}


Paths3D Hix::Features::Extend::detectExtensionOutline(Mesh* mesh, const std::unordered_set<FaceConstItr>& meshfaces){
    Mesh temp_mesh;
    for (auto mf : meshfaces){
		auto meshVertices = mf.meshVertices();

        temp_mesh.addFace(
				meshVertices[0].localPosition(),
                meshVertices[1].localPosition(),
                meshVertices[2].localPosition());
    }

    Paths3D temp_edges;
	auto faceCend = temp_mesh.getFaces().cend();
	for (auto mf = temp_mesh.getFaces().cbegin(); mf != faceCend; ++mf)
	{
		auto meshVertices = mf.meshVertices();
		auto edgeCirc = mf.edge();
		if (edgeCirc.nonOwningFaces().size() == 0){ // edge 0 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(*meshVertices[0]);
            temp_edge.push_back(*meshVertices[1]);
            temp_edges.push_back(temp_edge);
        }
		++edgeCirc;
        if (edgeCirc.nonOwningFaces().size() == 0){ // edge 1 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(*meshVertices[1]);
            temp_edge.push_back(*meshVertices[2]);
            temp_edges.push_back(temp_edge);
        }
		++edgeCirc;
        if (edgeCirc.nonOwningFaces().size() == 0){ // edge 2 is unconnected
            Path3D temp_edge;
            temp_edge.push_back(*meshVertices[2]);
            temp_edge.push_back(*meshVertices[0]);
            temp_edges.push_back(temp_edge);
        }
    }
    qDebug() << "collected edges : " << temp_edges.size();

    return temp_edges;
}

void Hix::Features::Extend::extendAlongOutline(Mesh* mesh, QVector3D normal, Paths3D selectedPaths, double distance){
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
            QVector3D qv0_in = next_paths[i][0].position;//offsetMesh->idx2MV(uomf.mesh_vertex[2].position();
            QVector3D qv1_in = next_paths[i][1].position;//offsetMesh->idx2MV(uomf.mesh_vertex[1].position();
            mesh->addFace(qv0_in, qv0, qv1);
            mesh->addFace(qv1_in, qv0_in, qv1);
        }

        // update path
        selectedPaths = next_paths;

        // reduce distance
        distance -= 1;
    }

    Path3D selectedPath;
    for (Path3D& edge : selectedPaths){
        bool exist = false;
        for (MeshVertex& mv : selectedPath){
            if(mv == edge[0])
                exist = true;
        }
        if (!exist)
            selectedPath.push_back(edge[0]);
    }
}

void Hix::Features::Extend::coverCap(Mesh* mesh, QVector3D normal,const std::unordered_set<FaceConstItr>& extension_faces, double distance){
    for (FaceConstItr mf : extension_faces){
		auto meshVertices = mf.meshVertices();
		mesh->addFace(
				meshVertices[0].localPosition() + distance*normal,
                meshVertices[1].localPosition() + distance*normal,
                meshVertices[2].localPosition() + distance*normal);
    }
}
