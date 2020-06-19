#include "Simplify.h"
#include "application/ApplicationManager.h"
#include <Eigen/Core>
#include <igl/remove_duplicate_vertices.h>
#include <igl/circulation.h>
#include <igl/collapse_edge.h>
#include <igl/edge_flaps.h>
#include <igl/shortest_edge_and_midpoint.h>

const auto AABBCost = [](
	const Eigen::MatrixXd& V,/*V*/
	const Eigen::MatrixXi&,/*F*/
	const Eigen::MatrixXi& E,/*E*/
	const Eigen::VectorXi&,/*EMAP*/
	const Eigen::MatrixXi&,/*EF*/
	const Eigen::MatrixXi&,/*EI*/
	const std::set<std::pair<double, int> >&,/*Q*/
	const std::vector<std::set<std::pair<double, int> >::iterator >&,/*Qit*/
	const Eigen::MatrixXd&,/*C*/
	const int e) -> bool {
		auto minLength = std::min(Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeX(), 
									Hix::Application::ApplicationManager::getInstance().settings().printerSetting.pixelSizeY());
		auto voxel = pow(minLength / 2, 3);
		auto volume = Hix::Bounds3D::Bounds3D();
		volume.update(QVector3D(V.row(E(e, 0)).x(), V.row(E(e, 0)).y(), V.row(E(e, 0)).z()));
		volume.update(QVector3D(V.row(E(e, 1)).x(), V.row(E(e, 1)).y(), V.row(E(e, 1)).z()));

		if (volume.volume() < voxel)
			return true;
		return false;
};

void Hix::Engine3D::Simplify::to_eigen_mesh(const Mesh* mesh, Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
	V.resize(mesh->getVertices().size(), 3);
	F.resize(mesh->getFaces().size(), 3);
	VertexConstItr vertexItr = mesh->getVertices().begin();
	for (unsigned int i = 0; i < mesh->getVertices().size(); ++i)
	{
		V.block<1, 3>(i, 0) = to_eigen_matrix<double>(vertexItr.localPosition());
		++vertexItr;
	}

	FaceConstItr faceitr = mesh->getFaces().begin();
	for (unsigned int i = 0; i < mesh->getFaces().size(); ++i) 
	{
		auto MV = faceitr.getVerticeIndices();
		F(i, 0) = MV[0];
		F(i, 1) = MV[1];
		F(i, 2) = MV[2];
		++faceitr;
	}
}

bool Hix::Engine3D::Simplify::simlify_mesh(Eigen::MatrixXd& OV, Eigen::MatrixXi& OF)
{
	/// properties ///
	bool something_collapsed = false;
	// Prepare array-based edge data structures and priority queue
	Eigen::VectorXi EMAP;
	Eigen::MatrixXi E, EF, EI;
	typedef std::set<std::pair<double, int> > PriorityQueue;
	PriorityQueue Q;
	std::vector<PriorityQueue::iterator > Qit;
	// If an edge were collapsed, we'd collapse it to these points:
	Eigen::MatrixXd C;
	int num_collapsed = 1;

	igl::edge_flaps(OF, E, EMAP, EF, EI);
	Qit.resize(E.rows());
	C.resize(E.rows(), OV.cols());
	Q.clear();

	for (int e = 0; e < E.rows(); e++)
	{
		double cost = e;
		Eigen::RowVectorXd p(1, 3);
		igl::shortest_edge_and_midpoint(e, OV, OF, E, EMAP, EF, EI, cost, p);
		C.row(e) = p;
		Qit[e] = Q.insert(std::pair<double, int>(cost, e)).first;
	}
	

	while(num_collapsed != 0)
	{
		num_collapsed = 0;
		/// simplify ///
		if (!Q.empty())
		{
			// collapse edge
			const int max_iter = std::ceil(0.01 * Q.size());
			for (int j = 0; j < max_iter; j++)
			{
				if (!igl::collapse_edge(
					igl::shortest_edge_and_midpoint, AABBCost, OV, OF, E, EMAP, EF, EI, Q, Qit, C))
				{
					break;
				}
				something_collapsed = true;
				num_collapsed++;
			}
		}
		
		qDebug() << "collapsed: " << num_collapsed;
	}

	return something_collapsed;
}

Hix::Engine3D::Mesh* Hix::Engine3D::Simplify::to_hix_mesh(Hix::Engine3D::Mesh* mesh, Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
	auto resultMesh = new Hix::Engine3D::Mesh(*mesh);
	resultMesh->clear();

	for (Eigen::Index i = 0; i < F.rows(); ++i) {
		resultMesh->addFace(QVector3D(V.row(F.row(i)(0)).x(), V.row(F.row(i)(0)).y(), V.row(F.row(i)(0)).z()),
							QVector3D(V.row(F.row(i)(1)).x(), V.row(F.row(i)(1)).y(), V.row(F.row(i)(1)).z()),
							QVector3D(V.row(F.row(i)(2)).x(), V.row(F.row(i)(2)).y(), V.row(F.row(i)(2)).z()));
	}

	return resultMesh;
}
