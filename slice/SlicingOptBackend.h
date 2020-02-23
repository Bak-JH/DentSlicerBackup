#pragma once

#include <array>
#include <QObject>
#include <qvariant.h>
class QmlManager;
class SlicingConfiguration;
class SlicingOptBackend : public QObject
{
	Q_OBJECT
public:
	SlicingOptBackend(SlicingConfiguration* config);
	void createSlicingOptControls();
public slots:
	void onOptionChanged(QString, QVariant);
private:
	SlicingConfiguration* _config;
	template<std::size_t arraySizeType>
	void addOptionDialogCombo(QString opName, std::array<std::string_view, arraySizeType> content, int defaultVal);
	void addOptionDialogPercentage(QString opName, int defaultVal);

};
