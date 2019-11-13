#pragma once
#include <unordered_set>
#include <unordered_map>

#include <vector>
#include "DentEngine/src/mesh.h"

namespace Hix
{
	using namespace Engine3D;
	namespace OverhangDetect
	{ 
		struct FaceOverhang
		{
			QVector3D coord;
			Hix::Engine3D::FaceConstItr face;
			const Hix::Engine3D::Mesh* owner()const;
			bool operator==(const FaceOverhang& o) const
			{
				return face == o.face && coord == o.coord;
			}
		};
		typedef std::variant<VertexConstItr, FaceOverhang>  Overhang;

	}
}

namespace std
{
	template<>
	struct hash<Hix::OverhangDetect::FaceOverhang>
	{
		//2D only!
		std::size_t operator()(const Hix::OverhangDetect::FaceOverhang& pt)const
		{
			constexpr static Hix::Engine3D::MeshVtxHasher hasher;
			return hasher(pt.coord);
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
		typedef std::unordered_set<typename OverhangDetect::Overhang>  Overhangs;
		QVector3D toCoord(const Overhang& overhangs);
		std::vector<QVector3D> toCoords(const Overhangs& overhangs);

		class Detector
		{
		public:
			OverhangDetect::Overhangs detectOverhang(const Mesh* shellMesh);
		private:
			std::unordered_set<VertexConstItr> localMinFacingDown(const Mesh* mesh);
			std::unordered_map<QVector3D, FaceConstItr> faceOverhangDetect(const Mesh* mesh);
			void faceOverhangPoint(const FaceConstItr& overhangFace, std::unordered_map<QVector3D, FaceConstItr>& output);

			std::unordered_map<FaceConstItr, QVector3D> _worldFNCache;

		};

	}







}

