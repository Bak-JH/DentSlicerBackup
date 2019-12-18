#include "configuration.h"
//#include "viewer3d.h"

#include <QQmlContext>
#include <QTimer>
#include <Qt3DCore>
SlicingConfiguration* scfg = new SlicingConfiguration();

SlicingConfiguration::SlicingConfiguration()
{
}

float SlicingConfiguration::raft_base_radius() const
{
	return support_radius_max* 3.0f;
}

