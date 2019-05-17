#ifndef MESH_H
#define MESH_H
#include <vector>
#include <Qt3DCore/qtransform.h>
#include <QVector3D>
#include <QHash>
#include "configuration.h"
#include "polyclipping/clipper/clipper.hpp"
#include "polyclipping/poly2tri/poly2tri.h"
#include <QTransform>
#include <QTime>
#include <array>
#include <variant>
#include "../common/TrackedIndexedList.h"
#define cos50 0.64278761
#define cos100 -0.17364818
#define cos150 -0.8660254
#define FZERO 0.00001f
#if defined(_DEBUG) || defined(QT_DEBUG )
#endif
using namespace ClipperLib;
namespace Hix
{
	namespace Engine3D
	{
		// plane contains at least 3 vertices contained in the plane in clockwise direction
		typedef std::vector<QVector3D> Plane;
		struct Vertex;
		struct Face;
		struct Mesh;
		typedef typename TrackedIndexedList<Vertex>::const_iterator VertexConstItr;
		typedef typename TrackedIndexedList<Face>::const_iterator FaceConstItr;


		struct Face {
			Face() {}
			QVector3D fn;
			QVector3D fn_unnorm;
			//TODO: fix meshes with more than 3 faces that share an edge
			std::array<VertexConstItr, 3> mesh_vertex;
			std::array<std::vector<FaceConstItr>, 3> neighboring_faces;
		};

		struct Vertex {

			Vertex() {}
			Vertex(QVector3D position) : position(position) {}
			friend inline bool operator== (const Vertex& a, const Vertex& b) {
				return a.position == b.position;
			}
			friend inline bool operator!= (const Vertex& a, const Vertex& b) {
				return a.position != b.position;
			}
			void calculateNormalFromFaces();
			QVector3D position;
			QVector3D vn;
			std::vector<FaceConstItr> connected_faces;
		};


		class Path3D : public std::vector<Vertex> {
		public:
			Path projection;
			std::vector<Path3D> inner;
			std::vector<Path3D> outer;
		};

		typedef std::vector<Path3D> Paths3D;

		class Mesh {
		public:
			Mesh();
			Mesh(const Mesh* origin);

			/********************** Undo state functions***********************/
			void setNextMesh(Mesh* mesh);
			void setPrevMesh(Mesh* mesh);
			Mesh* saveUndoState(const Qt3DCore::QTransform& transform);


			/********************** Mesh Edit Functions***********************/
			void vertexOffset(float factor);
			void vertexMove(QVector3D direction);
			void centerMesh();
			void vertexRotate(QMatrix4x4 tmpmatrix);
			void vertexScale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY);
			void reverseFaces();
			void addFace(QVector3D v0, QVector3D v1, QVector3D v2);
			void addFace(QVector3D v0, QVector3D v1, QVector3D v2, const Face* parentface);
			TrackedIndexedList<Face>::const_iterator removeFace(std::list<Face>::const_iterator f_it);
			void connectFaces();
			TrackedIndexedList<Vertex>& getVertices();
			TrackedIndexedList<Face>& getFaces();


			/********************** Mesh Modify and Copy Functions***********************/
			Mesh* copyMesh()const;
			Mesh* vertexMoved(QVector3D direction)const;


			/********************** Path Generation Functions **********************/
			static void addPoint(float x, float y, Path* path);
			Paths3D intersectionPaths(Path Contour, Plane target_plane)const;
			Path3D intersectionPath(Plane base_plane, Plane target_plane)const;
			Path intersectionPath(Face mf, float z)const;

			/********************** Helper Functions **********************/
			static void updateMinMax(QVector3D v, std::array<float, 6>& minMax);
			static std::array<float, 6> calculateMinMax(QMatrix4x4 rotmatrix, const Mesh* mesh);

			float getFaceZmin(Face mf)const;
			float getFaceZmax(Face mf)const;
			//Face idx2MF(int idx)const;
			//Vertex idx2MV(int idx)const;

			/********************** Getters **********************/
			//const getter
			const TrackedIndexedList<Vertex>& getVertices()const;
			const TrackedIndexedList<Face>& getFaces()const;
			float x_min()const;
			float x_max()const;
			float y_min()const;
			float y_max()const;
			float z_min()const;
			float z_max()const;
			Mesh* getPrev()const;
			Mesh* getNext()const;

			/********************** index to data **********************/

			inline Face& idx2mf(size_t idx)
			{
				return faces[idx];
			}
			inline Vertex& idx2vtx(size_t idx)
			{
				return vertices[idx];
			}

			inline const Face& idx2mf(size_t idx)const
			{
				return faces[idx];
			}
			inline const Vertex& idx2vtx(size_t idx)const
			{
				return vertices[idx];
			}

			/********************** Stuff that can be public **********************/

			QTime time;
			QVector3D m_translation;
			QMatrix4x4 m_matrix;


		private:
			/********************** Helper Functions **********************/
			std::vector<const Face*> findFaceWith2Vertices(const Vertex& v0, const Vertex& v1, const Face& self_f) const;
			VertexConstItr addFaceVertex(QVector3D v);
			void updateMinMax(QVector3D v);

			QHash<uint32_t, const Vertex*> vertices_hash;

			TrackedIndexedList<Vertex> vertices;
			TrackedIndexedList<Face> faces;

			// for undo & redo
			Mesh* prevMesh = nullptr;
			Mesh* nextMesh = nullptr;

			//index changed event callback
			void vtxIndexChangedCallback(size_t oldIdx, size_t newIdx);
			void faceIndexChangedCallback(size_t oldIdx, size_t newIdx);

			float _x_min = 99999, _x_max = 99999, _y_min = 99999, _y_max = 99999, _z_min = 99999, _z_max = 99999;

			//fileloader should be a factory pattern?
			friend class FileLoader;
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
		uint32_t Vertex2Hash(Vertex u);

	};
};


#endif // MESH_H
