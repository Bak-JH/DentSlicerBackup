#pragma once
#include "MeshIterators.h"
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



namespace Hix
{
	namespace Render
	{
		class SceneEntity;
	}
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
			void vertexOffset(float factor);
			void centerMesh();
			void vertexRotate(QMatrix4x4 tmpmatrix);
			void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
			void reverseFace(FaceItr faceItr);
			void reverseFaces();
			QVector3D transformedVtx(const VertexConstItr& vtx, const Qt3DCore::QTransform& transform)const;
			void clear();
            bool addFace(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2);
			bool addFace(const FaceConstItr& face);
			void removeFaces(const std::unordered_set<FaceConstItr>& f_it);
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
			/********************** Getters **********************/
			//non-const getters. really should not be used
			TrackedIndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory>& getVertices();
			TrackedIndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory>& getFaces();
			TrackedIndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory>& getHalfEdges();
			//const getter
			const TrackedIndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory>& getVertices()const;
			const TrackedIndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory>& getFaces()const;
			const TrackedIndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory>& getHalfEdges()const;


			inline float x_min()const{ return _bounds.xMin();}
			inline float x_max()const{ return _bounds.xMax();}
			inline float y_min()const{ return _bounds.yMin();}
			inline float y_max()const{ return _bounds.yMax();}
			inline float z_min()const{ return _bounds.zMin();}
			inline float z_max()const{ return _bounds.zMax();}
			std::unordered_set<FaceConstItr> findNearSimilarFaces(QVector3D normal,FaceConstItr mf, float maxNormalDiff = 0.1f, size_t maxCount = 10000)const;

			/********************** Stuff that can be public **********************/
			const Bounds3D& bounds()const;
			void setSceneEntity(const Render::SceneEntity* entity);
			const Render::SceneEntity* entity()const;
			QVector4D toWorld(const QVector4D& local)const;
			QVector4D toLocal(const QVector4D& world)const;
			QVector3D ptToWorld(const QVector3D& local)const;
			QVector3D vectorToWorld(const QVector3D& local)const;
			QVector3D ptToLocal(const QVector3D& world)const;
			QVector3D vectorToLocal(const QVector3D& world)const;

		private:
			void vertexMove(const QVector3D& direction);
			const Render::SceneEntity* _entity = nullptr;
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
			std::unordered_multimap<size_t, VertexConstItr> _verticesHash;
			TrackedIndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory> vertices;
			TrackedIndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory> halfEdges;
			TrackedIndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory> faces;

			//axis aligned bound box, needs to be recalculated even when just transform changed.
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



namespace std
{
	using namespace Hix::Engine3D;
	template<>
	struct hash<VertexConstItr>
	{
		size_t operator()(const VertexConstItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<HalfEdgeConstItr>
	{
		size_t operator()(const HalfEdgeConstItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<FaceConstItr>
	{
		size_t operator()(const FaceConstItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<VertexItr>
	{
		size_t operator()(const VertexItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<FaceItr>
	{
		size_t operator()(const FaceItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<HalfEdgeItr>
	{
		size_t operator()(const HalfEdgeItr& v)const
		{
			return v.index();
		}
	};
}