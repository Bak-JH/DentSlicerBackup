#include "Labelling.h"
#include "qmlmanager.h"

Hix::Features::Labelling::Labelling(const std::unordered_set<GLModel*>& selectedModels)
	:_targetModels(selectedModels)
{
	if (selectedModels.size() > 1)
	{
		qmlManager->openResultPopUp("", "multiple selecteion is not supported", "");
		return;
	}
	
	_previewModel = new Hix::LabelModel(selectedModels.begin());
}

void Hix::Features::Labelling::setTranslation(QVector3D translation)
{
	if (qmlManager->isActive<Labelling>(qmlManager->currentFeature()))
	{
		if (_previewModel)
			_previewModel->deleteLater();

		_previewModel->generateLabel(translation);
		_previewModel->updateModelMesh();
	}
}

void Hix::Features::Labelling::setText(QString text)
{
	if (_previewModel)
		_previewModel->text = text;
}

void Hix::Features::Labelling::setFontName(QString fontName)
{
	if (_previewModel)
		_previewModel->font.setFamily(fontName);
}

void Hix::Features::Labelling::setFontBold(bool isBold)
{
	if (_previewModel)
		_previewModel->font.setBold(isBold);
}

void Hix::Features::Labelling::setFontSize(int fontSize)
{
	if (_previewModel)
		_previewModel->font.setPointSize(fontSize);
}

void Hix::Features::Labelling::generateLabelMesh()
{
	if (!_previewModel)
	{
		qDebug() << "no labellingTextPreview";
		QMetaObject::invokeMethod(qmlManager->labelPopup, "noModel");
		return;
	}
	

	qmlManager->openProgressPopUp();
	qmlManager->setProgress(0.1f);
	
	for (auto target : _targetModels)
	{
		target->setTargetSelected(false);
		target->setMaterialColor(Hix::Render::Colors::Selected);
		target->updateModelMesh();
	}
	_previewModel = nullptr;

	qmlManager->setProgress(1.0f);
}
