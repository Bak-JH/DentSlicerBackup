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

		typedef typename IndexedListItr::const_iterator <HalfEdge>HalfEdgeConstItr;
		typedef typename IndexedListItr::const_iterator <MeshVertex>VertexConstItr;
		typedef typename IndexedListItr::const_iterator <MeshFace>FaceConstItr;
		typedef typename IndexedListItr::iterator		<HalfEdge>HalfEdgeItr;
		typedef typename IndexedListItr::iterator		<MeshVertex>VertexItr;
		typedef typename IndexedListItr::iterator		<MeshFace>FaceItr;


		struct HalfEdge
		{
			HalfEdge()
			{}
			HalfEdgeConstItr next;
			HalfEdgeConstItr prev;
			VertexConstItr from;
			VertexConstItr to;
			FaceConstItr owningFace;
			//TODO: guarantee no self intersection occurs and we can use this
			//HalfEdgeConstItr twin;
			std::unordered_set<HalfEdgeConstItr> twins()const;
			//twins in same direction
			std::unordered_set<HalfEdgeConstItr> nonTwins()const;
			//twins + nonTwins
			std::unordered_set<HalfEdgeConstItr> allFromSameEdge()const;
			std::vector<FaceConstItr> nonOwningFaces()const;
			//similar to non-owning, but half edges are on opposite direction ie) faces facing the same orientation
            std::unordered_set<FaceConstItr> twinFaces()const;
			bool isTwin(const HalfEdgeConstItr& other)const;

		};

		class HalfEdgeCirculator
		{
		public:
			HalfEdgeCirculator(HalfEdgeConstItr itrW);
			//you need to double *, but it's worth it for indexing capability
			//HalfEdgeConstItr& toItrW()const;
			HalfEdgeConstItr& toItr();

			const HalfEdge& operator*()const;
			void operator++();
			void operator--();
			HalfEdgeCirculator operator--(int);
			HalfEdgeCirculator operator++(int);
			const HalfEdge* operator->() const;
			const HalfEdge* toPtr() const;

		private:
			HalfEdgeConstItr _hEdgeItr;
		};


		struct MeshFace {
			MeshFace()
			{}
			QVector3D fn;
			HalfEdgeConstItr edge;
			std::array<VertexConstItr, 3> meshVertices()const;
			//std::array<std::vector<FaceConstItr>, 3> neighboring_faces;
			HalfEdgeCirculator edgeCirculator()const;
			std::array<size_t, 3> getVerticeIndices(const Mesh* owningMesh)const;
			std::array<float, 3> sortZ()const;
			float getFaceZmin()const;
			float getFaceZmax()const;
			bool getEdgeWithVertices(HalfEdgeConstItr& result,const VertexConstItr& a, const VertexConstItr& b)const;
			bool isNeighborOf(const FaceConstItr& nFace)const;

		};
		struct MeshVertex {
			MeshVertex() 
			{}
			MeshVertex(QVector3D pPosition):position(pPosition) {}
			friend inline bool operator== (const MeshVertex& a, const MeshVertex& b) {
				return a.position == b.position;
			}
			friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b) {
				return a.position != b.position;
			}
			std::unordered_set<VertexConstItr> connectedVertices()const;
			bool empty()const;
			void calculateNormalFromFaces();
			std::vector<FaceConstItr> connectedFaces()const;
			QVector3D position;
			QVector3D vn;
			std::vector<HalfEdgeConstItr> leavingEdges;
			std::vector<HalfEdgeConstItr> arrivingEdges;



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
			//short hand for TrackedList::toNormItr
			inline VertexItr toNormItr(const VertexConstItr& itr)
			{
				return vertices.toNormItr(itr);
			}
			inline FaceItr toNormItr(const FaceConstItr& itr)
			{
				return faces.toNormItr(itr);
			}
			inline HalfEdgeItr toNormItr(const HalfEdgeConstItr& itr)
			{
				return halfEdges.toNormItr(itr);
			}

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
