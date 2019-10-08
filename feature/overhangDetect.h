#pragma once
#include <unordered_set>
#include <unordered_map>

#include <vector>
#include "DentEngine/src/mesh.h"

namespace Hix
{
	namespace OverhangDetect
	{ 
		typedef std::pair<QVector3D, Hix::Engine3D::FaceConstItr> FaceOverhang;
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
			return hasher(pt.first);
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
		typedef std::unordered_set<std::variant<VertexConstItr, FaceOverhang>>  Overhangs;
		std::vector<QVector3D> toCoords(const Overhangs& overhangs);

		class Detector
		{
		public:
			Overhangs detectOverhang(const Mesh* shellMesh);
		private:
			std::unordered_set<VertexConstItr> localMinFacingDown(const Mesh* mesh);
			std::unordered_map<QVector3D, FaceConstItr> faceOverhangDetect(const Mesh* mesh);
			void faceOverhangPoint(const FaceConstItr& overhangFace, std::unordered_map<QVector3D, FaceConstItr>& output);

			std::unordered_map<FaceConstItr, QVector3D> _worldFNCache;

		};

	}







}

