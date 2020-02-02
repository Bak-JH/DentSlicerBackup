#include "DialogedMode.h"
using namespace Hix::Features;

Hix::Features::DialogedMode::DialogedMode(const QUrl& dialogUrl):_dialog(this, dialogUrl)
{
}

Hix::Features::DialogedMode::~DialogedMode()
{
}
