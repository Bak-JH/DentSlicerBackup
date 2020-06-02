#pragma once

class QOpenGLFunctions;
class QOpenGLExtraFunctions;
namespace Hix
{
	namespace Rendering
	{
		class GLContextFunctionObj
		{


		private:
			QOpenGLFunctions _glFn;
			QOpenGLExtraFunctions _glExt;
		};


	}
}
