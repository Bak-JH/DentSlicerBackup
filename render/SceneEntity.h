#pragma once


#include <QObject>
#include <QApplication>
#include <Qt3DCore>
#include <Qt3DRender>
#include <Qt3DExtras>
#include <Qt3DInput>
#include <QBuffer>
#include <memory>
#include "DentEngine/src/mesh.h"
#include "render/ModelMaterial.h"
namespace Hix
{
	namespace Render
	{
		using namespace Qt3DCore;
		using namespace Qt3DRender;
		using namespace Qt3DExtras;
		using namespace Hix::Engine3D;
		class SceneEntity : public QEntity
		{
			Q_OBJECT
		public:

			//size of QGeometry Attribute elements
			const static size_t POS_SIZE = 3; //x, y, z of position
			const static size_t NRM_SIZE = 3; //x, y, z of normal
			const static size_t COL_SIZE = 3; //x, y, z of normal
			const static size_t VTX_SIZE = (POS_SIZE + NRM_SIZE + COL_SIZE) * sizeof(float);

			const static size_t IDX_SIZE = 3; //3 indices to vertices
			const static size_t UINT_SIZE = sizeof(uint); //needs to be large enough to accomodate all range of vertex index
			const static size_t FACE_SIZE = IDX_SIZE * UINT_SIZE;
			// load teeth model default
			SceneEntity(QEntity* parent = nullptr); // main constructor for mainmesh and shadowmesh
			virtual ~SceneEntity();
			void setMesh(Mesh* mesh);
			void clearMesh();
			//for now, one sceneEntity per mesh, no setter for mesh in sceneEntity
			const Hix::Engine3D::Mesh* getMesh()const;
			const Qt3DCore::QTransform& transform() const;
			Qt3DCore::QTransform& transform();
			QVector3D toParentCoord(const QVector3D& pos)const;
			//world coordinates.
			QVector3D toRootCoord(const QVector3D& vtx)const;

			//update faces given indicies, if index >= indexUppderLimit, it's ignored
			void updateFaces(const std::unordered_set<size_t>& faceIndicies, const Hix::Engine3D::Mesh& mesh);
			void updateVertices(const std::unordered_set<size_t>& vtxIndicies, const Hix::Engine3D::Mesh& mesh);
			void updateEntireMesh(Hix::Engine3D::Mesh* mesh);
			void updateMesh(Hix::Engine3D::Mesh* mesh, bool force = false);
			void appendIndexArray(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);
			virtual void appendMeshVertex(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);


		protected:

			Qt3DCore::QTransform _transform;
			void clearMem();
			

			//Order is important! Look at the initializer list in constructor
			QGeometryRenderer m_geometryRenderer;
			QGeometry m_geometry;

			//3 vectors per data, each for position, normal, color
			Qt3DRender::QBuffer vertexBuffer;
			//defines mesh faces by 3 indices to the vertexArray
			Qt3DRender::QBuffer indexBuffer;
			//separates SSBO buffer for Per-primitive colors
			//Qt3DRender::QBuffer primitiveColorBuffer;
			QAttribute positionAttribute;
			QAttribute normalAttribute;
			QAttribute colorAttribute;
			QAttribute indexAttribute;


			//QVariantList _primitiveColorCodes;
			QEntity* m_parent;

			// Core mesh structures
			Hix::Engine3D::Mesh* _mesh = nullptr; 

			/***************Ray casting and hit test***************/
			bool _targetSelected = false;
			FaceConstItr targetMeshFace; // used for object selection (specific area, like extension or labelling)

		};
	}
}
