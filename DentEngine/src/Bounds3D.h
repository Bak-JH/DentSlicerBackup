#pragma once
#include <qvector3d.h>
#include <array>
namespace Hix
{
	namespace Engine3D
	{
		//bounding box in 3D
		class Bounds3D
		{

		public:
			Bounds3D();
			void update(const QVector3D& pt);
			void reset();


			void setXMax(float val);
			void setXMin(float val);
			void setYMax(float val);
			void setYMin(float val);
			void setZMax(float val);
			void setZMin(float val);

			float xMax()const;
			float xMin()const;
			float yMax()const;
			float yMin()const;
			float zMax()const;
			float zMin()const;
			float centreX()const;
			float centreY()const;
			float centreZ()const;
			QVector3D centre()const;
			QVector3D displaceWithin(const Bounds3D& child, QVector3D displacement =  QVector3D())const;
			std::array<float, 6> calculateMaxDisplacement(const Bounds3D & child)const;
			bool contains(const Hix::Engine3D::Bounds3D& other)const;
			//********When centred around origin ********
			Bounds3D(QVector3D lengths);
			void setXLength(float length);
			void setYLength(float length);
			void setZLength(float length);

		private:
			std::array<float, 6> _bound;

		};


	}
}
