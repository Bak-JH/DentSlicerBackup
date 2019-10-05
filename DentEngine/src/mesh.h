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

		class HalfEdgeConstItr;
		class VertexConstItr;
		class FaceConstItr;

		class HalfEdgeConstItr : public RandomAccessIteratorBase<HalfEdgeConstItr, Mesh>
		{
		public:
			using RandomAccessIteratorBase::RandomAccessIteratorBase;
			HalfEdgeConstItr next()const;
			HalfEdgeConstItr prev()const;
			void moveNext();
			void movePrev();
			VertexConstItr from()const;
			VertexConstItr to()const;
			FaceConstItr owningFace()const;

			//HalfEdgeConstItr twin;
			std::unordered_set<HalfEdgeConstItr> twins()const;
			//twins in same direction
			std::unordered_set<HalfEdgeConstItr> nonTwins()const;
			//twins + nonTwins
			std::unordered_set<HalfEdgeConstItr> allFromSameEdge()const;
			std::unordered_set<FaceConstItr> nonOwningFaces()const;
			//similar to non-owning, but half edges are on opposite direction ie) faces facing the same orientation
			std::unordered_set<FaceConstItr> twinFaces()const;
			bool isTwin(const HalfEdgeConstItr& other)const;
		private:
			const HalfEdge& ref()const;

		};

		class FaceConstItr : public RandomAccessIteratorBase<FaceConstItr, Mesh>
		{
		public:
			using RandomAccessIteratorBase::RandomAccessIteratorBase;
			const QVector3D& fn()const;
			HalfEdgeConstItr edge()const;
			std::array<VertexConstItr, 3> meshVertices()const;
			std::array<size_t, 3> getVerticeIndices()const;
			std::array<float, 3> sortZ()const;
			float getFaceZmin()const;
			float getFaceZmax()const;
			bool getEdgeWithVertices(HalfEdgeConstItr& result, const VertexConstItr& a, const VertexConstItr& b)const;
			bool isNeighborOf(const FaceConstItr& nFace)const;
		private:
			const MeshFace& ref()const;
		};

		class VertexConstItr : public RandomAccessIteratorBase<VertexConstItr, Mesh>
		{
		public:
			using RandomAccessIteratorBase::RandomAccessIteratorBase;
			const QVector3D& vn()const;
			const QVector3D& position()const;
			std::unordered_set<HalfEdgeConstItr> leavingEdges()const;
			std::unordered_set<HalfEdgeConstItr> arrivingEdges()const;
			std::unordered_set<VertexConstItr> connectedVertices()const;
			bool disconnected()const;
			std::vector<FaceConstItr> connectedFaces()const;
		private:
			const MeshVertex& ref()const;
		};


		class HalfEdgeItr : public HalfEdgeConstItr
		{
		public:
			using HalfEdgeConstItr::HalfEdgeConstItr;
			HalfEdge& ref()const;
		};

		class FaceItr : public FaceConstItr
		{
		public:
			using FaceConstItr::FaceConstItr;
			MeshFace& ref()const;

		};

		class VertexItr : public VertexConstItr
		{
		public:
			using VertexConstItr::VertexConstItr;
			MeshVertex& ref()const;
		};


		//actual memory footprint
		struct HalfEdge
		{
			uint32_t next;
			uint32_t from;
			uint32_t to;
			uint32_t owningFace;
		};
		struct MeshFace {
			uint32_t edge;
		};
		struct MeshVertex {
			MeshVertex(QVector3D);
			friend inline bool operator== (const MeshVertex& a, const MeshVertex& b) {
				return a.position == b.position;
			}
			friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b) {
				return a.position != b.position;
			}
			QVector3D position;
			std::unordered_set<uint32_t> leavingEdges;
			std::unordered_set<uint32_t> arrivingEdges;
		};





		class MeshIteratorFactory
		{
		public:
			MeshIteratorFactory();
			MeshIteratorFactory(Mesh* mesh);
		protected:
			Mesh* _mesh;
		};

		class HalfEdgeItrFactory: private MeshIteratorFactory
		{
		public:
			typedef HalfEdgeItr iterator;
			typedef HalfEdgeConstItr const_iterator;
			typedef IndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory> containerType;
			using MeshIteratorFactory::MeshIteratorFactory;
			iterator buildIterator(size_t index, const containerType* containerPtr)const;
			const_iterator buildConstIterator(size_t index, const containerType* containerPtr)const;
		};


		class VertexItrFactory : private MeshIteratorFactory
		{
		public:
			typedef VertexItr iterator;
			typedef VertexConstItr const_iterator;
			typedef IndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory> containerType;
			using MeshIteratorFactory::MeshIteratorFactory;
			iterator buildIterator(size_t index, const containerType* containerPtr)const;
			const_iterator buildConstIterator(size_t index, const containerType* containerPtr)const;
		};


		class FaceItrFactory: private MeshIteratorFactory
		{
		public:
			typedef FaceItr iterator;
			typedef FaceConstItr const_iterator;
			typedef IndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory> containerType;
			using MeshIteratorFactory::MeshIteratorFactory;
			iterator buildIterator(size_t index, const containerType* containerPtr)const;
			const_iterator buildConstIterator(size_t index, const containerType* containerPtr)const;
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
			FaceConstItr removeFace(FaceConstItr f_it);

			/********************** Getters **********************/
			//non-const getters. really should not be used
			auto& getVerticesNonConst();
			auto& getFacesNonConst();
			auto& getHalfEdgesNonConst();
			//const getter
			const auto& getVertices()const;
			const auto& getFaces()const;
			const auto& getHalfEdges()const;


			inline float x_min()const{ return _bounds.xMin();}
			inline float x_max()const{ return _bounds.xMax();}
			inline float y_min()const{ return _bounds.yMin();}
			inline float y_max()const{ return _bounds.yMax();}
			inline float z_min()const{ return _bounds.zMin();}
			inline float z_max()const{ return _bounds.zMax();}
			void findNearSimilarFaces(QVector3D normal,FaceConstItr mf,
				std::unordered_set<FaceConstItr>& result, float maxNormalDiff = 0.1f, size_t maxCount = 10000)const;
			/********************** Stuff that can be public **********************/
			const Bounds3D& bounds()const;
		private:
			/********************** Helper Functions **********************/
            //set twin relationship for this edge as well as matching twin edge

			size_t addOrRetrieveFaceVertex(const QVector3D& v);
			void removeVertexHash(QVector3D pos);
			void addHalfEdgesToFace(std::array<size_t, 3> faceVertices, size_t faceIdx);

			//index changed event callback
			void vtxIndexChangedCallback(size_t oldIdx, size_t newIdx);
			void faceIndexChangedCallback(size_t oldIdx, size_t newIdx);
			void hEdgeIndexChangedCallback(size_t oldIdx, size_t newIdx);

			MeshVtxHasher _vtxHasher;
			std::unordered_map<size_t, VertexConstItr> _verticesHash;
			TrackedIndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory> vertices;
			TrackedIndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory> halfEdges;
			TrackedIndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory> faces;
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

