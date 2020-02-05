#pragma once
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <qvector3d.h>
#include "../DentEngine/src/Bounds3D.h"
class GLModel;
namespace Qt3DCore
{
	class QEntity;
}
namespace Hix
{
	namespace QML
	{
		class PartList;
	}
	namespace Application
	{
		class ApplicationManager;
		class PartManager
		{
		public:
			PartManager();
			void addPart(std::unique_ptr<GLModel>&& model);
			std::unique_ptr<GLModel> removePart(GLModel* model);
			void isSelected(GLModel* model);
			void selectPart(GLModel* model);
			void selectAll();
			void unselectAll();
			std::unordered_set<GLModel*> selectedModels()const;
			std::unordered_set<GLModel*> allModels()const;
			Hix::Engine3D::Bounds3D selectedBound()const;
			QVector3D getSelectedCenter()const;
			QVector3D selectedModelsLengths()const;

		private:
			Qt3DCore::QEntity* _root;
			std::unordered_map<GLModel*, std::unique_ptr<GLModel>> _models;
			Hix::QML::PartList* _partList;
			friend class PartManagerLoader;
		};
		class PartManagerLoader
		{
		private:
			static void init(PartManager& manager, Qt3DCore::QEntity* entity);
			friend class Hix::Application::ApplicationManager;
		};




	}
}
