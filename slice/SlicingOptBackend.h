#pragma once

#include <array>
#include <QObject>
class QmlManager;
class SlicingConfiguration;
class SlicingOptBackend : public QObject
{
	Q_OBJECT
public:
	SlicingOptBackend(QmlManager* qmlManager, SlicingConfiguration* config);
	void createSlicingOptControls();
	void updateConfig();
private:
	QmlManager* _qmlManager;
	SlicingConfiguration* _config;
	template<std::size_t arraySizeType>
	void addOptionDialog(QString opName, std::array<std::string_view, arraySizeType> content, int defaultIdx);

};

