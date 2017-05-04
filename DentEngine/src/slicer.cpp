#include "slicer.h"
using namespace ClipperLib;

Paths subj;
Paths clip;
Paths solution;

int Slicer::slice(Mesh* mesh){
    vector<vector<Path>> slices = meshSlice(mesh);
    contourConstruct(slices);
}

// slices mesh into segments
vector<vector<Path>> Slicer::meshSlice(Mesh* mesh){
    float delta = cfg->layer_height;
    vector<float> planes;

    if (! strcmp(cfg->slicing_mode, "uniform")){
        planes = buildUniformPlanes(mesh->z_min, mesh->z_max, delta);
    } else if (cfg->slicing_mode == "adaptive") {
        // adaptive slice
        planes = buildAdaptivePlanes(mesh->z_min, mesh->z_max);
    }

    vector<vector<int>> triangleLists = buildTriangleLists(mesh, planes, delta);
    vector<vector<Path>> pathLists;

    vector<int> A;
    for (int i=0; i<planes.size(); i++){
        qDebug() << "slicing layer " << i << "/" << planes.size();
        A.insert(A.end(), triangleLists[i].begin(), triangleLists[i].end()); // union

        vector<Path>* paths = new vector<Path>;

        for (int t_idx=0; t_idx<A.size(); t_idx++){
            MeshFace cur_mf = mesh->idx2MF(A[t_idx]);
            if (mesh->getFaceZmax(cur_mf) < planes[i])
                A.erase(A.begin()+t_idx);
            else{
                // compute intersection
                paths->push_back(mesh->intersectionPath(cur_mf, planes[i]));
            }
        }
        pathLists.push_back(*paths);
    }

    return pathLists;
}

// construct closed contour using segments created from meshSlice step
int Slicer::contourConstruct(vector<vector<Path>>){

}

// builds vector of vector of triangle idxs sorted by z_min
vector<vector<int>> Slicer::buildTriangleLists(Mesh* mesh, vector<float> planes, float delta){

    // Create List of list
    vector<vector<int>> list_list_triangle;
    for (int l_idx=0; l_idx < planes.size()+1; l_idx ++){
        vector<int>* triangles_per_layer = new vector<int>;
        list_list_triangle.push_back(* triangles_per_layer);
    }

    // Uniform Slicing O(n)
    if (delta>0){
        for (int f_idx=0; f_idx<mesh->faces.size(); f_idx++){
            int llt_idx;
            MeshFace mf = mesh->idx2MF(f_idx);
            float z_min = mesh->getFaceZmin(mf);
            if (z_min < planes[0])
                llt_idx = 0;
            else if (z_min > planes[planes.size()-1])
                llt_idx = planes.size()-1;
            else
                llt_idx = ((z_min - planes[0])/delta) +1;

            list_list_triangle[llt_idx].push_back(f_idx);
        }
    }

    // General Case
    else {

    }

    return list_list_triangle;
}

vector<float> Slicer::buildUniformPlanes(float z_min, float z_max, float delta){
    vector<float> planes;
    for (int idx=0; idx<(z_max-z_min)/delta; idx++){
        planes.push_back(z_min + delta*idx);
    }
    return planes;
}

vector<float> Slicer::buildAdaptivePlanes(float z_min, float z_max){
    vector<float> planes;
    return planes;
}

/****************** Helper Functions *******************/

/*
int Slicer::sliceLayer(Mesh* mesh, float z)
{

    Paths subj;

    Path p;
    addPoint(100,100, &p);
    addPoint(200,100, &p);
    addPoint(200,200, &p);
    addPoint(100,200, &p);
    subj.push_back(p);

    Path p2;
    addPoint(150,50, &p2);
    addPoint(175,50, &p2);
    addPoint(175,250, &p2);
    addPoint(150,250, &p2);
    clip.push_back(p2);


    AddPaths(subj, ptSubject, true);
    AddPaths(clip, ptClip, true);
    Execute(ctIntersection, solution, pftNonZero, pftNonZero);

    printf("solution size = %d\n",(int)solution.size());
    for (unsigned i=0; i<solution.size(); i++)
    {
        Path p3 = solution.at(i);

        for (unsigned j=0; j<p3.size(); j++)
        {
            IntPoint ip = p3.at(j);
            printf("%d = %lld, %lld\n",j, ip.X,ip.Y);
        }

    }

    return 0;
}
*/
