#pragma once
#include <vector>
#include <Qt3DCore/qtransform.h>
#include <QVector3D>
#include <QHash>
#include "polyclipping/polyclipping.h"
#include <QTransform>
#include <QTime>
#include <array>
#include <variant>
#include "Bounds3D.h"
#include "../../common/TrackedIndexedList.h"
#include "../../common/Hasher.h"
#include "../../common/RandomAccessIteratorBase.h"
#define cos50 0.64278761
#define cos100 -0.17364818
#define cos150 -0.8660254
#define FZERO 0.00001f

namespace std
{
	//template<>
	//struct hash<QVector3D>
	//{
	//	size_t operator()(const QVector3D& v)const
	//	{
	//	}
	//};
}

namespace Hix
{
	namespace Engine3D
	{
		static constexpr float VTX_INBOUND_DIST = 0.002f;//10 micron resolution
		static constexpr float VTX_INBOUND_DITTER = VTX_INBOUND_DIST / 2;
		static constexpr float VTX_3D_DIST = 0.0034f;
		//hasher unique to each mesh class...
		class MeshVtxHasher: public Hix::Common::Hasher<QVector3D>
		{
		public:
			//TODO
			//void optimizeForBound(const Bounds3D& meshBound);
			size_t operator()(const QVector3D& hashed)const override;
		};

        template<class container_type, class itr_type>
        itr_type getEquivalentItr(const container_type& b, const container_type& a, const itr_type& aItr)
        {
            size_t idx = aItr - a.cbegin();
            return b.cbegin() + idx;
        }


		// plane contains at least 3 vertices contained in the plane in clockwise direction



		struct HalfEdge;
		struct MeshVertex;
		struct MeshFace;
		class Mesh;

		class HalfEdgeConstItr;
		class VertexConstItr;
		class FaceConstItr;

		class HalfEdgeConstItr : public RandomAccessIteratorBase<HalfEdgeConstItr, Mesh>
		{
		public:
			HalfEdgeConstItr();
			HalfEdgeConstItr(size_t idx, Mesh* owner);
			HalfEdgeConstItr next()const;
			HalfEdgeConstItr prev()const;
			VertexConstItr from()const;
			VertexConstItr to()const;
			FaceConstItr owningFace()const;
			//HalfEdgeConstItr twin;
			std::unordered_set<HalfEdgeItr> twins()const;
			//twins in same direction
			std::unordered_set<HalfEdgeItr> nonTwins()const;
			//twins + nonTwins
			std::unordered_set<HalfEdgeItr> allFromSameEdge()const;
			std::vector<FaceItr> nonOwningFaces()const;
			//similar to non-owning, but half edges are on opposite direction ie) faces facing the same orientation
			std::unordered_set<FaceItr> twinFaces()const;
			bool isTwin(const HalfEdgeItr& other)const;

		};

		class FaceConstItr : public MeshDataIterator<FaceItr>
		{
		public:
			FaceItr();
			FaceItr(size_t idx, Mesh* owner);
			const QVector3D& fn()const;
			void setFn(const QVector3D& val);

			HalfEdgeItr edge()const;
			std::array<VertexItr, 3> meshVertices()const;
			//std::array<std::vector<FaceConstItr>, 3> neighboring_faces;
			std::array<size_t, 3> getVerticeIndices(const Mesh * owningMesh)const;
			std::array<float, 3> sortZ()const;
			float getFaceZmin()const;
			float getFaceZmax()const;
			bool getEdgeWithVertices(HalfEdgeItr& result, const VertexItr& a, const VertexItr& b)const;
			bool isNeighborOf(const FaceItr& nFace)const;

		};

		class VertexConstItr : public MeshDataIterator<VertexItr>
		{
		public:
			VertexItr();
			VertexItr(size_t idx, Mesh* owner);
			const QVector3D& vn()const;
			void setVn(const QVector3D& val);
			const QVector3D& position()const;
			void setPosition(const QVector3D& val);
			std::unordered_set<HalfEdgeItr> leavingEdges()const;
			std::unordered_set<HalfEdgeItr> arrivingEdges()const;
			std::unordered_set<VertexItr> connectedVertices()const;
			bool empty()const;
			void calculateNormalFromFaces();
			std::vector<FaceItr> connectedFaces()const;
		};



		typedef const HalfEdgeItr HalfEdgeConstItr;
		typedef const VertexItr VertexConstItr;
		typedef const FaceItr FaceConstItr;

		//actual memory footprint
		struct HalfEdge
		{
			size_t next;
			size_t prev;
			size_t from;
			size_t to;
			size_t owningFace;
		};
		struct MeshFace {
			QVector3D fn;
			size_t edge;
		};
		struct MeshVertex {
			QVector3D position;
			QVector3D vn;
			std::vector<size_t> leavingEdges;
			std::vector<size_t> arrivingEdges;
		};

		






		bool isCommonManifoldFace(const FaceConstItr& a, const FaceConstItr& b, std::unordered_set<FaceConstItr> pool);
		bool findCommonManifoldFace(FaceConstItr& result, const FaceConstItr& a, std::unordered_set<FaceConstItr>& candidates, std::unordered_set<FaceConstItr> pool);


		typedef std::array<QVector3D,3> Plane;

		class Path3D : public std::vector<MeshVertex> {
		public:
			ClipperLib::Path projection;
			std::vector<Path3D> inner;
			std::vector<Path3D> outer;
		};

		typedef std::vector<Path3D> Paths3D;




		class Mesh {
		public:




			Mesh();
			Mesh(const Mesh& o);
			Mesh& operator=(Mesh other);

            Mesh& operator+=(const Mesh& o);
			//copy assign
			//Mesh& operator=(const Mesh o);
			/********************** Undo state functions***********************/

			/********************** Mesh Edit Functions***********************/
			//void vertexApplyTransformation(const Qt3DCore::QTransform& transform);
			void vertexOffset(float factor);
			void vertexMove(QVector3D direction);
			void centerMesh();
			void vertexRotate(QMatrix4x4 tmpmatrix);
			void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
			void reverseFace(FaceConstItr faceItr);
			void reverseFaces();
            bool addFace(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2);
			bool addFace(const FaceConstItr& face);
			TrackedIndexedList<MeshFace>::const_iterator removeFace(FaceConstItr f_it);
			TrackedIndexedList<MeshVertex>& getVerticesNonConst();
			TrackedIndexedList<MeshFace>& getFacesNonConst();
			TrackedIndexedList<HalfEdge>& getHalfEdgesNonConst();
			/********************** Mesh Modify and Copy Functions***********************/
			Mesh* vertexMoved(QVector3D direction)const;




			/********************** Helper Functions **********************/


			//MeshFace idx2MF(int idx)const;
			//MeshVertex idx2MV(int idx)const;

			/********************** Getters **********************/
			//const getter
			const TrackedIndexedList<MeshVertex>& getVertices()const;
			const TrackedIndexedList<MeshFace>& getFaces()const;
			const TrackedIndexedList<HalfEdge>& getHalfEdges()const;


			inline float x_min()const{ return _bounds.xMin();}
			inline float x_max()const{ return _bounds.xMax();}
			inline float y_min()const{ return _bounds.yMin();}
			inline float y_max()const{ return _bounds.yMax();}
			inline float z_min()const{ return _bounds.zMin();}
			inline float z_max()const{ return _bounds.zMax();}
			void findNearSimilarFaces(QVector3D normal,FaceConstItr mf,
				std::unordered_set<FaceConstItr>& result, float maxNormalDiff = 0.1f, size_t maxCount = 10000)const;



			/********************** index to data **********************/

			inline MeshFace& idx2mf(size_t idx)
			{
				return faces[idx];
			}
			inline MeshVertex& idx2vtx(size_t idx)
			{
				return vertices[idx];
			}

			inline const MeshFace& idx2mf(size_t idx)const
			{
				return faces[idx];
			}
			inline const MeshVertex& idx2vtx(size_t idx)const
			{
				return vertices[idx];
			}
			/********************** opposite **********************/

			inline size_t indexOf(const HalfEdgeConstItr& itr)const
			{
				return itr - halfEdges.cbegin();
			}
			inline size_t indexOf(const VertexConstItr& itr)const
			{
				return itr - vertices.cbegin();
			}
			inline size_t indexOf(const FaceConstItr& itr)const
			{
				return itr - faces.cbegin();
			}
			/********************** Stuff that can be public **********************/
			const Bounds3D& bounds()const;
		private:
			/********************** Helper Functions **********************/
            //set twin relationship for this edge as well as matching twin edge

			VertexItr addOrRetrieveFaceVertex(const QVector3D& v);
			void removeVertexHash(QVector3D pos);
			void addHalfEdgesToFace(std::array<VertexItr, 3> faceVertices, FaceConstItr face);

			//index changed event callback
			void vtxIndexChangedCallback(size_t oldIdx, size_t newIdx);
			void faceIndexChangedCallback(size_t oldIdx, size_t newIdx);
			void hEdgeIndexChangedCallback(size_t oldIdx, size_t newIdx);

			MeshVtxHasher _vtxHasher;
			std::unordered_map<size_t, VertexConstItr> _verticesHash;
			TrackedIndexedList<MeshVertex> vertices;
			TrackedIndexedList<HalfEdge> halfEdges;
			TrackedIndexedList<MeshFace> faces;
			Bounds3D _bounds;

		};

		QHash<uint32_t, ClipperLib::Path>::iterator findSmallestPathHash(QHash<uint32_t, ClipperLib::Path> pathHash);

		// construct closed contour using segments created from identify step
		Paths3D contourConstruct3D(Paths3D hole_edges);

		bool intPointInPath(ClipperLib::IntPoint ip, ClipperLib::Path p);
		bool pathInpath(Path3D target, Path3D in);

		std::vector<std::array<QVector3D, 3>> interpolate(Path3D from, Path3D to);

	};

	namespace Debug
	{
		using namespace Hix::Engine3D;

		QDebug operator<< (QDebug d, const VertexConstItr& obj);
		QDebug operator<< (QDebug d, const HalfEdgeConstItr& obj);
		QDebug operator<< (QDebug d, const FaceConstItr& obj);
	}

};

