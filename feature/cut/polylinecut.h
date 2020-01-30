#ifndef MODELCUT_H
#define MODELCUT_H
#include <QVector3D>
#include "feature/interfaces/FlushSupport.h"

class GLModel;
class CorkTriMesh;
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
		class Bounds3D;
	}
	namespace Render
	{
		class SceneEntity;
	}

	namespace Features
	{
		namespace Cut
		{
			class PolylineCut : public FeatureContainerFlushSupport
			{
			public:
				PolylineCut(GLModel* origModel, std::vector<QVector3D> cuttingPoints);
			protected:
				void runImpl()override;
			private:
				void generateCuttingWalls(const std::vector<QVector3D>& polyline, const Hix::Engine3D::Bounds3D& cutBound, Hix::Engine3D::Mesh& out);
				void cutCSG(const QString& subjectName, Hix::Render::SceneEntity* subject, const CorkTriMesh& subtract);
				GLModel* _target;
				std::vector<QVector3D> _cuttingPoints;
			};


		}
	}
}
#endif // MODELCUT_H
