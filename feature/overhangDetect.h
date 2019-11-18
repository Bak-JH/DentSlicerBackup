#pragma once
#include <unordered_set>
#include <unordered_map>

#include <vector>
#include "DentEngine/src/mesh.h"
#include "../Mesh/RayCaster.h"
namespace Hix
{
	using namespace Engine3D;
	namespace OverhangDetect
	{ 
		class Overhang
		{
		public:
			Overhang(const FaceConstItr& face, const QVector3D& coord);
			Overhang(const VertexConstItr& vtx);
			const QVector3D& coord()const;
			bool operator==(const Overhang& o) const;
			const Hix::Engine3D::Mesh* owner()const;
			const QVector3D& normal()const;
			//void setRaycastResult(const RayHits& hit);
			//const RayHits& rayHit()const;
		private:
			std::variant<FaceConstItr, VertexConstItr> _primitive;
			QVector3D _coord;
			QVector3D _normal;
			RayHits _rayHit;
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
	using namespace Engine3D;
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
			OverhangDetect::Overhangs detectOverhang(const Mesh* shellMesh);
		private:
			std::unordered_set<VertexConstItr> localMinFacingDown(const Mesh* mesh);
			std::unordered_multimap<FaceConstItr, QVector3D> faceOverhangDetect(const Mesh* mesh);
			void faceOverhangPoint(const FaceConstItr& overhangFace, std::unordered_multimap<FaceConstItr, QVector3D>& output);

			std::unordered_map<FaceConstItr, QVector3D> _worldFNCache;

		};

	}







}

