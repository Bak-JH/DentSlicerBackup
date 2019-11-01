#ifndef MODELCUT_H
#define MODELCUT_H
#include <QVector3D>

class GLModel;
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}

	namespace Features
	{
		namespace Cut
		{
			class PolylineCut
			{
			public:
				PolylineCut(GLModel* origModel, std::vector<QVector3D> cuttingPoints, bool fill);
			private:
				void cutAway();

				std::vector<QVector3D> _cuttingPoints;
				bool _fill;
				const Engine3D::Mesh* _origMesh;
				Engine3D::Mesh* _leftMesh;
				Engine3D::Mesh* _rightMesh;
			};


		}
	}
}
#endif // MODELCUT_H
