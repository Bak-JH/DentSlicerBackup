#include "Simplify.h"
#include "glmodel.h"
#include "application/ApplicationManager.h"

// Determinant
double det(QMatrix4x4 m, int a11, int a12, int a13,
						 int a21, int a22, int a23,
						 int a31, int a32, int a33)
{
	double det = m(a11 / 3, a11 % 4) * m(a22 / 3, a22 % 4) * m(a33 / 3, a33 % 4) 
				+ m(a13 / 3, a13 % 4) * m(a21 / 3, a21 % 4) * m(a32 / 3, a32 % 4) 
				+ m(a12 / 3, a12 % 4) * m(a23 / 3, a23 % 4) * m(a31 / 3, a31 % 4) 
				- m(a13 / 3, a13 % 4) * m(a22 / 3, a22 % 4) * m(a31 / 3, a31 % 4) 
				- m(a11 / 3, a11 % 4) * m(a23 / 3, a23 % 4) * m(a32 / 3, a32 % 4) 
				- m(a12 / 3, a12 % 4) * m(a21 / 3, a21 % 4) * m(a33 / 3, a33 % 4);
	return det;
}

// Error between vertex and Quadric
double Hix::Engine3D::Simplify::vertex_error(QMatrix4x4 matrix, double x, double y, double z)
{
	return matrix(0, 0) * x * x + 2 * matrix(0, 1) * x * y + 2 * matrix(0, 2) * x * z + 2 * matrix(0, 3) * x 
								+ matrix(1, 1) * y * y     + 2 * matrix(1, 2) * y * z + 2 * matrix(1, 3) * y 
													       + matrix(2, 2) * z * z	  + 2 * matrix(2, 3) * z 
																					  + matrix(3, 3);
}

// Error for one edge
double Hix::Engine3D::Simplify::edge_error(GLModel* model, MeshFace* face, QVector3D& p_result)
{
	Mesh* targetMesh = model->getMeshModd();
	HalfEdge testEdge;
	// find face vertices
	for (auto edge : model->getMesh()->getHalfEdges())
	{
		if (edge.owningFace == face->edge)
		{
			qDebug() << edge.owningFace;
			testEdge = edge;
		}
	}

	// compute interpolated vertex
	auto fromMatrix = QMatrix4x4(model->transform().matrix());
	auto toMatrix = QMatrix4x4(model->transform().matrix());

	fromMatrix.translate(targetMesh->getVertices()[testEdge.from].position);
	toMatrix.translate(targetMesh->getVertices()[testEdge.to].position);
	QMatrix4x4 q = fromMatrix + toMatrix;
	double error = 0;
	double detResult = det(q, 0, 1, 2, 1, 4, 5, 2, 5, 7);
	if (det != 0)
	{

		// q_delta is invertible
		p_result.x = -1 / detResult * (det(q, 1, 2, 3, 4, 5, 6, 5, 7, 8));	// vx = A41/det(q_delta)
		p_result.y = 1 / detResult * (det(q, 0, 2, 3, 1, 5, 6, 2, 7, 8));	// vy = A42/det(q_delta)
		p_result.z = -1 / detResult * (det(q, 0, 1, 3, 1, 4, 6, 2, 5, 8));	// vz = A43/det(q_delta)

		error = vertex_error(q, p_result.x, p_result.y, p_result.z);
	}
	else
	{
		// det = 0 -> try to find best result
		QVector3D p1 = targetMesh->getVertices()[testEdge.from].position;
		QVector3D p2 = targetMesh->getVertices()[testEdge.to].position;
		QVector3D p3 = (p1 + p2) / 2;
		double error1 = vertex_error(q, p1.x, p1.y, p1.z);
		double error2 = vertex_error(q, p2.x, p2.y, p2.z);
		double error3 = vertex_error(q, p3.x, p3.y, p3.z);
		error = std::min(error1, std::min(error2, error3));
		if (error1 == error) p_result = p1;
		if (error2 == error) p_result = p2;
		if (error3 == error) p_result = p3;
	}
	return error;
}