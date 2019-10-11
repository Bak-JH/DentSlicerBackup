#ifndef ARRANGE_H
#define ARRANGE_H
#include <vector>
#include <algorithm>
#include <QVector3D>
#include <Qt3DCore>
#include <QHash>
#include <QDebug>
#include <math.h>
//#include "./glmodel.h"
#include "DentEngine/src/polyclipping/clipper/clipper.hpp"
#include "convex_hull.h"
#if  defined(QT_DEBUG) || defined(_DEBUG)
#define _DEBUG_AUTO_ARRANGE
#endif
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
		struct HalfEdge;
		struct MeshVertex;
		struct MeshFace;

	}
}


typedef std::pair<ClipperLib::IntPoint, float> XYArrangement;
typedef std::vector<ClipperLib::IntPoint> Vecs;

class autoarrange : public QObject {
    Q_OBJECT
public:
    autoarrange();

    std::vector<XYArrangement> arngMeshes(std::vector<const Hix::Engine3D::Mesh*>& meshes, std::vector<Qt3DCore::QTransform> m_transform_set);
  
};



#endif // ARRANGE_H
