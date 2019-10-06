#pragma once
#include "../../common/RandomAccessIteratorBase.h"
#include <vector>
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
			typedef const class Mesh OwnerType;
		};
		struct NonConstItrInfo
		{
			typedef class HalfEdgeItr HalfEdgeItrType;
			typedef class VertexItr VertexItrType;
			typedef class FaceItr FaceItrType;
			typedef class Mesh OwnerType;
		};

		//actual base for edge, vtx, face iterators, specialized later for constness.
		template <typename HEItrType, typename ValType, typename TypeConstInfo>
		class HalfEdgeItrBase : public RandomAccessIteratorBase< HEItrType, typename TypeConstInfo::OwnerType>
		{
		public:
			using RandomAccessIteratorBase<HEItrType, typename TypeConstInfo::OwnerType>::RandomAccessIteratorBase;
			using ParentType = RandomAccessIteratorBase< HEItrType, typename TypeConstInfo::OwnerType>;
			using PtrType = ValType *;
			using RefType=  ValType &;

			RefType ref()const
			{
				return ParentType::_owner->getHalfEdges()(ParentType::_index);
			}
			RefType operator*() const
			{
				return ref();
			}
			PtrType operator->() const
			{
				return &(ParentType::_owner->getHalfEdges()(ParentType::_index));
			}
			HEItrType next()const
			{
				return HEItrType(ref().next, ParentType::_owner);
			}
			HEItrType prev()const
			{
				return HEItrType(next().next().index(), ParentType::_owner);
			}
			HEItrType moveNext()
			{
				ParentType::_index = ref().next;
			}
			HEItrType movePrev()
			{
				ParentType::_index = next().next().index();
			}
			typename TypeConstInfo::VertexItrType from()const
			{
				TypeConstInfo::VertexItrType(ref().from, ParentType::_owner);
			}
			typename TypeConstInfo::VertexItrType to()const
			{
				TypeConstInfo::VertexItrType(ref().to, ParentType::_owner);
			}
			typename TypeConstInfo::FaceItrType owningFace()const
			{
				TypeConstInfo::FaceItrType(ref().owningFace, ParentType::_owner);
			}
			//HalfEdgeConstItr twin;
			std::unordered_set<HEItrType> twins()const
			{
				std::unordered_set<HEItrType> twinEdges;
				auto fromVtx = from();
				for (auto oppDirEdge : to().leavingEdges())
				{
					if (oppDirEdge.to() == fromVtx)
					{
						twinEdges.insert(oppDirEdge);
					}
				}
				return twinEdges;
			}
			//twins in same direction
			std::unordered_set<HEItrType> nonTwins()const
			{
				std::unordered_set<HEItrType> nonTwins;
				auto toVtx = to();
				for (auto sameDirEdge : from().leavingEdges())
				{
					if (sameDirEdge.to() == toVtx && sameDirEdge != *this)
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

		template <typename VtxItrType, typename ValType, typename TypeConstInfo>
		class VertexItrBase : public RandomAccessIteratorBase< VtxItrType, typename TypeConstInfo::OwnerType>
		{
		public:
			using RandomAccessIteratorBase<VtxItrType, typename TypeConstInfo::OwnerType>::RandomAccessIteratorBase;
			using ParentType = RandomAccessIteratorBase< VtxItrType, typename TypeConstInfo::OwnerType>;

			using PtrType = ValType *;
			using RefType=  ValType &;

			RefType operator*() const
			{
				return ParentType::_owner->getVertices()(ParentType::_index);
			}
			PtrType operator->() const
			{
				return &(ParentType::_owner->getVertices()(ParentType::_index));
			}


			const QVector3D& vn() const
			{
				QVector3D vn;
				auto faces = connectedFaces();
				for (auto& face : faces)
				{
					vn += face.fn();
				}
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

			bool disconnected() const
			{
				if (ref().leavingEdges.empty() && ref().arrivingEdges.empty())
					return true;
				else
					false;
			}

			std::vector<FaceConstItr> connectedFaces() const
			{
				std::vector<FaceConstItr> result;
				for (auto each : leavingEdges())
				{
					result.emplace_back(each.owningFace());
				}
				return result;
			}



			const QVector3D& position() const
			{
				return ref().position;
			}


		};

		template <typename FaceItrType, typename ValType, typename TypeConstInfo>
		class FaceItrBase : public RandomAccessIteratorBase< FaceItrType, typename TypeConstInfo::OwnerType>
		{
		public:
			using RandomAccessIteratorBase<FaceItrType, typename TypeConstInfo::OwnerType>::RandomAccessIteratorBase;
			using ParentType = RandomAccessIteratorBase< FaceItrType, typename TypeConstInfo::OwnerType>;

			using PtrType = ValType *;
			using RefType=  ValType &;
			RefType operator*() const
			{
				return ParentType::_owner->getFaces()(ParentType::_index);
			}
			PtrType operator->() const
			{
				return &(ParentType::_owner->getFaces()(ParentType::_index));
			}
			const QVector3D& fn() const
			{
				auto fVtx = meshVertices();
				QVector3D fn = QVector3D::normal(fVtx[0].position(), fVtx[1].position(), fVtx[2].position());
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
					orderedZ[i] = mfVertices[i].position().z();
				}
				std::sort(orderedZ.begin(), orderedZ.end());
				return orderedZ;
			}

			float getFaceZmin() const
			{
				float face__z_min = std::numeric_limits<float>::max();
				auto mfVertices = meshVertices();
				for (int i = 0; i < mfVertices.size(); i++) {
					float temp__z_min = mfVertices[i].position().z();
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
					float temp__z_max = mfVertices[i].position().z();
					if (temp__z_max > face__z_max)
						face__z_max = temp__z_max;
				}
				return face__z_max;
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

			bool isNeighborOf(const FaceConstItr& other) const
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