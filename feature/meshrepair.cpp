#include "meshrepair.h"
#include "modelcut.h"
#include "qmlmanager.h"
#include <pcl/PCLPointCloud2.h>
#include <pcl/point_types.h>
#include <pcl/surface/marching_cubes_rbf.h>
#include <pcl/surface/marching_cubes_hoppe.h>
#include <pcl/features/normal_3d_omp.h>
#include <pcl/search/kdtree.h>
#include <pcl/pcl_macros.h>
#include <pcl/features/feature.h>
#include <pcl/geometry/triangle_mesh.h>
#include <pcl/geometry/mesh_conversion.h>
#include <pcl/surface/poisson.h>
#include <pcl/filters/passthrough.h>

#include <iostream>
using namespace Hix::Engine3D;
namespace MeshRepairPrivate
{
	Eigen::Matrix3f  calcCovarianceMatrix(const std::unordered_set<FaceConstItr>& faces);
	void calculateCurvature(pcl::PointNormal& output, const Eigen::Matrix3f& covarianceMat);
	void calculateNormalPoint(pcl::PointNormal& output, const VertexConstItr& meshVtx);
}

void MeshRepairPrivate::calculateNormalPoint(pcl::PointNormal& output, const VertexConstItr& meshVtx)
{
	//add position
	output.x = meshVtx.localPosition().x();
	output.y = meshVtx.localPosition().y();
	output.z = meshVtx.localPosition().z();

	auto connectedFaces = meshVtx.connectedFaces();
	//calculate vtx normal
	QVector3D vn;
	for (auto& face : connectedFaces)
	{
		vn += face.localFn();
	}
	vn.normalize();
	output.normal_x = vn.x();
	output.normal_y = vn.y();
	output.normal_z = vn.z();
	//calculate curvature
	auto convarianceMat = MeshRepairPrivate::calcCovarianceMatrix(connectedFaces);
	MeshRepairPrivate::calculateCurvature(output, convarianceMat);

}


void MeshRepairPrivate::calculateCurvature(pcl::PointNormal& output, const Eigen::Matrix3f& covarianceMat)
{
	// Extract the smallest eigenvalue and its eigenvector
	EIGEN_ALIGN16 Eigen::Vector3f::Scalar eigen_value;
	EIGEN_ALIGN16 Eigen::Vector3f eigen_vector;
	pcl::eigen33(covarianceMat, eigen_value, eigen_vector);


	// Compute the curvature surface change
	float eig_sum = covarianceMat.coeff(0) + covarianceMat.coeff(4) + covarianceMat.coeff(8);
	if (eig_sum != 0)
		output.curvature = fabsf(eigen_value / eig_sum);
	else
		output.curvature = 0;
}

Eigen::Matrix3f  MeshRepairPrivate::calcCovarianceMatrix(const std::unordered_set<FaceConstItr>& faces)
{
	Eigen::Matrix3f covariance_matrix = Eigen::Matrix3f::Zero();

	// create the buffer on the stack which is much faster than using cloud[indices[i]] and centroid as a buffer
	Eigen::Matrix<float, 1, 9, Eigen::RowMajor> accu = Eigen::Matrix<float, 1, 9, Eigen::RowMajor>::Zero();
	std::size_t point_count;

	std::unordered_set<VertexConstItr> neigbors;
	for (auto& each : faces)
	{
		auto faceVertices = each.meshVertices();
		for (auto& vtx : faceVertices)
		{
			neigbors.emplace(vtx);
		}
	}
	point_count = neigbors.size();
	for (auto& vtx : neigbors)
	{
		accu[0] += vtx.localPosition().x() *  vtx.localPosition().x();
		accu[1] += vtx.localPosition().x() *  vtx.localPosition().y();
		accu[2] += vtx.localPosition().x() *  vtx.localPosition().z();
		accu[3] += vtx.localPosition().y() *  vtx.localPosition().y();
		accu[4] += vtx.localPosition().y() *  vtx.localPosition().z();
		accu[5] += vtx.localPosition().z() *  vtx.localPosition().z();
		accu[6] += vtx.localPosition().x();
		accu[7] += vtx.localPosition().y();
		accu[8] += vtx.localPosition().z();
	}
	


	accu /= static_cast<float> (point_count);

	covariance_matrix.coeffRef(0) = accu[0] - accu[6] * accu[6];
	covariance_matrix.coeffRef(1) = accu[1] - accu[6] * accu[7];
	covariance_matrix.coeffRef(2) = accu[2] - accu[6] * accu[8];
	covariance_matrix.coeffRef(4) = accu[3] - accu[7] * accu[7];
	covariance_matrix.coeffRef(5) = accu[4] - accu[7] * accu[8];
	covariance_matrix.coeffRef(8) = accu[5] - accu[8] * accu[8];
	covariance_matrix.coeffRef(3) = covariance_matrix.coeff(1);
	covariance_matrix.coeffRef(6) = covariance_matrix.coeff(2);
	covariance_matrix.coeffRef(7) = covariance_matrix.coeff(5);
	return covariance_matrix;
}
template <bool IsManifoldT>
struct MeshTraits
{
	using VertexData = pcl::PointXYZ;
	using HalfEdgeData = pcl::geometry::NoData;
	using EdgeData = pcl::geometry::NoData;
	using FaceData = pcl::geometry::NoData;
	using IsManifold = std::integral_constant <bool, IsManifoldT>;
};

template<class PointClass>
QVector3D pclToQtPt(const PointClass& input)
{
	QVector3D pt;
	pt.setX(input.x);
	pt.setY(input.y);
	pt.setZ(input.z);
	return pt;
}

void remesh(GLModel* model)
{
	float default_iso_level = 0.1f;
	float default_extend_percentage = 10.0f;
	int default_grid_res = 1000;
	float default_off_surface_displacement = 0.01f;
	pcl::PointCloud<pcl::PointNormal>::Ptr cloud(new pcl::PointCloud<pcl::PointNormal>);

	auto mesh = model->getMesh();
	//cloud->reserve(mesh->getVertices().size());
	auto vtxCend = mesh->getVertices().cend();

	pcl::PointCloud<pcl::PointNormal>::Ptr benchmark(new pcl::PointCloud<pcl::PointNormal>);
	{

		using namespace pcl;
		pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);

		PointCloud<PointXYZ>::Ptr ptCloud(new PointCloud<PointXYZ>);
		for (auto vtxItr = mesh->getVertices().cbegin(); vtxItr != vtxCend; ++vtxItr)
		{
			auto vtxNormal = vtxItr.localVn();
			auto vtxPos = vtxItr.localPosition();
			ptCloud->push_back(pcl::PointXYZ());
			ptCloud->back().x = vtxPos.x();
			ptCloud->back().y = vtxPos.y();
			ptCloud->back().z = vtxPos.z();
		}

		NormalEstimationOMP<PointXYZ, Normal> ne;
		search::KdTree<PointXYZ>::Ptr tree1(new search::KdTree<PointXYZ>);
		tree1->setInputCloud(ptCloud);
		ne.setInputCloud(ptCloud);
		ne.setNumberOfThreads(6);
		ne.setSearchMethod(tree1);
		ne.setKSearch(20);
		ne.compute(*normals);
		for (size_t i = 0; i < normals->size(); ++i) {
			normals->points[i].normal_x *= -1;
			normals->points[i].normal_y *= -1;
			normals->points[i].normal_z *= -1;
		}

		concatenateFields(*ptCloud, *normals, *benchmark);
	}
	for (auto vtxItr = mesh->getVertices().cbegin(); vtxItr != vtxCend; ++vtxItr)
	{
		cloud->push_back(pcl::PointNormal());
		MeshRepairPrivate::calculateNormalPoint(cloud->back(), vtxItr);
	}
	//for (size_t i = 0; i < cloud->size(); ++i) {
	//	cloud->points[i].normal_x *= -1;
	//	cloud->points[i].normal_y *= -1;
	//	cloud->points[i].normal_z *= -1;
	//}


	pcl::PointCloud<pcl::PointNormal> outPts;
	std::vector<pcl::Vertices> outPoly;
	//pcl::MarchingCubesHoppe<pcl::PointNormal> mc;
	//mc.setIsoLevel(default_iso_level);
	//mc.setGridResolution(default_grid_res, default_grid_res, default_grid_res);
	//mc.setPercentageExtendGrid(default_extend_percentage);
	//mc.setInputCloud(cloud);
	//mc.reconstruct(outPts, outPoly);

	//add back to mesh;
	//polygon should be triangular due to the nature of marching cubes
	//using TriangleMesh = pcl::geometry::TriangleMesh<MeshTraits<true>>;
	//TriangleMesh triMesh;
	//pcl::geometry::toFaceVertexMesh(triMesh, output);
	//auto& faceData = triMesh.getFaceDataCloud();

	pcl::Poisson<pcl::PointNormal> poisson;
	poisson.setDepth(12);
	poisson.setInputCloud(cloud);
	poisson.reconstruct(outPts, outPoly);

	auto newMesh = new Hix::Engine3D::Mesh();
	for (auto& tri : outPoly)
	{
		if (tri.vertices.size() != 3)
		{
			throw std::runtime_error("output mesh from marching cubes is not triangular");
		}
		newMesh->addFace(
			pclToQtPt(outPts[tri.vertices[0]]),
			pclToQtPt(outPts[tri.vertices[1]]),
			pclToQtPt(outPts[tri.vertices[2]])
		);
	}
	model->setMesh(newMesh);
}

void MeshRepair::modelRepair(GLModel* model)
{

	remesh(model);

	qmlManager->setProgress(0.5);

    qmlManager->setProgress(0.8);
    qmlManager->setProgress(1);
}


// no need to remove unused vertices since it iterates faces
// removes totally unconnected faces
// if 2 edges are unconnected or 1 edge is unconnected, goto fillhole
void MeshRepair::removeUnconnected(Mesh* mesh){
    int unconnected_cnt = 0;
    size_t face_idx = 0;

    auto mf_it = mesh->getFaces().cbegin();

    while (mf_it != mesh->getFaces().cend()){
        face_idx ++;
		if (face_idx % 100 == 0)
		{
			//don't hog UI thread too long;
			QCoreApplication::processEvents();
		}
        int neighbor_cnt = 0;
		auto edge = mf_it.edge();
		for (size_t i = 0; i < 3; ++i, edge.moveNext())
		{
			neighbor_cnt += edge.nonOwningFaces().size();
		}

        if (neighbor_cnt == 0){
            qDebug() << "unconnected face";
            mf_it = mesh->removeFace(mf_it);
            unconnected_cnt ++;
        } else {
            mf_it ++;
        }
    }
    qDebug() << unconnected_cnt << "unconnected faces found, faces cnt :" << mesh->getFaces().size();
}
//DEPRECATED, addFace already do this.
// removes zero area triangles

Paths3D MeshRepair::identifyHoles(const Mesh* mesh){

    // used for auto repair steps
    Paths3D holes;
    int face_idx = 0;
	auto faceCend = mesh->getFaces().cend();
	for(auto mf = mesh->getFaces().cbegin(); mf != faceCend; ++mf)
	{
        face_idx ++;
        if (face_idx %100 ==0)
            QCoreApplication::processEvents();

        //qDebug() << "neighbors " << mf.neighboring_faces[0].size() << mf.neighboring_faces[1].size() << mf.neighboring_faces[2].size();
		auto edge = mf.edge();
		for (size_t i = 0; i < 3; ++i, edge.moveNext())
		{
			auto neighbors = edge.nonOwningFaces();
			if (neighbors.size() != 0)
			{
				Path3D temp_edge;
				auto meshVertices = mf.meshVertices();
				temp_edge.push_back(*meshVertices[i]);
				temp_edge.push_back(*meshVertices[(i+1)%3]);
				holes.push_back(temp_edge);
			}
		}
    }
    qDebug()<< "hole edges coutn : " << holes.size();

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


std::vector<Hix::Engine3D::HalfEdgeConstItr> MeshRepair::identifyBoundary(const Mesh* mesh)
{
	std::vector<HalfEdgeConstItr> boundaryEdges;
	for (auto heItr = mesh->getHalfEdges().cbegin(); heItr != mesh->getHalfEdges().cend(); ++heItr)
	{
		//if a half edge do not have a twin, it's a boundary edge
		auto twnFaces = heItr.twinFaces();
		if (twnFaces.empty())
		{
			boundaryEdges.push_back(heItr);

		}
	}
	return boundaryEdges;
}


// detects hole and remove them
void MeshRepair::fillHoles(Mesh* mesh, const Paths3D& holes){
    for (Path3D hole : holes){
        qDebug() << "filling hole" << hole.size() << "sized";
        if (hole.size() <= 2) { // if edge is less than 3 (no hole)
            continue;
        }

        QVector3D centerOfMass = QVector3D(0,0,0);
        for (MeshVertex mv : hole){
            centerOfMass += mv.position;
        }
        centerOfMass /= hole.size();

        // get orientation
        /*bool ccw = true;
        QVector3D current_plane_normal = QVector3D::normal(hole[1].position, centerOfMass, hole[0].position);
        if (QVector3D::dotProduct(current_plane_normal, plane_normal)>0){
            ccw = false;
        }*/

        for (int i=0; i<hole.size(); i++){
            mesh->addFace(hole[i].position, centerOfMass, hole[(i+1)%hole.size()].position);
            mesh->addFace(hole[(i+1)%hole.size()].position, centerOfMass, hole[i].position);
        }
        /*mesh->addFace(QVector3D(0,0,0), QVector3D(100,100,0), QVector3D(100,0,0));
        mesh->addFace(QVector3D(0,0,0), QVector3D(1000,100,0), QVector3D(100,0,0));*/
/*
        for (int i=0; i<hole.size(); i++){
            if (ccw){
                mesh->addFace(hole[i].position, centerOfMass, hole[(i+1)%hole.size()].position);
            } else {
                mesh->addFace(hole[(i+1)%hole.size()].position, centerOfMass, hole[i].position);
            }
        }
        */

        /*for (int i=0; i<contour.size(); i++){
            if (ccw){
                leftMesh->addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
                rightMesh->addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
            } else {
                leftMesh->addFace(contour[(i+1)%contour.size()].position, centerOfMass, contour[i].position);
                rightMesh->addFace(contour[i].position, centerOfMass, contour[(i+1)%contour.size()].position);
            }
        }*/

        /*
        qDebug() << "ok till here1";
        // fill holes
        qDebug() << "ok till here2";
        for (std::array<QVector3D,3> face : fillPath(hole)){
            mesh->addFace(face[0],face[1],face[2]);
        }
        qDebug() << "ok till here3";

        */

        /*Path3D prev_path;
        Path3D cur_path;

        for (Path3D::iterator vert_it = hole.begin()+1; vert_it != hole.end()-1; ++vert_it){
            MeshVertex prev_vert = (*(vert_it-1));
            MeshVertex cur_vert = (*vert_it);
            MeshVertex next_vert = (*(vert_it+1));

            prev_path.clear();
            prev_path.push_back(prev_vert);
            prev_path.push_back(cur_vert);
            cur_path.clear();
            cur_path.push_back(cur_vert);
            cur_path.push_back(next_vert);
            reverse(cur_path.begin(), cur_path.end());

            int div_cnt = suggestDivisionCnt(prev_path, cur_path);
            for ( ; div_cnt>=0 ; div_cnt--){
                // do something abount division
            }
        }*/
    }

    qDebug() << "ok till here4";
}
//
//std::vector<std::array<QVector3D, 3>> MeshRepair::fillPath(Path3D path){
//    std::vector<std::array<QVector3D, 3>> result;
//    result.reserve(path.size()*10);
//
//    if (path.size() <3)
//        return result;
//
//    Plane maximal_plane;
//    maximal_plane.push_back(path[0].position);
//    maximal_plane.push_back(path[1].position);
//    maximal_plane.push_back(path[2].position);
//
//    qDebug() << "fill path debug 1";
//    for (MeshVertex mv : path){
//        if (modelcut::isLeftToPlane(maximal_plane, mv.position)){
//            // move plane to normal direction with distance from mv
//            QVector3D plane_normal = QVector3D::normal(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
//            float distance = mv.position.distanceToPlane(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
//            for (int i=0; i<3; i++){
//                maximal_plane[i] += distance*plane_normal;
//            }
//        }
//    }
//    qDebug() << "fill path debug 2";
//
//    // project path to target path
//    Path3D target_path;
//    target_path.reserve(path.size());
//
//    QVector3D plane_normal = QVector3D::normal(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
//
//    qDebug() << "fill path debug 3";
//    for (int i=0; i<path.size(); i++){
//        target_path.push_back(path[i]);
//
//        // project qvector3d to maximal plane
//        float distance = path[i].position.distanceToPlane(maximal_plane[0],maximal_plane[1],maximal_plane[2]);
//        target_path.end().position() += plane_normal * distance;
//    }
//    qDebug() << "fill path debug 4";
//
//    //std::vector<QVector3D> interpolated = interpolate(path, target_path);
//    //qDebug() << "interpolated size : " << interpolated.size();
//    // interpolate between path and target path
//    for (std::array<QVector3D, 3> face : interpolate(path, target_path)){
//        result.push_back(face);
//    }
//
//    qDebug() << "fill path debug 5";
//
//    // fill Hole
//    int half_path_size = path.size()/2;
//
//    // fill hole really
//    for (int i=0; i<half_path_size-1; i++){
//        std::array<QVector3D,3> temp_face;
//        temp_face[0] = path[i].position;
//        temp_face[1] = path[path.size()-i-2].position;
//        temp_face[2] = path[i+1].position;
//        result.push_back(temp_face);
//        temp_face[0] = path[i+1].position;
//        temp_face[1] = path[path.size()-i-2].position;
//        temp_face[2] = path[path.size()-i-3].position;
//        result.push_back(temp_face);
//    }
//    qDebug() << "fill path debug 6";
//
//    if (path.size()%2 != 0){
//        std::array<QVector3D,3> temp_face;
//        temp_face[0] = path[0].position;
//        temp_face[1] = path[path.size()-1].position;
//        temp_face[2] = path[path.size()-2].position;
//        result.push_back(temp_face);
//    }
//    qDebug() << "fill path debug 7";
//
//    return result;
//}


int MeshRepair::suggestDivisionCnt(Path3D e1,Path3D e2){
    QVector3D e1_diff = e1[1].position-e1[0].position;
    QVector3D e2_diff = e2[1].position-e2[0].position;

    float norm_prod = e1_diff.length() * e2_diff.length();
    float dot_prod = QVector3D::dotProduct(e1_diff, e2_diff);

    float cos = dot_prod/norm_prod;


    if (cos >= cos50){
        return 0;
    } else if (cos100<=cos<cos50){
        return 1;
    } else if (cos150<=cos<cos100){
        return 2;
    } else {
        return 3;
    }
    return 4;
}

// detects orientation defects, which induces normal std::vector errors and render errors
void MeshRepair::fixNormalOrientations(Mesh* mesh){

}

// future works
void MeshRepair::removeGaps(Mesh* mesh){

}


/*---------------- helper functions -----------------*/


