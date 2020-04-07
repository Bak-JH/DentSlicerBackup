#pragma once
#include "../polyclipping.h"
#include <qvector2d.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <array>
#include "../../../common/RandomGen.h"
namespace Hix
{
	namespace Engine3D
	{
		class Mesh;
	}
}

namespace Hix
{
	namespace Polyclipping
	{
		typedef std::array<QVector2D, 3> Triangle;
		//WARNING: These classes are dirty, so use them for a single triangulation and don't reuse them.

		class PolytreeCDT
		{

		public:
			PolytreeCDT(const ClipperLib::PolyTree* polytree);
			PolytreeCDT(const std::unordered_map<ClipperLib::IntPoint, QVector2D>* map, const ClipperLib::PolyTree* polytree);
			std::unordered_map<ClipperLib::PolyNode*, std::vector<Triangle>> triangulate();
			~PolytreeCDT();

		private:
			const ClipperLib::PolyTree* _tree = nullptr;
			const std::unordered_map<ClipperLib::IntPoint, QVector2D>* _floatIntMap = nullptr;

		};

		class CDTImpl;
		class MeshCDT
		{
			MeshCDT(Hix::Engine3D::Mesh* mesh, bool isReverse);
			virtual ~MeshCDT();
			void setZ(float z);
			void insertSolidContour(const std::vector<QVector2D>& contour);
			//overwrites z
			void insertSolidContourZAxis(const std::vector<QVector3D>& contour);

			//triangulate, append tris to mesh
			void triangulateAndAppend();
		private:
			Hix::Engine3D::Mesh* _mesh;
			std::unique_ptr<CDTImpl> _impl;
			float _z = 0.0f;
		};


	}

}

