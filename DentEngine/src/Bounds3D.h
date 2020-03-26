#pragma once
#include <qvector3d.h>
#include <array>
#include <unordered_set>
namespace Hix
{
	namespace Render
	{
		class SceneEntity;
	}
	namespace Engine3D
	{
		class Mesh;
		//bounding box in 3D
		class Bounds3D
		{

		public:
			Bounds3D();
			Bounds3D(const Hix::Render::SceneEntity& bounded);
			template<typename ItrType>
			Bounds3D(ItrType itr, ItrType end) : Bounds3D()
			{
				for (; itr != end; ++itr)
				{
					update(*itr);
				}
			}
			void update(const QVector3D& pt);
			void localBoundUpdate(const Hix::Engine3D::Mesh& mesh);
			Bounds3D& operator+=(const Bounds3D& other);
			void translate(const QVector3D& displacement);
			void scale(const QVector3D& scales);
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
			float lengthX()const;
			float lengthY()const;
			float lengthZ()const;
			QVector3D centre()const;
			QVector3D lengths()const;
			const std::array<float, 6>& bound()const;
			QVector3D displaceWithin(const Bounds3D& child, QVector3D displacement =  QVector3D())const;
			std::array<float, 6> calculateMaxDisplacement(const Bounds3D & child)const;
			bool contains(const Hix::Engine3D::Bounds3D& other)const;
			bool intersects(const Hix::Engine3D::Bounds3D& other)const;
			bool intersects2D(const Hix::Engine3D::Bounds3D& other)const;

			//********When centred around origin ********
			Bounds3D(QVector3D lengths);
			void setXLength(float length);
			void setYLength(float length);
			void setZLength(float length);

		private:
			std::array<float, 6> _bound;

		};

		template<typename ItrType>
		Hix::Engine3D::Bounds3D bound2D(ItrType itr, ItrType end)
		{
			Hix::Engine3D::Bounds3D bound;
			for (; itr != end; ++itr)
			{
				bound.update(QVector3D(itr->x(), itr->y(), 0));
			}
			return bound;
		}
		template<typename SceneEntityDType>
		Hix::Engine3D::Bounds3D combineBounds(const std::unordered_set<SceneEntityDType*>& set)
		{
			Hix::Engine3D::Bounds3D bound;
			for (auto& e : set)
			{
				bound += e->recursiveAabb();
			}
			return bound;
		}



	}
}
