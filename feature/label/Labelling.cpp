#include "Labelling.h"
#include "qmlmanager.h"

Hix::Features::Labelling::Labelling()
{
}



Hix::Features::Labelling::~Labelling()
{
	delete _previewModel;
}

void Hix::Features::Labelling::faceSelected(GLModel* selected, const Hix::Engine3D::FaceConstItr& selectedFace, const Hix::Input::MouseEventData& mouse, const Qt3DRender::QRayCasterHit& hit)
{
	_targetModel = selected;
	updateLabelMesh(hit.localIntersection(), selectedFace.localFn());

}

void Hix::Features::Labelling::updateLabelMesh(const QVector3D translation, const QVector3D normal)
{
	if (qmlManager->isActive<Labelling>())
	{
		if (!_previewModel)
		{
			_previewModel = new Hix::LabelModel(_targetModel);
		}
		_previewModel->generateLabelMesh(translation, normal, _text, _font);
	}
}

void Hix::Features::Labelling::setText(const QString text)
{
	_text = text;
}

void Hix::Features::Labelling::setFontName(const QString fontName)
{
	_font.setFamily(fontName);
}

void Hix::Features::Labelling::setFontBold(const bool isBold)
{
	_font.setBold(isBold);
}

void Hix::Features::Labelling::setFontSize(const int fontSize)
{
	_font.setPointSize(fontSize);
}

void Hix::Features::Labelling::generateLabelMesh()
{
	if (!_previewModel)
	{
		qDebug() << "no labellingTextPreview";
		QMetaObject::invokeMethod(qmlManager->labelPopup, "noModel");
		return;
	}
	_targetModel->setMaterialColor(Hix::Render::Colors::Selected);
	_targetModel->updateModelMesh();
	_previewModel = nullptr;

}
