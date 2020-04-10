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
#include "DentEngine/src/Bounds3D.h"
namespace Hix
{
	namespace Render
	{
		using namespace Qt3DRender;
		using namespace Qt3DExtras;

		template<typename OwnerPtr, typename Fn>
		void callFunctorRecursive(OwnerPtr owner, const Fn& callable)
		{
			callable(owner);
			for (auto child : owner->childNodes())
			{
				auto childEntity = dynamic_cast<OwnerPtr>(child);
				if (childEntity)
				{
					callFunctorRecursive(childEntity, callable);
				}
			}
		}

		class SceneEntity : public Qt3DCore::QEntity
		{
			Q_OBJECT
		public:
			//size of QGeometry Attribute elements
			const static size_t POS_SIZE = 3; //x, y, z of position
			const static size_t COL_SIZE = 4; //rgb color
			const static size_t VTX_SIZE = (POS_SIZE + COL_SIZE) * sizeof(float);

			const static size_t IDX_SIZE = 3; //3 indices to vertices
			const static size_t UINT_SIZE = sizeof(uint); //needs to be large enough to accomodate all range of vertex index
			const static size_t FACE_SIZE = IDX_SIZE * UINT_SIZE;
			// load teeth model default
			SceneEntity(Qt3DCore::QEntity* parent = nullptr); // main constructor for mainmesh and shadowmesh
			virtual ~SceneEntity();
			void setMesh(Hix::Engine3D::Mesh* mesh);

			void clearMesh();
			//for now, one sceneEntity per mesh, no setter for mesh in sceneEntity
			const Hix::Engine3D::Mesh* getMesh()const;
			Hix::Engine3D::Mesh* getMeshModd();

			const Qt3DCore::QTransform& transform() const;
			QVector4D toParentCoord(const QVector4D& childPos)const;
			QVector4D fromParentCoord(const QVector4D& parentPos)const;

			//world coordinates.
			QMatrix4x4 toRootMatrix() const;
			QMatrix4x4 toLocalMatrix() const;
			QVector4D toRootCoord(const QVector4D& local)const;
			QVector4D toLocalCoord(const QVector4D& world)const;
			QVector3D ptToRoot(const QVector3D& local)const;
			QVector3D vectorToRoot(const QVector3D& local)const;
			QVector3D ptToLocal(const QVector3D& world)const;
			QVector3D vectorToLocal(const QVector3D& world)const;
			SceneEntity* parentSceneEntity()const;

			//update faces given indicies, if index >= indexUppderLimit, it's ignored
			void updateFaces(const std::unordered_set<size_t>& faceIndicies, const Hix::Engine3D::Mesh& mesh);
			void updateVertices(const std::unordered_set<size_t>& vtxIndicies, const Hix::Engine3D::Mesh& mesh);
			void updateEntireMesh(Hix::Engine3D::Mesh* mesh);
			void updateMesh(bool force = false);
			void appendIndexArray(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);
			virtual void appendMeshVertex(const Hix::Engine3D::Mesh* mesh,
				Hix::Engine3D::FaceConstItr begin, Hix::Engine3D::FaceConstItr end);

			//get this model's axis aligned bounding box
			const Hix::Engine3D::Bounds3D& aabb()const;
			Hix::Engine3D::Bounds3D& aabb();
			//get total AABB including it's children
			Hix::Engine3D::Bounds3D recursiveAabb()const;
			void updateRecursiveAabb();
			Qt3DCore::QTransform& transform();
			void getChildrenModels(std::unordered_set<const SceneEntity*>& results)const;



		protected:

			template<typename OwnerPtr, typename Fn, typename ...Args>
			void callRecursive(OwnerPtr owner, Fn functionPtr, Args... arguments)
			{
				for (auto child : owner->childNodes())
				{
					auto childEntity = dynamic_cast<OwnerPtr>(child);
					if (childEntity)
					{
						(childEntity->*functionPtr)(arguments...);
					}
				}
			}
			template<typename OwnerPtr, typename Fn, typename ...Args>
			void callRecursive(OwnerPtr owner, Fn functionPtr, Args... arguments)const
			{
				for (auto child : owner->childNodes())
				{
					auto childEntity = dynamic_cast<OwnerPtr>(child);
					if (childEntity)
					{
						(childEntity->*functionPtr)(arguments...);
					}
				}
			}
			//modifiable transform, cannot be accessed by other classes
			//Axis aligned bounding box
			Hix::Engine3D::Bounds3D _aabb;

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
			QAttribute colorAttribute;
			QAttribute indexAttribute;



			// Core mesh structures
			Hix::Engine3D::Mesh* _mesh = nullptr; 

			/***************Ray casting and hit test***************/

		};
	}
}
