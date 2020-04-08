#include "../application/ApplicationManager.h"

#include "sendError.h"

using namespace Hix;
using namespace Hix::Render;
using namespace Hix::Features;
using namespace Hix::Engine3D;




Hix::Features::SendError::SendError(const std::string& title, const std::string& details)
{

}

Hix::Features::SendError::~SendError()
{
}

void Hix::Features::SendError::run()
{
	Sleep(3000);
	postUIthread([]() {
		auto& progressManager = Hix::Application::ApplicationManager::getInstance().taskManager().progressManager();
		progressManager.deletePopup();
		});

}
