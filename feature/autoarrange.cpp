#include "autoarrange.h"
#include <cstdlib>
#include <ctime>
#define ANGLE_UNIT 270
#define STAGE_WIDTH 95000
#define STAGE_HEIGHT 75000
#define OFFSET 2000
#include <qmlmanager.h>
#include <exception>
#include <unordered_map>
#include "DentEngine/src/mesh.h"
#include "DentEngine/src/configuration.h"
#include "DentEngine/src/slicer.h"
using namespace ClipperLib;
using namespace Hix::Engine3D;
autoarrange::autoarrange()
{
}


std::vector<XYArrangement> autoarrange::arngMeshes(std::vector<const Mesh*>& meshes, std::vector<const Qt3DCore::QTransform*> m_transform_set){
  
    return std::vector<XYArrangement>();
}
