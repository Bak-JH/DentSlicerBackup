#include "Debug.h"
#include <stdexcept>


#ifdef _DEBUG
void Hix::Debug::throwDontUse()
{
	throw std::runtime_error("Don't use this!");
}
#endif

