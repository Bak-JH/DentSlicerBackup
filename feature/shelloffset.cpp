#include "shelloffset.h"
#include "../qml/components/Inputs.h"
#include "../qml/components/ControlOwner.h"
#include "feature/repair/meshrepair.h"
#include "DentEngine/src/mesh.h"
#include "cut/ZAxialCut.h"
#include "deleteModel.h"
#include "application/ApplicationManager.h"


// offset shell with mm

using namespace Hix::Engine3D;
using namespace Hix::Features;
using namespace Hix::Features::Cut;
const QUrl OFFSET_POPUP_URL = QUrl("qrc:/Qml/FeaturePopup/PopupShellOffset.qml");

Hix::Features::ShellOffsetMode::ShellOffsetMode():_cuttingPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total()), DialogedMode(OFFSET_POPUP_URL)
{
	auto& co = controlOwner();
	co.getControl(_offsetValue, "offsetValue");

	auto selectedModels = Hix::Application::ApplicationManager::getInstance().partManager().selectedModels();
	if (selectedModels.size() == 1)
	{
		_modelBound = _subject->recursiveAabb();
		_cuttingPlane.enablePlane(true);
		_cuttingPlane.transform().setTranslation(QVector3D(0, 0, _modelBound.zMin() + 1 * _modelBound.lengthZ() / 1.8));
		_subject = *selectedModels.begin();
	}
	else
	{
		throw std::runtime_error("A single model must be selected");
	}
}

Hix::Features::ShellOffsetMode::~ShellOffsetMode()
{
}

void Hix::Features::ShellOffsetMode::getSliderSignal(double value)
{
	float zlength = _modelBound.lengthZ();
	_cuttingPlane.transform().setTranslation(QVector3D(0, 0, _modelBound.zMin() + value * zlength / 1.8));
}

void Hix::Features::ShellOffsetMode::applyButtonClicked()
{

}





Hix::Features::ShellOffset::ShellOffset(GLModel* target, float offset, float zPlane) : _target(target), _offset(offset), _zPlane(zPlane)
{}

Hix::Features::ShellOffset::~ShellOffset()
{
}


void Hix::Features::ShellOffset::runImpl()
{
	addFeature(new HollowMesh(_target, _offset));
	addFeature(new ZAxialCut(_target, _zPlane, Hix::Features::Cut::KeepTop));
	FeatureContainer::runImpl();
}

Hix::Features::HollowMesh::HollowMesh(GLModel* target, float offset) : _target(target), _offset(offset)
{}

Hix::Features::HollowMesh::~HollowMesh()
{
}

void Hix::Features::HollowMesh::undoImpl()
{
	auto tmp = _target->getMeshModd();
	_target->setMesh(_prevMesh.release());
	_prevMesh.reset(tmp);
}

void Hix::Features::HollowMesh::redoImpl()
{
	auto tmp = _target->getMeshModd();
	_target->setMesh(_prevMesh.release());
	_prevMesh.reset(tmp);
}

void Hix::Features::HollowMesh::runImpl()
{
	_prevMesh.reset(_target->getMeshModd());
	auto hollowMesh = new Mesh(*_prevMesh.get());
	Mesh offsetMesh(*_target->getMeshModd());
	offsetMesh.vertexOffset(-_offset);
	offsetMesh.reverseFaces();
	*hollowMesh += offsetMesh;
	_target->setMesh(hollowMesh);

}
