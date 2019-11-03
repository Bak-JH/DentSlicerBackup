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
				PolylineCut(GLModel* origModel, std::vector<QVector3D> cuttingPoints);
			private:
				//void cutAway();

				const Engine3D::Mesh* _origMesh;

			};


		}
	}
}
#endif // MODELCUT_H
