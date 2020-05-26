#include "meshrepair.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "TMesh/tmesh.h"
#include "../../glmodel.h"
#include "../addModel.h"
#include"../../common/Debug.h"
using namespace T_MESH;
using namespace Hix::Engine3D;
using namespace Hix::Features;

Paths3D  Hix::Features::identifyHoles(const Mesh* mesh) {

	// used for auto repair steps
	Paths3D holes;
	std::unordered_set<HalfEdgeConstItr> interesect;
	auto faceCend = mesh->getFaces().cend();
	for (auto mf = mesh->getFaces().cbegin(); mf != faceCend; ++mf)
	{
		auto edge = mf.edge();
		for (size_t i = 0; i < 3; ++i, edge.moveNext())
		{
			auto neighbors = edge.nonOwningFaces();
			if (neighbors.size() == 0)
			{
				Path3D temp_edge;
				auto meshVertices = mf.meshVertices();
				temp_edge.push_back(*meshVertices[i]);
				temp_edge.push_back(*meshVertices[(i + 1) % 3]);
				holes.push_back(temp_edge);
			}
			else if (neighbors.size() > 1)
			{
				if (neighbors.size() % 2 != 0)
				{
					qDebug() << "shit";
				}
			}
		}
	}
	qDebug() << "hole edges coutn : " << holes.size();

	// get closed contour from hole_edges
	holes = contourConstruct3D(holes);
	qDebug() << "hole detected";

	/*for (Paths3D::iterator ps_it = mesh->holes.begin(); ps_it != mesh->holes.end();){

		if (ps_it->size() <3)
			ps_it = mesh->holes.erase(ps_it);
		ps_it++;
	}*/
	qDebug() << "mesh hole count :" << holes.size();
	return holes;
}

bool Hix::Features::isRepairNeeded(const Hix::Engine3D::Mesh* mesh)
{
	auto edgeEnd = mesh->getHalfEdges().cend();
	for (auto edgeItr = mesh->getHalfEdges().cbegin(); edgeItr != edgeEnd; ++edgeItr)
	{
		if (edgeItr.twins().size() != 1)
			return true;
	}
	return false;
}

std::vector<Hix::Engine3D::HalfEdgeConstItr>  Hix::Features::getBoundaryEdges(const Hix::Engine3D::Mesh* mesh)
{
	std::vector<HalfEdgeConstItr> boundaryEdges;
	for (auto heItr = mesh->getHalfEdges().cbegin(); heItr != mesh->getHalfEdges().cend(); ++heItr)
	{
		if (heItr.twins().size() != 1)
		{
			boundaryEdges.emplace_back(heItr);
		}
	}
	return boundaryEdges;
}



std::vector<Hix::Engine3D::Mesh*> Hix::Features::seperateDisconnectedMeshes(Hix::Engine3D::Mesh* mesh)
{
	std::vector<Hix::Engine3D::Mesh*> meshes;
	std::deque<std::unordered_set<FaceConstItr>> connected;
	std::unordered_set<size_t> unexplored;
	auto meshFaces = mesh->getFaces();
	size_t totalFaceCnt = meshFaces.size();
	unexplored.reserve(totalFaceCnt);
	for (size_t i = 0; i < totalFaceCnt; ++i)
	{
		unexplored.insert(i);
	}

	while (!unexplored.empty())
	{
		auto& current = connected.emplace_back();
		auto begin = *unexplored.begin();
		std::deque<FaceConstItr>q;
		current.emplace(meshFaces.cbegin() + begin);
		unexplored.erase(begin);
		q.emplace_back(*current.begin());
		while (!q.empty())
		{
			auto curr = q.front();
			q.pop_front();
			auto edge = curr.edge();
			for (size_t i = 0; i < 3; ++i, edge.moveNext()) {
				auto nFaces = edge.twinFaces();
				for (auto nFace : nFaces)
				{
					if (current.find(nFace) == current.end())
					{
						q.emplace_back(nFace);
						current.emplace(nFace);
						unexplored.erase(nFace.index());
					}
				}
			}
		}
	}
	//if there are indeed multiple meshes, allocate new meshs
	if (connected.size() > 1)
	{
		meshes.reserve(connected.size());
		for (auto& faces : connected)
		{
			auto newMesh = new Mesh();
			meshes.emplace_back(newMesh);
			for (auto& face : faces)
			{
				newMesh->addFace(face);
			}
		}
	}
	else
	{
		meshes.emplace_back(mesh);
	}
	return meshes;
}






double closestPair(List *bl1, List *bl2, Vertex **closest_on_bl1, Vertex **closest_on_bl2)
{
	Node *n, *m;
	Vertex *v, *w;
	double adist, mindist = DBL_MAX;

	FOREACHVVVERTEX(bl1, v, n)
		FOREACHVVVERTEX(bl2, w, m)
	if ((adist = w->squaredDistance(v))<mindist)
	{
		mindist = adist;
		*closest_on_bl1 = v;
		*closest_on_bl2 = w;
	}

	return mindist;
}

bool joinClosestComponents(Basic_TMesh *tin)
{
	Vertex *v, *w, *gv, *gw;
	Triangle *t, *s;
	Node *n;
	List triList, boundary_loops, *one_loop;
	List **bloops_array;
	int i, j, numloops;

	// Mark triangles with connected component's unique ID 
	i = 0;
	FOREACHVTTRIANGLE((&(tin->T)), t, n) t->info = NULL;
	FOREACHVTTRIANGLE((&(tin->T)), t, n) if (t->info == NULL)
	{
		i++;
		triList.appendHead(t);
		t->info = (void *)i;

		while (triList.numels())
		{
			t = (Triangle *)triList.popHead();
			if ((s = t->t1()) != NULL && s->info == NULL) { triList.appendHead(s); s->info = (void *)i; }
			if ((s = t->t2()) != NULL && s->info == NULL) { triList.appendHead(s); s->info = (void *)i; }
			if ((s = t->t3()) != NULL && s->info == NULL) { triList.appendHead(s); s->info = (void *)i; }
		}
	}

	if (i<2)
	{
		FOREACHVTTRIANGLE((&(tin->T)), t, n) t->info = NULL;
		//   JMesh::info("Mesh is a single component. Nothing done.");
		return false;
	}

	FOREACHVTTRIANGLE((&(tin->T)), t, n)
	{
		t->v1()->info = t->v2()->info = t->v3()->info = t->info;
	}

	FOREACHVVVERTEX((&(tin->V)), v, n) if (!IS_VISITED2(v) && v->isOnBoundary())
	{
		w = v;
		one_loop = new List;
		do
		{
			one_loop->appendHead(w); MARK_VISIT2(w);
			w = w->nextOnBoundary();
		} while (w != v);
		boundary_loops.appendHead(one_loop);
	}
	FOREACHVVVERTEX((&(tin->V)), v, n) UNMARK_VISIT2(v);

	bloops_array = (List **)boundary_loops.toArray();
	numloops = boundary_loops.numels();

	int numtris = tin->T.numels();
	double adist, mindist = DBL_MAX;

	gv = NULL;
	for (i = 0; i<numloops; i++)
	for (j = 0; j<numloops; j++)
	if (((Vertex *)bloops_array[i]->head()->data)->info != ((Vertex *)bloops_array[j]->head()->data)->info)
	{
		adist = closestPair(bloops_array[i], bloops_array[j], &v, &w);
		if (adist<mindist) { mindist = adist; gv = v; gw = w; }
	}

	if (gv != NULL) tin->joinBoundaryLoops(gv, gw, 1, 0);

	FOREACHVTTRIANGLE((&(tin->T)), t, n) t->info = NULL;
	FOREACHVVVERTEX((&(tin->V)), v, n) v->info = NULL;

	free(bloops_array);
	while ((one_loop = (List *)boundary_loops.popHead()) != NULL) delete one_loop;

	return (gv != NULL);
}

Mesh* toHixMesh(const Basic_TMesh& input)
{
	Mesh* mesh = new Mesh();
	Node* n;
	Triangle* t;
	FOREACHTRIANGLEOFMESH(input, t, n)
	{
		mesh->addFace(
			QVector3D(t->v1()->x, t->v1()->y, t->v1()->z),
			QVector3D(t->v2()->x, t->v2()->y, t->v2()->z),
			QVector3D(t->v3()->x, t->v3()->y, t->v3()->z));
	}
	return mesh;
}



void toHixMesh(const Basic_TMesh& input, Hix::Engine3D::Mesh& mesh)
{
	Node* n;
	Triangle* t;
	mesh.clear();
	FOREACHTRIANGLEOFMESH(input, t, n)
	{
		mesh.addFace(
			QVector3D(t->v1()->x, t->v1()->y, t->v1()->z),
			QVector3D(t->v2()->x, t->v2()->y, t->v2()->z),
			QVector3D(t->v3()->x, t->v3()->y, t->v3()->z));
	}
}
void Hix::Features::repair(Hix::Engine3D::Mesh& mesh)
{
	TMesh::init(); // This is mandatory
	std::vector<size_t> triInds;
	std::vector<float> vtcs;
	auto& faces = mesh.getFaces();
	auto& mvs = mesh.getVertices();
	auto faceEnd = faces.cend();
	auto mvsEnd = mvs.cend();
	triInds.reserve(faces.size() * 3);
	triInds.reserve(mvs.size() * 3);
	for (auto itr = faces.cbegin(); itr != faceEnd; ++itr)
	{
		auto faceVtcs = itr.meshVertices();
		triInds.push_back(faceVtcs[0].index());
		triInds.push_back(faceVtcs[1].index());
		triInds.push_back(faceVtcs[2].index());
	}
	for (auto& vtx : mvs)
	{
		vtcs.push_back(vtx.position.x());
		vtcs.push_back(vtx.position.y());
		vtcs.push_back(vtx.position.z());
	}
	Basic_TMesh tin;
	tin.loadTriangleList(vtcs.data(), triInds.data(), mvs.size(), faces.size());
   //seperate into multiple components if there are disjoing components
	tin.removeSmallestComponents();

	// Fill holes
	if (tin.boundaries())
	{
		tin.fillSmallBoundaries(0, true);
	}

	// Run geometry correction
	if (!tin.boundaries()) TMesh::warning("Fixing degeneracies and intersections...\n");
	if (tin.boundaries() || !tin.meshclean())
		qDebug() << "mesh repair failed";
	
	toHixMesh(tin, mesh);

}

std::vector<Mesh*> Hix::Features::importAndRepairMesh(const std::string& importPath)
{
	std::vector<Hix::Engine3D::Mesh*> meshes;
	TMesh::init(); // This is mandatory
	 clock_t beginning = clock();

	 // Uncomment the following to prevent message reporting
	 // TMesh::quiet = true;

	 Basic_TMesh tin;
	 bool stl_output = false;
	 bool join_multiple_components = false;
	 if (tin.load(importPath.c_str()) != 0)
	 {
		 return meshes;
	 }

	 //if (join_multiple_components)
	 //{
		// TMesh::info("\nJoining input components ...\n");
		// TMesh::begin_progress();
		// while (joinClosestComponents(&tin)) TMesh::report_progress("Num. components: %d       ", tin.shells());
		// TMesh::end_progress();
		// tin.deselectTriangles();
	 //}

	//seperate into multiple components if there are disjoing components
	auto components = tin.seperateComponents();
	components.emplace_back(tin);
	meshes.reserve(components.size());
	//run repair
	for (auto& comp : components)
	{
		// Fill holes
		if (comp.boundaries())
		{
			comp.fillSmallBoundaries(0, true);
		}

		// Run geometry correction
		if (!comp.boundaries()) TMesh::warning("Fixing degeneracies and intersections...\n");
		if (comp.boundaries() || !comp.meshclean())
			qDebug() << "mesh repair failed";
		meshes.emplace_back(toHixMesh(comp));
	}


	 printf("Elapsed time: %d ms\n", clock() - beginning);

	return meshes;
}

Hix::Features::MeshRepair::MeshRepair(GLModel* selectedModel): _model(selectedModel)
{
}

Hix::Features::MeshRepair::~MeshRepair()
{
}

void Hix::Features::MeshRepair::runImpl()
{

	repairImpl(_model, _model->modelName());
}

GLModel* Hix::Features::MeshRepair::get()
{
	return _model;
}

void Hix::Features::MeshRepair::repairImpl(GLModel* subject, const QString& modelName)
{
	size_t childIdx = 0;
	for (auto childNode : subject->childNodes())
	{
		auto entity = dynamic_cast<GLModel*>(childNode);
		if (entity)
		{
			auto childName = modelName + "_child" + QString::number(childIdx);
			repairImpl(entity, childName);
			++childIdx;
		}
	}

	auto seperated = Hix::Features::seperateDisconnectedMeshes(subject->getMeshModd());
	//remove smallest components
	double avgSize = 0;
	for (auto sepComp : seperated)
	{
		avgSize += sepComp->getFaces().size();
	}
	avgSize /= seperated.size();
	avgSize *= 0.20;

	for (auto itr = seperated.begin(); itr != seperated.end();)
	{
		if ((double)(*itr)->getFaces().size() > (double)avgSize)
		{
			++itr;
		}
		else
		{
			delete (*itr);
			itr = seperated.erase(itr);
		}
	}
	for (auto sepComp : seperated)
	{
		Hix::Features::repair(*sepComp);
	}


	if (seperated.size() == 0)
	{
		qDebug() << "repair failed";
		return;
	}
	else if (seperated.size() == 1)
	{
		//do nothing, see seperateDisconnectedMeshes
		postUIthread([subject]() {
			subject->updateMesh(true);
		});

	}
	else
	{
		//mesh was split into seperate components, they should be added as children of the original subject
		//subject is now an "empty" node, with no mesh and just transform matrix
		postUIthread([subject, &seperated, &modelName, this]() {
			subject->clearMesh();
			subject->setMesh(new Mesh());
		});

		//add children via add model feature
		size_t childIdx = 0;
		Qt3DCore::QTransform emptyTransform;
		for (auto& comp : seperated)
		{
			auto addModel = new AddModel(subject, comp, modelName + "_child" + QString::number(childIdx), &emptyTransform);
			tryRunFeature(*addModel);
			addFeature(addModel);
			++childIdx;
		}
	}
}
