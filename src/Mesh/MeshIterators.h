#pragma once
#include "../../common/RandomAccessIteratorBase.h"
#include <vector>
#include <array>
#include <unordered_set>
#include <QVector3D>

namespace Hix
{
	namespace Engine3D
	{
		class Mesh;


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


		//iterator type info
		struct ConstItrInfo
		{
			typedef class HalfEdgeConstItr HalfEdgeItrType;
			typedef class VertexConstItr VertexItrType;
			typedef class FaceConstItr FaceItrType;
			typedef const class Hix::Engine3D::Mesh OwnerType;
		};
		struct NonConstItrInfo
		{
			typedef class HalfEdgeItr HalfEdgeItrType;
			typedef class VertexItr VertexItrType;
			typedef class FaceItr FaceItrType;
			typedef class Hix::Engine3D::Mesh OwnerType;
		};

		//actual base for edge, vtx, face iterators, specialized later for constness.
		template <typename HEItrType, typename T, typename TypeConstInfo>
		class HalfEdgeItrBase : public RandomAccessIteratorBase< HEItrType, typename TypeConstInfo::OwnerType>
		{
		public:
			using RandomAccessIteratorBase<HEItrType, typename TypeConstInfo::OwnerType>::RandomAccessIteratorBase;
			using ParentType = RandomAccessIteratorBase< HEItrType, typename TypeConstInfo::OwnerType>;
			using value_type = T;
			using pointer = value_type*;
			using reference = value_type&;

			reference ref()const
			{
				return ParentType::_owner->getHalfEdges()[ParentType::_index];
			}
			reference operator*() const
			{
				return ref();
			}
			pointer operator->() const
			{
				return &(ParentType::_owner->getHalfEdges()[ParentType::_index]);
			}
			HEItrType next()const
			{
				return HEItrType(ref().next, ParentType::_owner);
			}
			HEItrType prev()const
			{
				return HEItrType(next().next().index(), ParentType::_owner);
			}
			void moveNext()
			{
				ParentType::_index = ref().next;
			}
			void movePrev()
			{
				ParentType::_index = next().next().index();
			}
			typename TypeConstInfo::VertexItrType from()const
			{
				return TypeConstInfo::VertexItrType(ref().from, ParentType::_owner);
			}
			typename TypeConstInfo::VertexItrType to()const
			{
				return TypeConstInfo::VertexItrType(ref().to, ParentType::_owner);
			}
			typename TypeConstInfo::FaceItrType owningFace()const
			{
				return TypeConstInfo::FaceItrType(ref().owningFace, ParentType::_owner);
			}
			float localLength()const
			{
				float length = from().localPosition().distanceToPoint(to().localPosition());
				return length;
			}
			//HalfEdgeConstItr twin;
			std::unordered_set<HEItrType> twins()const
			{
				std::unordered_set<HEItrType> twinEdges;
				auto fromVtx = ref().from;
				for (auto oppDirEdge : to().leavingEdges())
				{
					if (oppDirEdge->to == fromVtx)
					{
						twinEdges.insert(oppDirEdge);
					}
				}
				return twinEdges;
			}
			bool isBoundary()const
			{
				if (twins().size() == 0)
					return true;
				return false;
			}

			//twins in same direction
			std::unordered_set<HEItrType> nonTwins()const
			{
				std::unordered_set<HEItrType> nonTwins;
				auto toVtx = to();
				for (auto sameDirEdge : from().leavingEdges())
				{
					if (sameDirEdge.to() == toVtx && *this != sameDirEdge)
					{
						nonTwins.insert(sameDirEdge);
					}
				}
				return nonTwins;
			}
			//twins + nonTwins
			std::unordered_set<HEItrType> allFromSameEdge()const
			{
				auto allEdges = twins();
				auto tmp = nonTwins();
				allEdges.insert(tmp.begin(), tmp.end());
				return allEdges;
			}
			std::unordered_set<typename TypeConstInfo::FaceItrType> nonOwningFaces()const
			{
				std::unordered_set<typename TypeConstInfo::FaceItrType> result;
				auto otherEdges = allFromSameEdge();
				for (auto each : otherEdges)
				{
					result.insert(each.owningFace());
				}
				return result;
			}
			//similar to non-owning, but half edges are on opposite direction ie) faces facing the same orientation
			std::unordered_set<typename TypeConstInfo::FaceItrType> twinFaces()const
			{
				std::unordered_set<typename TypeConstInfo::FaceItrType> twnFaces;
				for (auto& each : twins())
				{
					twnFaces.emplace(each.owningFace());
				}
				return twnFaces;
			}
			bool isTwin(const HEItrType& other)const
			{
				auto twns = twins();
				return twns.find(other) != twns.end();
			}
		};

		template <typename VtxItrType, typename T, typename TypeConstInfo>
		class VertexItrBase : public RandomAccessIteratorBase< VtxItrType, typename TypeConstInfo::OwnerType>
		{
		public:
			using RandomAccessIteratorBase<VtxItrType, typename TypeConstInfo::OwnerType>::RandomAccessIteratorBase;
			using ParentType = RandomAccessIteratorBase< VtxItrType, typename TypeConstInfo::OwnerType>;
			using value_type = T;
			using pointer = value_type*;
			using reference = value_type&;
			reference ref()const
			{
				return ParentType::_owner->getVertices()[ParentType::_index];
			}
			reference operator*() const
			{
				return ref();
			}
			pointer operator->() const
			{
				return &(ParentType::_owner->getVertices()[ParentType::_index]);
			}

			const QVector3D& localVn() const
			{
				QVector3D vn;
				auto faces = connectedFaces();
				for (auto& face : faces)
				{
					vn += face.localFn();
				}
				vn.normalize();
				return vn;
			}
			const QVector3D& worldVn() const
			{
				auto vn = localVn();
				vn = ParentType::_owner->vectorToWorld(vn);
				vn.normalize();
				return vn;
			}
			std::unordered_set<typename TypeConstInfo::HalfEdgeItrType> leavingEdges() const
			{
				std::unordered_set<typename TypeConstInfo::HalfEdgeItrType> edges;
				for (auto edgeIdx : ref().leavingEdges)
				{
					edges.emplace(edgeIdx, _owner);
				}
				return edges;
			}

			std::unordered_set<typename TypeConstInfo::HalfEdgeItrType> arrivingEdges() const
			{
				std::unordered_set<typename TypeConstInfo::HalfEdgeItrType> edges;
				for (auto edgeIdx : ref().arrivingEdges)
				{
					edges.emplace(edgeIdx, _owner);
				}
				return edges;
			}

			std::unordered_set<typename TypeConstInfo::VertexItrType> connectedVertices() const
			{
				std::unordered_set<typename TypeConstInfo::VertexItrType> connected;
				for (auto& each : leavingEdges())
				{
					connected.insert(each.to());
				}
				for (auto& each : arrivingEdges())
				{
					connected.insert(each.from());
				}
				return connected;
			}
			std::unordered_set< typename TypeConstInfo::HalfEdgeItrType> leavingBoundary()const
			{
				std::unordered_set< typename TypeConstInfo::HalfEdgeItrType> leavingBoundary;
				auto leavings = leavingEdges();
				auto arriving = arrivingEdges();
				std::unordered_set<size_t> arrivingFromVtcs;
				for (auto& each : arriving)
				{
					arrivingFromVtcs.insert(each->from);
				}
				
				for (auto& each : leavings)
				{
					if (arrivingFromVtcs.find(each->to) == arrivingFromVtcs.end())
					{
						leavingBoundary.insert(each);
					}
				}
				return leavingBoundary;
			}
			//std::unordered_set< typename TypeConstInfo::HalfEdgeItrType> arrivingBoundary()const
			//{
			//	std::unordered_set< typename TypeConstInfo::HalfEdgeItrType> leavingBoundary;
			//	auto leavings = leavingEdges();
			//	auto arriving = arrivingEdges();
			//	std::unordered_set<size_t> arrivingFromVtcs;
			//	for (auto& each : arriving)
			//	{
			//		arrivingFromVtcs.insert(each->from);
			//	}

			//	for (auto& each : leavings)
			//	{
			//		if (arrivingFromVtcs.find(each->to) == arrivingFromVtcs.end())
			//		{
			//			leavingBoundary.insert(each);
			//		}
			//	}
			//	return leavingBoundary;
			//}


			bool isConnected(const typename TypeConstInfo::VertexItrType& other)const
			{
				for (auto& each : leavingEdges())
				{
					if (each.to() == other)
						return true;
				}
				for (auto& each : arrivingEdges())
				{
					if (each.from() == other)
						return true;
				}
				return false;
			}

			bool disconnected() const
			{
				if (ref().leavingEdges.empty() && ref().arrivingEdges.empty())
					return true;
				else
					false;
			}

			std::unordered_set<typename TypeConstInfo::FaceItrType> connectedFaces() const
			{
				std::unordered_set<typename TypeConstInfo::FaceItrType> result;
				for (auto each : leavingEdges())
				{
					result.emplace(each.owningFace());
				}
				return result;
			}
			QVector3D localPosition() const
			{
				return ref().position;
			}
			QVector3D worldPosition()const
			{
				return ParentType::_owner->ptToWorld(ref().position);
			}



		};

		template <typename FaceItrType, typename T, typename TypeConstInfo>
		class FaceItrBase : public RandomAccessIteratorBase< FaceItrType, typename TypeConstInfo::OwnerType>
		{
		public:
			using RandomAccessIteratorBase<FaceItrType, typename TypeConstInfo::OwnerType>::RandomAccessIteratorBase;
			using ParentType = RandomAccessIteratorBase< FaceItrType, typename TypeConstInfo::OwnerType>;
			using value_type = T;
			using pointer = value_type*;
			using reference = value_type&;
			reference ref()const
			{
				return ParentType::_owner->getFaces()[ParentType::_index];
			}
			reference operator*() const
			{
				return ref();
			}
			pointer operator->() const
			{
				return &(ParentType::_owner->getFaces()[ParentType::_index]);
			}

			const QVector3D& localFn() const
			{
				auto fVtx = meshVertices();
				QVector3D fn = QVector3D::normal(fVtx[0].localPosition(), fVtx[1].localPosition(), fVtx[2].localPosition());
				return fn;
			}
			const QVector3D& worldFn() const
			{
				auto fn = localFn();
				fn = ParentType::_owner->vectorToWorld(fn);
				fn.normalize();
				return fn;
			}
			typename TypeConstInfo::HalfEdgeItrType edge() const
			{
				return typename TypeConstInfo::HalfEdgeItrType(ref().edge, _owner);
			}
			std::array<typename TypeConstInfo::VertexItrType, 3> meshVertices() const
			{
				std::array<typename TypeConstInfo::VertexItrType, 3> result;
				auto circulator = edge();
				for (size_t i = 0; i < 3; ++i)
				{
					result[i] = circulator.from();
					circulator.moveNext();
				}
				return result;
			}
			bool hasVertex(const typename TypeConstInfo::VertexItrType& vertex) const
			{
				auto circulator = edge();
				for (size_t i = 0; i < 3; ++i)
				{
					if (circulator.from() == vertex)
						return true;
					circulator.moveNext();
				}
				return false;
			}
			std::array<size_t, 3> getVerticeIndices() const
			{
				std::array<size_t, 3> result;
				auto faceVs = meshVertices();
				for (size_t i = 0; i < 3; ++i)
				{
					result[i] = faceVs[i].index();
				}
				return result;
			}

			std::array<float, 3> sortZ() const
			{
				std::array<float, 3> orderedZ;
				auto mfVertices = meshVertices();
				for (int i = 0; i < mfVertices.size(); i++) {
					orderedZ[i] = mfVertices[i].worldPosition().z();
				}
				std::sort(orderedZ.begin(), orderedZ.end());
				return orderedZ;
			}

			float getFaceZmin() const
			{
				float face__z_min = std::numeric_limits<float>::max();
				auto mfVertices = meshVertices();
				for (int i = 0; i < mfVertices.size(); i++) {
					float temp__z_min = mfVertices[i].worldPosition().z();
					if (temp__z_min < face__z_min)
						face__z_min = temp__z_min;
				}
				return face__z_min;
			}

			float getFaceZmax() const
			{
				float face__z_max = std::numeric_limits<float>::lowest();
				auto mfVertices = meshVertices();
				for (int i = 0; i < mfVertices.size(); i++) {
					float temp__z_max = mfVertices[i].worldPosition().z();
					if (temp__z_max > face__z_max)
						face__z_max = temp__z_max;
				}
				return face__z_max;
			}
			bool isWrongOrientation()
			{
				auto hEdge = edge();
				for (size_t i = 0; i < 3; ++i)
				{
					if (!hEdge.nonTwins().empty())
						return true;
					hEdge.moveNext();
				}
				return false;
			}
			bool getEdgeWithVertices(typename TypeConstInfo::HalfEdgeItrType& result, const typename TypeConstInfo::VertexItrType& a, const typename TypeConstInfo::VertexItrType& b) const
			{
				auto hEdge = edge();
				for (size_t i = 0; i < 3; ++i)
				{
					auto to = hEdge.to();
					auto from = hEdge.from();
					if ((to == a && from == b) || (to == b && from == a))
					{
						result = hEdge;
						return true;
					}
					hEdge.moveNext();
				}
				return false;
			}

			bool isNextVtx(const typename TypeConstInfo::VertexItrType& prev, const typename TypeConstInfo::VertexItrType& next) const
			{
				auto hEdge = edge();
				for (size_t i = 0; i < 3; ++i)
				{

					if (hEdge.from() == prev)
					{
						if (hEdge.to() == next)
						{
							return true;
						}
						break;
					}
					hEdge.moveNext();
				}
				return false;
			}

			bool isNeighborOf(const typename TypeConstInfo::FaceItrType& other) const
			{

				auto hEdge = edge();
				for (size_t i = 0; i < 3; ++i)
				{
					auto nFaces = hEdge.twinFaces();
					for (auto& each : nFaces)
					{
						if (each == other)
						{
							return true;
						}
					}
				}
				return false;
			}
			std::unordered_set<FaceItrType> neighborFaces()const
			{
				std::unordered_set<FaceItrType> faces;
				auto hEdge = edge();
				for (size_t i = 0; i < 3; ++i)
				{
					auto nFaces = hEdge.twinFaces();
					faces.merge(std::move(nFaces));
					hEdge.moveNext();
				}
				return faces;
			}

			std::unordered_set<FaceItrType> findAllConnected()const
			{
				std::unordered_set<FaceItrType> explored;
				std::deque< FaceItrType>frontier;
				frontier.push_back(*static_cast<const FaceItrType*>(this));
				explored.insert(*static_cast<const FaceItrType*>(this));
				while (!frontier.empty())
				{
					auto curr = frontier.front();
					frontier.pop_front();
					auto neigborFaces = curr.neighborFaces();

					for (auto& each : neigborFaces)
					{
						if (explored.find(each) == explored.cend())
						{
							frontier.push_back(each);
							explored.insert(each);
						}
					}
				}
				return explored;
			}

			//for degenerative situation when two faces are connected only from a single shared vertex
			bool isButterfly(const typename TypeConstInfo::FaceItrType& other, const typename TypeConstInfo::VertexItrType& common)const
			{
				std::unordered_set<FaceItrType> explored;
				std::deque< FaceItrType>frontier;
				frontier.push_back(*static_cast<const FaceItrType*>(this));
				explored.insert(*static_cast<const FaceItrType*>(this));
				while (!frontier.empty())
				{
					auto curr = frontier.front();
					frontier.pop_front();
					auto neigborFaces = curr.neighborFaces();

					for (auto& each : neigborFaces)
					{
						if (each.hasVertex(common))
						{

							//early check
							if (each == other)
							{
								return false;
							}
							else
							{
								if (explored.find(each) == explored.cend())
								{
									frontier.push_back(each);
									explored.insert(each);
								}
							}
						}
					}
				}
				return true;
			}

		};


		//const, non const specializations
		class HalfEdgeConstItr : public HalfEdgeItrBase<HalfEdgeConstItr, const HalfEdge, ConstItrInfo>
		{
		public:
			using HalfEdgeItrBase::HalfEdgeItrBase;
		};
		class HalfEdgeItr : public HalfEdgeItrBase<HalfEdgeItr, HalfEdge, NonConstItrInfo>
		{
		public:
			using HalfEdgeItrBase::HalfEdgeItrBase;
		};
		class VertexConstItr : public VertexItrBase<VertexConstItr, const MeshVertex, ConstItrInfo>
		{
		public:
			using VertexItrBase::VertexItrBase;
		};
		class VertexItr : public VertexItrBase<VertexItr, MeshVertex, NonConstItrInfo>
		{
		public:
			using VertexItrBase::VertexItrBase;
		};
		class FaceConstItr : public FaceItrBase<FaceConstItr, const MeshFace, ConstItrInfo>
		{
		public:
			using FaceItrBase::FaceItrBase;
		};
		class FaceItr : public FaceItrBase<FaceItr, MeshFace, NonConstItrInfo>
		{
		public:
			using FaceItrBase::FaceItrBase;
		};


	}
}