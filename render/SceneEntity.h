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

			const Hix::Engine3D::Mesh* getMesh()const;
			const Qt3DCore::QTransform* getTransform() const;
			void setTranslation(const QVector3D& t);
			QVector3D getTranslation();
			void setMatrix(const QMatrix4x4& matrix);

			Qt3DRender::QLayer* getLayer();
			
			/***************Ray casting and hit test***************/
			void setHitTestable(bool isEnabled);
			bool isHitTestable();
		


		protected:

			void clearMem();

			Qt3DCore::QTransform m_transform;
			

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
			Hix::Engine3D::Mesh* _mesh;

			/***************Ray casting and hit test***************/
			bool _hitEnabled = false;
			bool _targetSelected = false;
			FaceConstItr targetMeshFace; // used for object selection (specific area, like extension or labelling)
			Qt3DRender::QLayer _layer;

		};
	}
}
