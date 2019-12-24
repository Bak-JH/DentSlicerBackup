#pragma once

// Qt include file //
#include <QObject>

// Hix include file //
#include "common/Debug.h"

class DialogManager : public QObject
{
	Q_OBJECT
public:
	void openDialog();
};