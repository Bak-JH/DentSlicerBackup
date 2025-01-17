#pragma once
#include "MeshIterators.h"
#include <QHash>
#include "../feature/polyclipping/polyclipping.h"
#include <QTransform>
#include <QTime>
#include <array>
#include <variant>
#include <optional>
#include "../../common/TrackedIndexedList.h"
#include "../../common/Hasher.h"



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


		struct MeshDeleteGuard
		{
			TrackedIndexedList<MeshVertex, std::allocator<MeshVertex>, VertexItrFactory>::DeleteGuardType vtxDeleteGuard;
			TrackedIndexedList<HalfEdge, std::allocator<HalfEdge>, HalfEdgeItrFactory>::DeleteGuardType hEdgeDeleteGuard;
			TrackedIndexedList<MeshFace, std::allocator<MeshFace>, FaceItrFactory>::DeleteGuardType faceDeleteGuard;
			MeshDeleteGuard(Mesh* mesh);
			MeshDeleteGuard& operator=(MeshDeleteGuard&& o) = default;
			MeshDeleteGuard(MeshDeleteGuard&& o) = default;
			void flush();
			void release();
			MeshDeleteGuard& operator+=(MeshDeleteGuard&& other);


		};

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
			void vertexApplyTransformation(const Qt3DCore::QTransform& transform);
			void vertexOffset(float factor);
			void vertexRotate(const QQuaternion& rot);

			//returns amount moved
			QVector3D centerMesh();
			void reverseFace(FaceItr faceItr);
			void reverseFaces();
			void clear();
			template <typename ContainerType>
			void addFaces(const ContainerType& cont)
			{
				if (cont.size() == 0 || cont.size() % 3 != 0)
					throw std::runtime_error("vertices count invalid for addFaces");
				auto itr = cont.cbegin();
				auto cend = cont.cend();
				while (itr != cend)
				{
					addFace(*itr++, *itr++, *itr++);
				}
			}
			std::optional<std::array<size_t, 3>> addFace(const QVector3D& v0, const QVector3D& v1, const QVector3D& v2);
			bool addFace(const FaceConstItr& face);
			MeshDeleteGuard removeFacesWithoutShifting(const std::unordered_set<FaceConstItr>& faceItrs);
			void removeFaces(const std::unordered_set<FaceConstItr>& faceItrs);
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


			void setSceneEntity(const Render::SceneEntity* entity);
			const Render::SceneEntity* entity()const;
			QVector4D toWorld(const QVector4D& local)const;
			QVector4D toLocal(const QVector4D& world)const;
			QVector3D ptToWorld(const QVector3D& local)const;
			QVector3D vectorToWorld(const QVector3D& local)const;
			QVector3D ptToLocal(const QVector3D& world)const;
			QVector3D vectorToLocal(const QVector3D& world)const;
			VertexConstItr getVtxAtLocalPos(const QVector3D& pos)const;

			
			//templates

			template <typename FaceCond>
			std::unordered_set<FaceConstItr> findNearFaces(FaceConstItr startFace, FaceCond cond, size_t maxCount) const
			{
				std::unordered_set<FaceConstItr> result;
				std::unordered_set<FaceConstItr> explored;
				std::unordered_set<HalfEdgeConstItr> frontiers;
				std::deque<FaceConstItr>q;
				result.reserve(maxCount);
				explored.reserve(maxCount);
				q.emplace_back(startFace);
				result.emplace(startFace);
				explored.emplace(startFace);
				while (!q.empty())
				{
					auto curr = q.front();
					q.pop_front();
					if (explored.size() == maxCount)
						break;
					auto edge = curr.edge();
					for (size_t i = 0; i < 3; ++i, edge.moveNext()) {
						auto nFaces = edge.twinFaces();
						for (auto nFace : nFaces)
						{
							if (explored.find(nFace) == explored.end())
							{
								explored.emplace(nFace);
								if (cond(nFace))
								{
									q.emplace_back(nFace);
									result.emplace(nFace);
								}
							}
							else
							{
								if (result.find(nFace) == result.end())
								{
									qDebug() << "overtaken face?";
								}
							}
						}
					}
				}
				return result;
			}
			std::unordered_set<FaceConstItr> findNearSimilarFaces(QVector3D normal, FaceConstItr mf, float maxNormalDiff = 0.1f, size_t maxCount = 15000)const;



		private:
			void rehashVtcs();
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

		};
	};

	namespace Debug
	{
		QDebug operator<< (QDebug d, const Hix::Engine3D::VertexConstItr& obj);
		QDebug operator<< (QDebug d, const Hix::Engine3D::HalfEdgeConstItr& obj);
		QDebug operator<< (QDebug d, const Hix::Engine3D::FaceConstItr& obj);
	}

};



namespace std
{
	template<>
	struct hash<Hix::Engine3D::VertexConstItr>
	{
		size_t operator()(const Hix::Engine3D::VertexConstItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<Hix::Engine3D::HalfEdgeConstItr>
	{
		size_t operator()(const Hix::Engine3D::HalfEdgeConstItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<Hix::Engine3D::FaceConstItr>
	{
		size_t operator()(const Hix::Engine3D::FaceConstItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<Hix::Engine3D::VertexItr>
	{
		size_t operator()(const Hix::Engine3D::VertexItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<Hix::Engine3D::FaceItr>
	{
		size_t operator()(const Hix::Engine3D::FaceItr& v)const
		{
			return v.index();
		}
	};
	template<>
	struct hash<Hix::Engine3D::HalfEdgeItr>
	{
		size_t operator()(const Hix::Engine3D::HalfEdgeItr& v)const
		{
			return v.index();
		}
	};
}