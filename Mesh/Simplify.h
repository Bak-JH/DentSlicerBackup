#pragma once
#include <QMatrix4x4>

class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		struct MeshFace;
		class Mesh;

		namespace Simplify
		{
			double vertex_error(QMatrix4x4 q, double x, double y, double z);
			double edge_error(GLModel* model, MeshFace* face, QVector3D& result);
		}
	}
}