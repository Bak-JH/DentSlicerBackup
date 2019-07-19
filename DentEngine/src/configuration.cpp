#include "configuration.h"
//#include "viewer3d.h"

#include <QQmlContext>
#include <QTimer>
#include <Qt3DCore>
SlicingConfiguration* scfg = new SlicingConfiguration();

void SlicingConfiguration::set(const QVariantMap& config)
{
	for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter) {
		qDebug() << iter.key() << iter.value().toString();
		if (!strcmp(iter.key().toStdString().c_str(), "support_type")) {
			if (iter.value().toString() == "none") {
				scfg->support_type = SlicingConfiguration::SupportType::None;
			}
			else if (iter.value().toString() == "general") {
				scfg->support_type = SlicingConfiguration::SupportType::General;
			}
			else  if (iter.value().toString() == "k-branch") {
				scfg->support_type = SlicingConfiguration::SupportType::KBranch;
			}
		}
		else if (!strcmp(iter.key().toStdString().c_str(), "infill_type")) {
			if (iter.value().toString() == "none") {
				scfg->infill_type = SlicingConfiguration::InFillType::None;
			}
			else if (iter.value().toString() == "general") {
				scfg->infill_type = SlicingConfiguration::InFillType::General;
			}
			else  if (iter.value().toString() == "full") {
				scfg->infill_type = SlicingConfiguration::InFillType::Full;
			}
		}
		else if (!strcmp(iter.key().toStdString().c_str(), "raft_type")) {
			if (iter.value().toString() == "none") {
				scfg->raft_type = SlicingConfiguration::RaftType::None;
			}
			else if (iter.value().toString() == "general") {
				scfg->raft_type = SlicingConfiguration::RaftType::General;
			}
		}
		else if (!strcmp(iter.key().toStdString().c_str(), "layer_height")) {
			scfg->layer_height = iter.value().toFloat();
			//(*scfg)[iter.key().toStdString().c_str()] = 2;//iter.value().toString();
		}
		else if (!strcmp(iter.key().toStdString().c_str(), "resolution")) {
			//(*scfg)[iter.key().toStdString().c_str()] = 2;//iter.value().toString();
		}
		else if (!strcmp(iter.key().toStdString().c_str(), "resin_type")) {
			if (iter.value().toString() == "Temporary") {
				scfg->resin_type = TEMPORARY_RESIN;
				scfg->contraction_ratio = TEMPORARY_CONTRACTION_RATIO;
			}
			else if (iter.value().toString() == "Clear") {
				scfg->resin_type = CLEAR_RESIN;
				scfg->contraction_ratio = CLEAR_CONTRACTION_RATIO;
			}
			else {
				scfg->resin_type = CASTABLE_RESIN;
				scfg->contraction_ratio = CASTABLE_CONTRACTION_RATIO;
			}
		}
	}
}
