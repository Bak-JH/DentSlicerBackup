#pragma once
#include <QDebug>


#if defined(_DEBUG) || defined(QT_DEBUG )
//multiple debug labels FFS
#define _DEBUG

#endif


#ifdef _DEBUG
#include "debugging/DebugRenderObject.h"

#define DONT_USE Hix::Debug::throwDontUse();
namespace Hix
{
	namespace Debug
	{
		void throwDontUse();
	}

}
#else
#define DONT_USE
#endif