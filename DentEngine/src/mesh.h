#pragma once
#include <vector>
#include <Qt3DCore/qtransform.h>
#include <QVector3D>
#include <QHash>
#include "polyclipping/clipper/clipper.hpp"
#include "polyclipping/poly2tri/poly2tri.h"
#include "configuration.h"
#include <QTransform>
#include <QTime>
#include <array>
#include <variant>
#include "../../common/TrackedIndexedList.h"
#define cos50 0.64278761
#define cos100 -0.17364818
#define cos150 -0.8660254
#define FZERO 0.00001f
using namespace ClipperLib;

namespace Hix
{
	namespace Engine3D
	{

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
			std::vector<HalfEdgeConstItr> twins;

			std::vector<FaceConstItr> nonOwningFaces()const;
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
			QVector3D fn_unnorm;
			HalfEdgeConstItr edge;
			std::array<VertexConstItr, 3> meshVertices()const;
			//std::array<std::vector<FaceConstItr>, 3> neighboring_faces;
			HalfEdgeCirculator edgeCirculator()const;
			std::array<size_t, 3> getVerticeIndices(const Mesh* owningMesh)const;

		};
		struct MeshVertex {
			MeshVertex() 
			{}
			MeshVertex(QVector3D position, QVector3D color): position(position), color(color) {}
			friend inline bool operator== (const MeshVertex& a, const MeshVertex& b) {
				return a.position == b.position;
			}
			friend inline bool operator!= (const MeshVertex& a, const MeshVertex& b) {
				return a.position != b.position;
			}
			bool empty()const;
			void calculateNormalFromFaces();
			std::vector<FaceConstItr> connectedFaces()const;
			QVector3D position;
			QVector3D vn;
			QVector3D color;
			std::vector<HalfEdgeConstItr> leavingEdges;
			std::vector<HalfEdgeConstItr> arrivingEdges;


		};





		typedef std::vector<QVector3D> Plane;

		class Path3D : public std::vector<MeshVertex> {
		public:
			Path projection;
			std::vector<Path3D> inner;
			std::vector<Path3D> outer;
		};

		typedef std::vector<Path3D> Paths3D;

		class Mesh {
		public:
			Mesh();
			//THIS IS NOT A COPY CONSTRUCTOR!
			Mesh(const Mesh* origin);
			//this is
			Mesh(const Mesh&);
			//copy assign
			//Mesh& operator=(const Mesh o);
			/********************** Undo state functions***********************/
			void setNextMesh(Mesh* mesh);
			void setPrevMesh(Mesh* mesh);
			Mesh* saveUndoState(const Qt3DCore::QTransform& transform);


			/********************** Mesh Edit Functions***********************/
			void setVerticesColor(QVector3D color);
			void vertexOffset(float factor);
			void vertexMove(QVector3D direction);
			void centerMesh();
			void vertexRotate(QMatrix4x4 tmpmatrix);
			void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
			void reverseFace(FaceConstItr faceItr);
			void reverseFaces();
			void addFace(QVector3D v0, QVector3D v1, QVector3D v2, QVector3D color = COLOR_DEFAULT_MESH);
			TrackedIndexedList<MeshFace>::const_iterator removeFace(FaceConstItr f_it);
			void connectFaces();
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


			/********************** Path Generation Functions **********************/
			static void addPoint(float x, float y, Path* path);
			Paths3D intersectionPaths(Path Contour, Plane target_plane)const;
			Path3D intersectionPath(Plane base_plane, Plane target_plane)const;
			Path intersectionPath(MeshFace mf, float z)const;

			/********************** Helper Functions **********************/
			static void updateMinMax(QVector3D v, std::array<float, 6>& minMax);
			static std::array<float, 6> calculateMinMax(QMatrix4x4 rotmatrix, const Mesh* mesh);

			float getFaceZmin(MeshFace mf)const;
			float getFaceZmax(MeshFace mf)const;
			//MeshFace idx2MF(int idx)const;
			//MeshVertex idx2MV(int idx)const;

			/********************** Getters **********************/
			//const getter
			const TrackedIndexedList<MeshVertex>& getVertices()const;
			const TrackedIndexedList<MeshFace>& getFaces()const;
			const TrackedIndexedList<HalfEdge>& getHalfEdges()const;

			float x_min()const;
			float x_max()const;
			float y_min()const;
			float y_max()const;
			float z_min()const;
			float z_max()const;
			Mesh* getPrev()const;
			Mesh* getNext()const;

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

			inline size_t indexOf(HalfEdgeConstItr& itr)const
			{
				return itr - halfEdges.cbegin();
			}
			inline size_t indexOf(VertexConstItr& itr)const
			{
				return itr - vertices.cbegin();
			}
			inline size_t indexOf(FaceConstItr& itr)const
			{
				return itr - faces.cbegin();
			}
			/********************** Stuff that can be public **********************/

			QTime time;
			QVector3D m_translation;
			QMatrix4x4 m_matrix;
			/***********************for undo redo search*************************/
			QTime getPrevTime();
			QTime getNextTime();

		private:
			/********************** Helper Functions **********************/

			void setTwins(HalfEdgeItr edge);
			VertexItr addOrRetrieveFaceVertex(QVector3D v, QVector3D color);
			void removeVertexHash(QVector3D pos);
			VertexConstItr getSimilarVertex(uint32_t digest, QVector3D v);
			void addHalfEdgesToFace(std::array<VertexItr, 3> faceVertices, FaceConstItr face);
			void updateMinMax(QVector3D v);

			QHash<uint32_t, VertexConstItr> vertices_hash;
			std::vector< VertexConstItr> __DEBUGItr;
			std::vector< const MeshVertex* > __DEBUGPtr;

			TrackedIndexedList<MeshVertex> vertices;
			TrackedIndexedList<HalfEdge> halfEdges;
			TrackedIndexedList<MeshFace> faces;

			// for undo & redo
			Mesh* prevMesh = nullptr;
			Mesh* nextMesh = nullptr;

			//index changed event callback
			void vtxIndexChangedCallback(size_t oldIdx, size_t newIdx);
			void faceIndexChangedCallback(size_t oldIdx, size_t newIdx);
			void hEdgeIndexChangedCallback(size_t oldIdx, size_t newIdx);

			float _x_min = 99999, _x_max = 99999, _y_min = 99999, _y_max = 99999, _z_min = 99999, _z_max = 99999;

		};


		uint32_t vertexHash(QVector3D v);

		QHash<uint32_t, Path>::iterator findSmallestPathHash(QHash<uint32_t, Path> pathHash);

		// construct closed contour using segments created from identify step
		Paths contourConstruct(Paths);
		Paths3D contourConstruct3D(Paths3D hole_edges);

		bool intPointInPath(IntPoint ip, Path p);
		bool pathInpath(Path3D target, Path3D in);

		std::vector<std::array<QVector3D, 3>> interpolate(Path3D from, Path3D to);

		uint32_t intPoint2Hash(IntPoint u);
		uint32_t Vertex2Hash(MeshVertex& u);

	};
};
