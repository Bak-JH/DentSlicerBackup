#include "meshrepair.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "TMesh/tmesh.h"
#include "../../DentEngine/src/Mesh.h"

using namespace T_MESH;
using namespace Hix::Engine3D;
using namespace Hix::Features;
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

std::vector<Mesh*> importAndRepairMesh(const std::string& importPath)
{
	std::vector<Mesh*> meshes;
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
