#include "configuration.h"
//#include "viewer3d.h"

#include <QQmlContext>
#include <QTimer>
#include <Qt3DCore>
SlicingConfiguration* scfg = new SlicingConfiguration();

float SlicingConfiguration::raft_base_radius() const
{
	return support_radius_max* 9.0f;
}

float SlicingConfiguration::raft_offset_radius() const
{
	return raft_base_radius()/20.0f;
}
