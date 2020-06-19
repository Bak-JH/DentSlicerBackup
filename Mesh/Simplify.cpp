#include "Simplify.h"
#include "application/ApplicationManager.h"
#include <Eigen/Core>
#include <igl/remove_duplicate_vertices.h>
#include <igl/circulation.h>
#include <igl/collapse_edge.h>
#include <igl/edge_flaps.h>
#include <igl/shortest_edge_and_midpoint.h>

void Hix::Engine3D::Simplify::to_eigen_mesh(const Mesh* mesh, Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
	V.resize(3 * mesh->getFaces().size(), 3);
	F.resize(mesh->getFaces().size(), 3);
	FaceConstItr faceitr = mesh->getFaces().begin();
	for (unsigned int i = 0; i < mesh->getFaces().size(); ++i) {
		V.block<1, 3>(3 * i + 0, 0) = to_eigen_matrix<double>(faceitr.meshVertices().at(0).localPosition());
		V.block<1, 3>(3 * i + 1, 0) = to_eigen_matrix<double>(faceitr.meshVertices().at(1).localPosition());
		V.block<1, 3>(3 * i + 2, 0) = to_eigen_matrix<double>(faceitr.meshVertices().at(2).localPosition());
		F(i, 0) = int(3 * i + 0);
		F(i, 1) = int(3 * i + 1);
		F(i, 2) = int(3 * i + 2);
		++faceitr;
	}

	Eigen::MatrixXd rV;
	Eigen::MatrixXi rF;
	// We will convert this to a proper 3d mesh with no duplicate points.
	Eigen::VectorXi SVI, SVJ;
	igl::remove_duplicate_vertices(V, F, MESH_EPS, rV, SVI, SVJ, rF);
	V = std::move(rV);
	F = std::move(rF);
}

bool Hix::Engine3D::Simplify::simlify_mesh(Eigen::MatrixXd& OV, Eigen::MatrixXi& OF, int simplify_level)
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
	int num_collapsed;

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
	num_collapsed = 0;

	for (int i = 0; i < simplify_level; ++i)
	{
		/// simplify ///
		if (!Q.empty())
		{
			// collapse edge
			const int max_iter = std::ceil(0.01 * Q.size());
			for (int j = 0; j < max_iter; j++)
			{
				if (!igl::collapse_edge(
					igl::shortest_edge_and_midpoint, OV, OF, E, EMAP, EF, EI, Q, Qit, C))
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
