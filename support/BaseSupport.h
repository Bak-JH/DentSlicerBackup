#pragma once

class QVector3D;

namespace Hix
{
	namespace Support
	{
		class BaseSupport
		{
		public:
			virtual const QVector3D& getBasePt()const = 0;
		};
	}
}