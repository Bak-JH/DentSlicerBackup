#include "ModelAttachedSupport.h"
#include "../glmodel.h"
#include <variant>
using namespace Hix::OverhangDetect;
const GLModel& Hix::Support::ModelAttachedSupport::getAttachedModel()
{
	const Hix::Engine3D::Mesh* mesh = getOverhang().owner();
	return *dynamic_cast<const GLModel*>(mesh->entity());
}
