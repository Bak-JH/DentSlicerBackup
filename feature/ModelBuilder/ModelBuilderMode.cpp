#include "ModelBuilderMode.h"
#include <fstream>
#include <unordered_set>
#include <boost/algorithm/string.hpp>
#include <QFileDialog>
#include "../../Qml/components/Inputs.h"
#include "../../Qml/components/Buttons.h"

#include "../../glmodel.h"
#include "../addModel.h"
#include "../Shapes2D.h"
#include "../Extrude.h"
#include "../cdt/HixCDT.h"
#include "TwoManifoldBuilder.h"
#include "../repair/meshrepair.h"
#include "application/ApplicationManager.h"
#include "../widget/RotateWidget.h"
#include "../../DentEngine/src/mesh.h"


using namespace Hix;
using namespace Hix::Features;
using namespace Hix::Features::CSG;
using namespace Hix::Engine3D;
constexpr float ZMARGIN = 3;

const static std::unordered_map<MBEditMode, std::string> __modeTexts = { 
	{MBEditMode::Rotation, "Rotate model to correct orientation, default given orientation is auto-calculated."},
	{MBEditMode::FaceSelection, "Remove degenerative faces using polygonal selection."},
	{MBEditMode::PlaneCut, "Adjust horizontal cutting plane, default given height is auto-calculated."} 
};

const static std::unordered_map<MBEditMode, std::string> __modeNames = {
	{MBEditMode::Rotation, "Step1: Model Rotation"},
	{MBEditMode::FaceSelection, "Step2: Face Cleaning"},
	{MBEditMode::PlaneCut, "Step3: Plane Cut"}
};


const static std::unordered_map<SelectionState, std::string> __selectionTxt = {
	{SelectionState::Selecting, "Select faces"},
	{SelectionState::Selected, "Erase selected faces"}
};

const QUrl MB_POPUP = QUrl("qrc:/Qml/FeaturePopup/PopupModelBuild.qml");




void Hix::Features::ModelBuilderMode::featureStarted()
{
}

void Hix::Features::ModelBuilderMode::featureEnded()
{

	_model->updateRecursiveAabb();
	updatePosition();
}

QVector3D Hix::Features::ModelBuilderMode::getWidgetPosition()
{
	return 	_model->recursiveAabb().centre();
}

void Hix::Features::ModelBuilderMode::updatePosition()
{
	if (_model)
	{
		//auto& planeTrs = _selectionPlane.transform();
		//Hix::Features::WidgetMode::updateTransform(planeTrs);
		if (_edit == MBEditMode::FaceSelection)
		{
			//model root transform
			auto sysTransform = Hix::Application::ApplicationManager::getInstance().sceneManager().systemTransform();
			//camera space
			auto cam = Hix::Application::ApplicationManager::getInstance().sceneManager().camera();
			auto camNormal = -cam->viewVector();
			auto camPt = cam->position() + cam->viewVector().normalized()* 15.0f;


			//converted model space
			auto position = QVector3D(sysTransform->matrix().inverted() * QVector4D(cam->position(),1));
			auto normal = QVector3D(sysTransform->matrix().inverted() * QVector4D(camNormal,0));
			auto pt = QVector3D(sysTransform->matrix().inverted() * QVector4D(camPt,1));
			normal.normalize();
			//qDebug() << normal << pt;
			_selectionPlane.setPointNormal({ pt, normal });

			_selectionPlane.clearPt();
			setSelectionState(SelectionState::Selecting);
			//qDebug() << "cam pos orig" << cam->position();
			//qDebug() << "cam pos" << position;
			//qDebug() << normal << pt;

			//auto normalLength = normal.length();
			//normal.normalize();
			//qDebug() << normal << normalLength;
			////auto pointOnPlane = _widget.transform()->translation();
			//auto pointOnPlane = sysTransform->translation() + (normal * (normalLength * 0.8));
			//_selectionPlane.setPointNormal({ pointOnPlane, normal });
			//setSelectionState(SelectionState::Selecting);
		}
		else
		{
			Hix::Features::WidgetMode::updatePosition();
		}
	}
}

void Hix::Features::ModelBuilderMode::setMode(MBEditMode mode)
{
	if (mode != _edit)
	{
		_edit = mode;
		_widget.setVisible(false);
		slider().setVisible(false);
		_selectionPlane.enablePlane(false);
		setModeTxts(_edit);
		setSelectionState(SelectionState::None);
		//_button0->setEnabled(false);
		//_button0->setVisible(false);
		//_button1->setEnabled(false);
		//_button1->setVisible(false);
		_topPlane.setEnabled(false);
		_bottPlane.setEnabled(false);
		_button0->setProperty("visible", false);
		_button1->setProperty("visible", false);
		//_selectionPlane.setParent((Qt3DCore::QNode*)nullptr);
		switch (_edit)
		{
		case Hix::Features::MBEditMode::Rotation:
		{
			_widget.setVisible(true);
			updatePosition();
		}
		break;
		case Hix::Features::MBEditMode::FaceSelection:
		{
			_selectionPlane.enablePlane(true);
			//_button0->setEnabled(true);
			//_button0->setVisible(true);
			//_button1->setEnabled(true);
			//_button1->setVisible(true);
			_button0->setProperty("visible", true);
			_button1->setProperty("visible", true);

			//need this to reduce float error
			_model->flushTransform();
			_faceSelector.emplace(*_model.get());
			updatePosition();

		}
		break;
		case Hix::Features::MBEditMode::PlaneCut:
		{
			slider().setVisible(true);
			_topPlane.setEnabled(true);
			_bottPlane.setEnabled(true);
		}
		break;
		default:
			break;
		}
	}
}

void Hix::Features::ModelBuilderMode::setModeTxts(MBEditMode mode)
{
	auto modeName = QString::fromStdString(__modeNames.at(mode));
	auto modeTxt = QString::fromStdString(__modeTexts.at(mode));

	QMetaObject::invokeMethod(_popup.get(), "setModeText", 
		Q_ARG(QVariant, modeName), Q_ARG(QVariant, modeTxt));
}

void Hix::Features::ModelBuilderMode::setSelectionState(SelectionState state)
{
	if (state != _selectionState)
	{
		_selectionState = state;
		switch (_selectionState)
		{
		case Hix::Features::SelectionState::Selecting:
		{
			_selectionPlane.clearPt();
			_selectedFaces.clear();
			_model->unselectMeshFaces();
		}
			break;
		case Hix::Features::SelectionState::Selected:
		{
		}
			break;
		default:
			break;
		}
		colorFaces(_model.get(), _selectedFaces);
		_button0->setProperty("buttonText", QString::fromStdString(__selectionTxt.at(_selectionState)));

	}

}

bool Hix::Features::ModelBuilderMode::select()
{
	auto contour = _selectionPlane.contour();
	if (!contour.empty())
	{
		auto selectedVtcs = _faceSelector->doSelection(_selectionPlane.pointNormal(), contour);
		auto fEnd = _model->getMesh()->getFaces().cend();
		auto itr = _model->getMesh()->getFaces().cbegin();
		for (; itr != fEnd; ++itr)
		{
			auto mv = itr.meshVertices();
			size_t found = 0;
			for (auto& v : mv)
			{
				if (selectedVtcs.find(v) != selectedVtcs.cend())
				{
					++found;
				}
			}
			if (found == 3)
				_selectedFaces.insert(itr);
		}
	}
	return !_selectedFaces.empty();
}

void Hix::Features::ModelBuilderMode::erase()
{
	_model->getMeshModd()->removeFaces(_selectedFaces);
}



Hix::Features::ModelBuilderMode::ModelBuilderMode():
	DialogedMode(MB_POPUP), RangeSliderMode(0, 1),
	_topPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), true),
	_bottPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), true),
	_selectionPlane(Hix::Application::ApplicationManager::getInstance().sceneManager().total(), 100000, 100000, QColor(140,140,140), 0.0f),
	_edit(MBEditMode::PlaneCut)
{
	auto& co = controlOwner();
	co.getControl(_prevButton, "prevButton");
	co.getControl(_nextButton, "nextButton");
	co.getControl(_button0, "button0");
	co.getControl(_button1, "button1");
	_button1->setProperty("buttonText", "Undo selection");


	QObject::connect(_button0, &Hix::QML::Controls::Button::clicked, [this]() {
		switch (_selectionState)
		{
		case Hix::Features::SelectionState::Selecting:
		{
			if (select())
			{
				//if selection suceeds
				setSelectionState(Hix::Features::SelectionState::Selected);
			}
		}
		break;
		case Hix::Features::SelectionState::Selected:
		{
			erase();
			setSelectionState(Hix::Features::SelectionState::Selecting);

		}
		break;
		}
		});

	QObject::connect(_button1, &Hix::QML::Controls::Button::clicked, [this]() {
		setSelectionState(SelectionState::Selecting);
		});

	QObject::connect(_prevButton, &Hix::QML::Controls::Button::clicked, [this]() {
		auto newIdx = magic_enum::enum_index(_edit).value() - 1;
		if (newIdx == 0)
		{
			//prev needs to be disabled as this is the last state
			_prevButton->setEnabled(false);
			_prevButton->setVisible(false);

		}
		//always enable next button since prev happend
		_nextButton->setEnabled(true);
		_nextButton->setVisible(true);
		setMode(magic_enum::enum_value<MBEditMode>(newIdx));
		});
	QObject::connect(_nextButton, &Hix::QML::Controls::Button::clicked, [this]() {
		constexpr auto lastEnumIdx = magic_enum::enum_count<MBEditMode>() - 1;
		auto newIdx = magic_enum::enum_index(_edit).value() + 1;
		if (newIdx == lastEnumIdx)
		{
			_nextButton->setEnabled(false);
			_nextButton->setVisible(false);

		}
		_prevButton->setEnabled(true);
		_prevButton->setVisible(true);
		setMode(magic_enum::enum_value<MBEditMode>(newIdx));
		});
	//initial state
	_prevButton->setEnabled(false);
	_prevButton->setVisible(false);

	setMode(magic_enum::enum_value<MBEditMode>(0));

	auto fileUrl = QFileDialog::getOpenFileUrl(nullptr, "Select scanned surface file", QUrl(), "3D Model file (*.stl)");
	auto fileName = fileUrl.fileName();
	if (fileName.isEmpty())
	{
		scheduleForDelete();
		return;
	}
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(std::make_unique<MBPrep>(this, fileUrl));


	QObject::connect(&slider(), &Hix::QML::RangeSlideBarShell::lowerValueChanged, [this]() {
		_bottPlane.transform().setMatrix(QMatrix4x4());
		_bottPlane.transform().setTranslation(QVector3D(0, 0, slider().lowerValue()));
	});

	QObject::connect(&slider(), &Hix::QML::RangeSlideBarShell::upperValueChanged, [this]() {
		_topPlane.transform().setMatrix(QMatrix4x4());
		_topPlane.transform().setTranslation(QVector3D(0, 0, slider().upperValue()));
		});

}

Hix::Features::ModelBuilderMode::~ModelBuilderMode()
{
}

void Hix::Features::ModelBuilderMode::build()
{
	auto cuttingPlane = _topPlane.transform().translation().z();
	auto bottomPlane = _bottPlane.transform().translation().z();
	//auto aabb0 = _model->aabb();
	_model->flushTransform();
	//auto aabb1 = _model->aabb();
	TwoManifoldBuilder* builder = new TwoManifoldBuilder(*_model->getMeshModd(), _model->modelName(), cuttingPlane, bottomPlane);
	Hix::Application::ApplicationManager::getInstance().taskManager().enqueTask(builder);
	_model->setMesh(nullptr);
}

void Hix::Features::ModelBuilderMode::applyButtonClicked()
{
	build();
}

Hix::Features::MBPrep::MBPrep(ModelBuilderMode* mode, QUrl fileUrl): _mode(mode), _fileUrl(fileUrl)
{
}

Hix::Features::MBPrep::~MBPrep()
{
}

void Hix::Features::MBPrep::run()
{
	auto fileName = _fileUrl.fileName();
	std::filesystem::path filePath(_fileUrl.toLocalFile().toStdWString());
	std::fstream file(filePath);
	auto mesh = new Mesh();
	if (boost::iequals(filePath.extension().string(), ".stl")) {

		if (!FileLoader::loadMeshSTL(mesh, file))
		{
			std::fstream fileBinary(filePath, std::ios_base::in | std::ios_base::binary);
			FileLoader::loadMeshSTL_binary(mesh, fileBinary);
		}
	}
	else if (boost::iequals(filePath.extension().string(), ".obj")) {
		FileLoader::loadMeshOBJ(mesh, file);
	}
	fileName = GLModel::filenameToModelName(fileName.toStdString());
	mesh->centerMesh();
	postUIthread([this, &mesh, &fileName]() {
		_mode->_model.reset(new GLModel(Hix::Application::ApplicationManager::getInstance().partManager().modelRoot(), mesh, fileName, nullptr));
		auto centredBound = _mode->_model->recursiveAabb().centred();
		_mode->_model->moveModel(centredBound.centre());
		//auto bound = origBound.centred();
		//bound.set ZMax(0.0f);
		//bound.setZMin(0.0f);
		//_to = -origBound.centre();

	});
	float cutPlane, botPlane;
	QQuaternion rotation;
	guessOrientation(*_mode->_model->getMeshModd(), cutPlane, botPlane, rotation);
	postUIthread([this, &cutPlane, &botPlane, &rotation]() {
		_mode->_model->transform().setRotation(rotation);
		_mode->_model->updateRecursiveAabb();
		auto zTranslate = _mode->_model->setZToBed();

		_mode->_model->moveModel(QVector3D(0, 0, ZMARGIN));
		zTranslate += ZMARGIN;
		_mode->_model->updateRecursiveAabb();
		_mode->initPPShader(*_mode->_model.get());
		auto rAABB = _mode->_model->recursiveAabb();
		auto modelMaxRadius = rAABB.bbMaxRadius();
		auto modelCentre = rAABB.centre();
		_mode->slider().setMin(modelCentre.z() -modelMaxRadius);
		_mode->slider().setMax(modelCentre.z() + modelMaxRadius);
		auto upSliderVal = cutPlane + zTranslate;
		auto lowSliderVal = botPlane + zTranslate;
		
		_mode->slider().setUpperValue(upSliderVal);
		_mode->slider().setLowerValue(lowSliderVal);
		_mode->_topPlane.transform().setTranslation(QVector3D(0, 0, upSliderVal));
		_mode->_bottPlane.transform().setTranslation(QVector3D(0, 0, lowSliderVal));
		std::unordered_set<GLModel*> models{ _mode->_model.get() };
		//widget stuff
		_mode->_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(1, 0, 0), &_mode->_widget, models));
		_mode->_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 1, 0), &_mode->_widget, models));
		_mode->_widget.addWidget(std::make_unique<Hix::UI::RotateWidget>(QVector3D(0, 0, 1), &_mode->_widget, models));

		_mode->updatePosition();
		_mode->setMode(Hix::Features::MBEditMode::Rotation);

		});
}

Hix::Features::FaceSelector::FaceSelector(const Hix::Render::SceneEntity& subject): _subjectOrig(*subject.getMesh()), _subject(toCorkMesh(*subject.getMesh()))
{
}

std::unordered_set<VertexConstItr> Hix::Features::FaceSelector::doSelection(const Hix::Plane3D::PDPlane& selectionPlane, const std::vector<QVector3D>& polyline)
{
	Mesh selectionMesh;

	//polyline must be 2d points, based on coordinate system of the plane, z = 0
	QVector3D planeFar = selectionPlane.normal * -2000.0f + selectionPlane.point;
	std::vector<QVector3D> path;
	path.reserve(2);
	path.emplace_back(selectionPlane.point);
	path.emplace_back(planeFar);
	auto jointDir = Hix::Features::Extrusion::interpolatedJointNormals(path);
	auto jointContours = Hix::Features::Extrusion::extrudeAlongPath(&selectionMesh, QVector3D(0, 0, 1), polyline, path, jointDir);
	//generate caps
	if (jointContours.front().size() > 1)
	{
		Hix::CDT::MeshCDT cdt(&selectionMesh, true);
		cdt.insertSolidContourZAxis(jointContours.front());
		cdt.triangulateAndAppend();
	}

	if (jointContours.back().size() > 1)
	{
		Hix::CDT::MeshCDT cdt(&selectionMesh, false);
		cdt.insertSolidContourZAxis(jointContours.back());
		cdt.triangulateAndAppend();
	}
	//do transformation from camera space to model space


	auto selectionCorkMesh = toCorkMesh(selectionMesh);

	//do comparison
	CorkTriMesh output;
	computeIntersection(selectionCorkMesh, _subject, &output);
	freeCorkTriMesh(&output);
	//we check which vertex is maintained in union and compare with original mesh. therefore float error is detrimental here.
	auto unionResult = toHixMesh(output);
	std::unordered_set<VertexConstItr> selectedVtcs;
	auto vEnd = unionResult->getVertices().cend();
	for (auto vItr = unionResult->getVertices().cbegin(); vItr != vEnd; ++vItr)
	{
		auto found = _subjectOrig.getVtxAtLocalPos(vItr.localPosition());
		if (found.initialized())
		{
			selectedVtcs.insert(found);
		}
	}
	return selectedVtcs;
}
