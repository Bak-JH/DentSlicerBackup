#pragma once
#include <unordered_set>
#include <unordered_map>

#include <vector>
#include "Mesh/mesh.h"
#include "../Mesh/RayCaster.h"
#include "feature/interfaces/Feature.h"
namespace Hix
{
	namespace OverhangDetect
	{ 
		class Overhang
		{
		public:
			Overhang(const Hix::Engine3D::FaceConstItr& face, const QVector3D& coord);
			Overhang(const Hix::Engine3D::VertexConstItr& vtx);
			const QVector3D& coord()const;
			bool operator==(const Overhang& o) const;
			const Hix::Engine3D::Mesh* owner()const;
			const QVector3D& normal()const;
			const QVector3D& primitiveOverhang()const;
			Hix::Engine3D::FaceConstItr nearestFace()const;
			bool isVertex()const;
			//void setRaycastResult(const RayHits& hit);
			//const RayHits& rayHit()const;
		private:
			std::variant<Hix::Engine3D::FaceConstItr, Hix::Engine3D::VertexConstItr> _primitive;
			QVector3D _coord;
			QVector3D _primNormal;
			QVector3D _normal;
			Hix::Engine3D::RayHits _rayHit;
		};

	}
}

namespace std
{
	template<>
	struct hash<Hix::OverhangDetect::Overhang>
	{
		//2D only!
		std::size_t operator()(const Hix::OverhangDetect::Overhang& pt)const
		{
			constexpr static Hix::Engine3D::MeshVtxHasher hasher;
			return hasher(pt.coord());
			//TODO

		}
	};

}

namespace Hix
{
	namespace Slicer
	{
		class Planes;
		class Slices;
	}
	namespace OverhangDetect
	{
		typedef std::vector<OverhangDetect::Overhang>  Overhangs;
		class Detector
		{
		public:
			OverhangDetect::Overhangs detectOverhang(const Hix::Engine3D::Mesh* shellMesh);
		private:
			std::unordered_set<Hix::Engine3D::VertexConstItr> localMinFacingDown(const Hix::Engine3D::Mesh* mesh);
			std::unordered_multimap<Hix::Engine3D::FaceConstItr, QVector3D> faceOverhangDetect(const Hix::Engine3D::Mesh* mesh);
			void faceOverhangPoint(const Hix::Engine3D::FaceConstItr& overhangFace, std::unordered_multimap<Hix::Engine3D::FaceConstItr, QVector3D>& output);

			std::unordered_map<Hix::Engine3D::FaceConstItr, QVector3D> _worldFNCache;

		};

	}







}

